/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003  
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/* Desc: OpenGL context wrapper  
 * Author: Andrew Howard
 * Date: 7 Oct 2004
 * CVS: $Id: GLContext.cc,v 1.20 2005/04/26 18:29:46 natepak Exp $
 */

/** @page gazebo_opengl OpenGL Support

Gazebo uses OpenGL to render images for both user feedback (ObserverCam
model) and simulated cameras (e.g., MonoCam, StereoHead,
SonyVID30 models).  This document describes OpenGL concepts with which
users should be familiar, and lists some limitations of particular
OpenGL driver implementations.

@par Offscreen rendering and the renderMethod attribute

With version 0.5, Gazebo has moved to an off-screen rendering model;
i.e., camera images are rendered to memory rather than the screen.
This approach has a number of significant advantages:

- Support for console mode operation (no GUI; good for batch jobs).

- Support for stereo cameras (with simulated disparity maps).

- No weird X window manager artifacts in simulated camera images (as
was the case with Gazebo 0.4).

Unfortunately, hardware/driver support for (accelerated) offscreen
rendering currently seems to be very patchy, with at least four
different methods available:

-# @b XLIB : Render to an unmapped X window.  This is just a sneaky way of using
   the standard on-screen rendering pipeline (to a hidden window).
   - Pros: hardware accelerated
   - Cons: only works on some X11 implementations (e.g. works on OS X,
     but not on Linux/Xorg).
-# @b SGIX : Use the SGIX pbuffer extension (which was designed specifically for
   this purpose).
   - Pros: hardware accelerated
   - Cons: only available on some chipsets (works on NVidia; not sure
     about ATI).
-# @b GLX : Use the GLX pixmap extension (write to offscreen pixmap).
   - Pros: standard API.
   - Cons: no hardware acceleration.
-# @b GLXP : Use the GLX pbuffer API in GLX 1.3.
   - Pros: standard API
   - Cons: not supported on some drivers?

Since different platforms support different solutions, Gazebo
implements methods 1, 2 and 3, selectable via the @c renderMethod
attribute in the camera models.  Users can also select the @b AUTO
method, which automatically cycles though the methods to find one that
works.


@par Hardware compatability list

Due to the off-screen rendering issues described above, Gazebo
is not compatable with certain hardware/software combinations.
Known working (and not working) combinations are listed below.

- Linux/nVidia
  - Gentoo Linux, XOrg X11
  - nVidia GeForce (various) with nVidia binary driver
  - Supports: GLXP (accelerated), SGIX (accelerated), GLX (non-accelerated)
  - Notes: XLIB produces corrupted images

- Linux/ATI
  - Gentoo Linux, XOrg X11
  - ATI ? with ATI flgrx driver
  - Supports: GLXP (accelerated), GLX (non-accelerated)
  - Notes: XLIB produces corrupt images; SGIX crashes Gazebo.
  
- OSX/nVidia
  - Apple OS X 10.3, Apple X11 1.0
  - nVidia with Apple drivers
  - Supports: XLIB (accelerated), GLX (non-accelerated)

Please let us know of any non-working combinations.
  

@par Performance tips

Gazebo can be very CPU intensive, with almost all of the cycles going
into rendering or collision detection.  Here are some tips for making
rendering faster:

- To good approximation, rendering time is proportional to the the
  number cameras (since each camera must render the scene), the size
  of the camera image(s), and the camera update rate(s).  Reducing any
  or all of these will increase rendering speed significantly.

- Rendering time scales with scene complexity (number of vertices).
  Some models are particularly vertex rich, and must be used
  sparingly.  A good way to check the scene complexity is to force
  wire frame rendering using the @c polygonFill attribute; this will
  show all of the triangles in the scene.

- Re-generate terrains with a sparser grid and/or larger error bound;
  this can dramatically reduce the number of vertices in a terrain.

- Turn off skins or re-create the skin model with a reduced set of
  vertices.


@par Camera frustrums and clip planes

Gazebo uses the standard OpenGL perspective projection (pin-hole
camera).  The field-of-view for such cameras is defined by four
attributes:

- The horizontal field of view.
- The aspect ratio (image width/height).
- Near and far clip planes.

The easiest way to understand these attributes is through the notion
of a frustrum (depicted below).

@image html frustrum.gif "Camera frustrum"

The frustrum defines the viewable volume in three dimensions, with the
near and far clip attributes bounding the depth of the volume (i.e.,
the camera will only see points whose depth lies between the near and
far clip values).

Clip values also have a second function: the ratio nearClip/farClip
controls the effective resolution of the Z-buffer used by OpenGL for
hidden surface removal.  See the OpenGL documentation for details, but
suffice to say setting the near clip to zero and the far clip to
infinity is a @b bad @b idea.  Lowering this ratio will lead to better
quality rendering.


*/
/** @} */


#if HAVE_CONFIG_H
  #include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

#include "Error.hh"
#include "GLContext.hh"


//////////////////////////////////////////////////////////////////////////////
// Global vars
extern Display *display;
extern bool optUseXlib;


//////////////////////////////////////////////////////////////////////////////
// Keep track of the total number of contexts
static int contextCount = 0;


//////////////////////////////////////////////////////////////////////////////
// Constructor 
GLContext::GLContext()
{
  this->contextIndex = contextCount++;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
GLContext::~GLContext()
{
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the camera
int GLContext::Init(int width, int height, int color, int alpha, int depth, const char *method, GLXContext shareList)
{
/////////////////////////////////////////////////////////////////////////
// COMENTADO TRECHO DE CODIGO NAO USADO PARA FACILITAR O REUSO DO CODIGO
/////////////////////////////////////////////////////////////////////////
//   this->reqWidth = width;
//   this->reqHeight = height;
//   this->reqColor = color;
//   this->reqAlpha = alpha;
//   this->reqDepth = depth;
//   
//   if (::display == NULL)
//   {
//     PRINT_ERR("X display not set");
//     return -1;
//   }
// 
//   // Try SGIX pbuffer rendering
//   if (strcasecmp(method, "SGIX") == 0)
//   {
//     if (InitSGIX(shareList) != 0)
//       return -1;
//   }
//   else if (strcasecmp(method, "GLXP") == 0)
//   {
//     if (InitGLXP(shareList))
//       return -1;
//   }
//   // Try GLX pixmap rendering
//   else  if (strcasecmp(method, "GLX") == 0)
//   {
//     if (InitGLXPixmap(shareList) != 0)
//       return -1;
//   }
// 
//   // Try Xlib rendering
//   else if (strcasecmp(method, "XLIB") == 0)
//   {
//     if (InitXlib(shareList) != 0)
//       return -1;
//   }
// 
//   else
//   {
// #ifdef PLATFORM_OSX
// 
//     // On OS X, we try Xlib rendering first (no pbuffer support), the
//     // fall back to GLX pixbuff rendering.
//     if (InitXlib(shareList) != 0)
//     {
//       if (InitGLXPixmap(shareList) != 0)
//       {
//         PRINT_ERR("unable to initialize GL context");
//         return -1;
//       }
//     }
// 
// #else
// 
//     // On Linux we try GLXP hardware rendering first, then
//     // fall back to SGIX hardware rendering, then fallback
//     // to GLX pixbuff.  Xlib rendering doesnt seem
//     // to work on Linux (needs more testing).
//     if (InitGLXP(shareList) != 0)
//     {
//       if (InitSGIX(shareList) != 0)
//       {      
//         if (InitGLXPixmap(shareList) != 0)
//         {
//           PRINT_ERR("unable to initialize GL context");
//           return -1;
//         }
//       }
//     }
//     
// #endif
//   }

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Create unmapped Xlib window for offscreen rendering
int GLContext::InitXlib(GLXContext shareList)
{
/////////////////////////////////////////////////////////////////////////
// COMENTADO TRECHO DE CODIGO NAO USADO PARA FACILITAR O REUSO DO CODIGO
/////////////////////////////////////////////////////////////////////////
//   XVisualInfo *visual;
//   Colormap cmap;
//   XSetWindowAttributes swa;
//   Window win;
//   GLXContext cx;
//   // REMOVE XEvent event;
// 
//   int attrCount;
//   int attrList[32];
// 
//   // TODO: honor color bits request
//   attrCount = 0;
//   attrList[attrCount++] = GLX_RGBA;
//   attrList[attrCount++] = GLX_DEPTH_SIZE;
//   attrList[attrCount++] = this->reqDepth;
//   attrList[attrCount++] = GLX_DOUBLEBUFFER;
//   attrList[attrCount++] = None;
//   
//   /* get an appropriate visual */
//   visual = glXChooseVisual(::display, DefaultScreen(::display), attrList);  
//   if (!visual)
//   {
//     PRINT_MSG0(2, "unable to get suitable visual");
//     return -1;
//   }
//   
//   /* create a GLX context */
//   cx = glXCreateContext(::display, visual, shareList, GL_TRUE);
//   if (!cx)
//   {
//     PRINT_MSG0(2, "unable to create suitable context");
//     return -1;
//   }
// 
//   /* create a color map */
//   cmap = XCreateColormap(::display, RootWindow(::display, visual->screen),
//                          visual->visual, AllocNone);
//   /* create a window */
//   swa.colormap = cmap;
//   swa.border_pixel = 0;
//   swa.event_mask = StructureNotifyMask;
//   win = XCreateWindow(::display, RootWindow(::display, visual->screen),
//                       0, 0, this->reqWidth, this->reqHeight,
//                       0, visual->depth, InputOutput, visual->visual,
//                       CWBorderPixel|CWColormap|CWEventMask, &swa);
//   // REMOVE XMapWindow(::display, win);
//   //XIfEvent(::display, &event, WaitForNotify, (char*)win);
// 
//   this->drawable = win;
//   this->context = cx;
// 
//   int a[5];
//   glXGetConfig(::display, visual, GLX_RED_SIZE, a + 0);
//   glXGetConfig(::display, visual, GLX_GREEN_SIZE, a + 1);
//   glXGetConfig(::display, visual, GLX_BLUE_SIZE, a + 2);
//   glXGetConfig(::display, visual, GLX_ALPHA_SIZE, a + 3);
//   glXGetConfig(::display, visual, GLX_DEPTH_SIZE, a + 4);
// 
//   // Print some info about the configuration
//   PRINT_MSG6(1, "rendering: [Xlib unmapped] direct [%s] RGBA [%d %d %d %d] depth [%d]",
//              (glXIsDirect(::display, this->context) ? "yes" : "no"),
//              a[0], a[1], a[2], a[3], a[4]);

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Create GLX pixmap for offscreen rendering
int GLContext::InitGLXPixmap(GLXContext shareList)
{
/////////////////////////////////////////////////////////////////////////
// COMENTADO TRECHO DE CODIGO NAO USADO PARA FACILITAR O REUSO DO CODIGO
/////////////////////////////////////////////////////////////////////////
//   XVisualInfo *visual;
//   Window win;
//   int depth;
//   int attrCount;
//   int attrList[32];
// 
//   // TODO: honor color bits request
//   attrCount = 0;
//   attrList[attrCount++] = GLX_RGBA;
//   attrList[attrCount++] = GLX_DEPTH_SIZE;
//   attrList[attrCount++] = this->reqDepth;
//   attrList[attrCount++] = None;
// 
//   PRINT_MSG0(2, "trying GLX rendering");
//     
//   // Get the default window and window depth
//   win = DefaultRootWindow(::display);
//   depth = DefaultDepth(::display, DefaultScreen(display));
// 
//   // Create X pixmap
//   this->Xpixmap = XCreatePixmap(::display, win, this->reqWidth, this->reqHeight, depth);
//   if (!this->Xpixmap)
//   {    
//     PRINT_MSG0(2, "unable to create X pixmap");
//     return -1;
//   }
// 
//   // Find a suitable visual
//   visual = glXChooseVisual(::display, DefaultScreen(display), attrList);
//   if (!visual)
//   {
//     PRINT_GL_ERR();    
//     PRINT_MSG0(2, "unable to get suitable visual");
//     return -1;
//   }
// 
//   // Create a GLX pixmap
//   this->pixmap = glXCreateGLXPixmap(::display, visual, this->Xpixmap);
//   if (!this->pixmap)
//   {
//     PRINT_GL_ERR();
//     PRINT_MSG0(2, "unable to create glX pixmap");
//     return -1;
//   }
// 
//   // Use this pixemap as our drawable
//   this->drawable = pixmap;
//   
//   // Create the rendering context
//   this->context = glXCreateContext(::display, visual, shareList, False);
//   if (!this->context)
//   {
//     PRINT_GL_ERR();
//     PRINT_MSG0(2, "unable to create context");
//     return -1;
//   }
// 
//   int a[5];
//   glXGetConfig(::display, visual, GLX_RED_SIZE, a + 0);
//   glXGetConfig(::display, visual, GLX_GREEN_SIZE, a + 1);
//   glXGetConfig(::display, visual, GLX_BLUE_SIZE, a + 2);
//   glXGetConfig(::display, visual, GLX_ALPHA_SIZE, a + 3);
//   glXGetConfig(::display, visual, GLX_DEPTH_SIZE, a + 4);
// 
//   // Print some info about the configuration
//   PRINT_MSG6(1, "rendering: [GLX offscreen] direct [%s] RGBA [%d %d %d %d] depth [%d]",
//              (glXIsDirect(::display, this->context) ? "yes" : "no"),
//              a[0], a[1], a[2], a[3], a[4]);
//         
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Create SGI pbuffer for offscreen rendering
int GLContext::InitSGIX( GLXContext shareList )
{
/////////////////////////////////////////////////////////////////////////
// COMENTADO TRECHO DE CODIGO NAO USADO PARA FACILITAR O REUSO DO CODIGO
/////////////////////////////////////////////////////////////////////////
// #ifdef HAVE_PBUFFER
// 
//   int i;
//   XVisualInfo *visual;
//   GLXFBConfigSGIX config, *configs;
//   int config_count;
//   int attrCount;
//   int attrList[32];
//   int a[10];
// 
//   // TODO: honor color bits request
//   attrCount = 0;
//   attrList[attrCount++] = GLX_RENDER_TYPE_SGIX;
//   attrList[attrCount++] = GLX_RGBA_BIT_SGIX;
//   attrList[attrCount++] = GLX_DEPTH_SIZE;
//   attrList[attrCount++] = this->reqDepth;
//   attrList[attrCount++] = None;
//   
//   PRINT_MSG0(2, "trying SGIX pbuffer rendering");
// 
//   // Get possible configurations
//   config = NULL;
//   config_count = 0;
//   configs = glXChooseFBConfigSGIX(::display, 0, attrList, &config_count);
//   PRINT_GL_ERR();
//     
//   // Loop through configs and look for a suitable one
//   for (i = 0; i < config_count; i++)
//   {
//     config = configs[i];
// 
//     glXGetFBConfigAttribSGIX(::display, config, GLX_RED_SIZE, a + 0);
//     glXGetFBConfigAttribSGIX(::display, config, GLX_GREEN_SIZE, a + 1);
//     glXGetFBConfigAttribSGIX(::display, config, GLX_BLUE_SIZE, a + 2);
//     glXGetFBConfigAttribSGIX(::display, config, GLX_ALPHA_SIZE, a + 3);
//     glXGetFBConfigAttribSGIX(::display, config, GLX_DEPTH_SIZE, a + 4);
//         
//     // Print some info about the configuration
//     PRINT_MSG5(2, "[SGIX pbuffer] RGBA [%d %d %d %d] depth [%d]",
//                a[0], a[1], a[2], a[3], a[4]);
//     
//     this->pbuffer = glXCreateGLXPbufferSGIX(::display, config,
//                                             this->reqWidth, this->reqHeight, attrList);
//     PRINT_GL_ERR();
//     if (this->pbuffer)
//       break;
//   }
//   if (!this->pbuffer)
//   {
//     PRINT_MSG0(2, "unable to create SGIX pbuffer; no matching configs");
//     return -1;
//   }
// 
//   // Use this buffer as our drawable
//   this->drawable = pbuffer;
//  
//   // Get the visual
//   visual = glXGetVisualFromFBConfigSGIX(::display, config);
//   PRINT_GL_ERR();
//   if (!visual)
//   {
//     PRINT_MSG0(2, "unable to get suitable visual");
//     return -1;
//   }
// 
//   // Get the rendering context
//   this->context = glXCreateContext(::display, visual, shareList, True);
//   PRINT_GL_ERR();
//   if (!this->context)
//   {
//     PRINT_MSG0(2, "unable to create context");
//     return -1;
//   }
// 
//   glXGetFBConfigAttribSGIX(::display, config, GLX_RED_SIZE, a + 0);
//   glXGetFBConfigAttribSGIX(::display, config, GLX_GREEN_SIZE, a + 1);
//   glXGetFBConfigAttribSGIX(::display, config, GLX_BLUE_SIZE, a + 2);
//   glXGetFBConfigAttribSGIX(::display, config, GLX_ALPHA_SIZE, a + 3);
//   glXGetFBConfigAttribSGIX(::display, config, GLX_DEPTH_SIZE, a + 4);
//   
//   // Print some info about the configuration
//   PRINT_MSG6(1, "rendering: [SGIX pbuffer] direct [%s] RGBA [%d %d %d %d] depth [%d]",
//              (glXIsDirect(::display, this->context) ? "yes" : "no"), 
//              a[0], a[1], a[2], a[3], a[4]);

  return 0;
//#else
//  PRINT_MSG0(2, "trying pbuffer rendering -- extension not present");
//  return -1;
//#endif
}

//////////////////////////////////////////////////////////////////////////////
// Create SGI pbuffer for offscreen rendering
int GLContext::InitGLXP( GLXContext shareList )
{
/////////////////////////////////////////////////////////////////////////
// COMENTADO TRECHO DE CODIGO NAO USADO PARA FACILITAR O REUSO DO CODIGO
/////////////////////////////////////////////////////////////////////////
// #ifdef HAVE_PBUFFER
//   XVisualInfo *visual;
//   GLXFBConfig config, *configs;
//   int i;
//   int nitems;
// 
//   // TODO: honor color bits request
//   
//   int attrib[]  =
//     {
//       //    GLX_DOUBLEBUFFER,  False,
//       GLX_RED_SIZE,      1,
//       GLX_GREEN_SIZE,    1,
//       GLX_BLUE_SIZE,     1,
//       GLX_DEPTH_SIZE,    this->reqDepth,
//       GLX_RENDER_TYPE,   GLX_RGBA_BIT,
//       //    GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT | GLX_WINDOW_BIT,
//       None
//     };
// 
//   int pbufAttrib[] =
//     {
//       GLX_PBUFFER_WIDTH,   this->reqWidth,
//       GLX_PBUFFER_HEIGHT,  this->reqHeight,
//       //    GLX_RENDER_TYPE,     GLX_RGBA_BIT,
//       //    GLX_DEPTH_SIZE, this->reqDepth,
//       GLX_LARGEST_PBUFFER, False,
//       None
//     };
// 
//   int a[10];
//   
//   PRINT_MSG0(2, "trying GLXP pbuffer rendering");
// 
//   config = NULL;
//   configs = glXChooseFBConfig(::display,0,attrib,&nitems);
//   PRINT_GL_ERR();
//   if (!configs)
//   {
//     PRINT_MSG0(2, "unable to create GLXP pbuffer; no matching configs");
//     return -1;
//   }
//   
//   for (i=0;i<nitems;i++)
//   {
//     config = configs[i];
//     this->pbuffer = glXCreatePbuffer(::display, config, pbufAttrib);
//     PRINT_GL_ERR();
//     if (this->pbuffer)
//       break;
//   }
//   if (!this->pbuffer)
//   {
//     PRINT_MSG0(2, "unable to create GLXP pbuffer;");
//     return -1;
//   }
//   
//   // Use this buffer as our drawable
//   this->drawable = pbuffer;
//  
//   // Get the visual
//   visual = glXGetVisualFromFBConfig(::display, config);
//   PRINT_GL_ERR();
//   if (!visual)
//   {
//     PRINT_MSG0(2, "unable to get suitable visual");
//     return -1;
//   }
// 
//   // Get the rendering context
//   this->context = glXCreateContext(::display, visual, shareList, True);
//   PRINT_GL_ERR();
//   if (!this->context)
//   {
//     PRINT_MSG0(2, "unable to create context");
//     return -1;
//   }
// 
//   glXGetFBConfigAttrib(::display, config, GLX_RED_SIZE, a + 0);
//   glXGetFBConfigAttrib(::display, config, GLX_GREEN_SIZE, a + 1);
//   glXGetFBConfigAttrib(::display, config, GLX_BLUE_SIZE, a + 2);
//   glXGetFBConfigAttrib(::display, config, GLX_ALPHA_SIZE, a + 3);
//   glXGetFBConfigAttrib(::display, config, GLX_DEPTH_SIZE, a + 4);
//   
//   // Print some info about the configuration
//   PRINT_MSG6(1, "rendering: [GLXP pbuffer] direct [%s] RGBA [%d %d %d %d] depth [%d]",
//              (glXIsDirect(::display, this->context) ? "yes" : "no"), 
//              a[0], a[1], a[2], a[3], a[4]);
  return 0;
// #else
//   PRINT_MSG0(2, "trying pbuffer rendering -- extension not present");
//   return -1;
// #endif
}

//////////////////////////////////////////////////////////////////////////////
// Finalize the camera
void GLContext::Fini()
{
/////////////////////////////////////////////////////////////////////////
// COMENTADO TRECHO DE CODIGO NAO USADO PARA FACILITAR O REUSO DO CODIGO
/////////////////////////////////////////////////////////////////////////
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Make this the current rendering context
int GLContext::MakeCurrent()
{
/////////////////////////////////////////////////////////////////////////
// COMENTADO TRECHO DE CODIGO NAO USADO PARA FACILITAR O REUSO DO CODIGO
/////////////////////////////////////////////////////////////////////////
//   if (!glXMakeCurrent(::display, this->drawable, this->context))
//   {
//     PRINT_GL_ERR();
//     return -1;
//   }
  return 0;
}

