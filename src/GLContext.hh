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
 * CVS: $Id: GLContext.hh,v 1.7 2005/04/26 18:29:46 natepak Exp $
 */


#ifndef GLCONTEXT_HH
#define GLCONTEXT_HH

#include <GL/glx.h>


/// @brief Class for managing GL contexts.
///
/// Used by OpenGL-based camera sensors to create GL contexts and
/// display lists.
class GLContext
{
  /// @brief Constructor
  public: GLContext();

  /// @brief Destructor
  public: virtual ~GLContext();

  /// @brief Initialize the context, based on the requested properties.
  /// @param width, height image dimensions (pixels)
  /// @param color minimum bits for color buffers
  /// @param alpha minimum bits for alpha buffer
  /// @param depth minimum bits for z buffer
  /// @param method Prefered rendering method: SGIX, GLX or XLIB.
  /// @param shareList Context with which to share display lists.
  /// @returns Returns zero on success, non-zero on errror
  public: int Init(int width, int height, int color, int alpha, int depth,
                   const char *method, GLXContext shareList = NULL);

  /// @brief Finialize the context
  public: void Fini();

  /// @brief Get the rendering context
  public: GLXContext GetContext() {return this->context;}

  /// @brief Get the display list index
  public: int GetContextIndex() {return this->contextIndex;}

  /// @brief Make this the current rendering context
  /// @returns Returns 0 on success.
  public: int MakeCurrent();

  /// @brief Create unmapped xlib window for onscreen rendering
  private: int InitXlib(GLXContext shareList);

  /// @brief Create GLX pixmap for offscreen rendering
  private: int InitGLXPixmap(GLXContext shareList);

  /// @brief Create SGI pbuffer for offscreen rendering
  private: int InitSGIX(GLXContext shareList);

  /// @brief Create GLXP pbuffer for offscreen rendering
  private: int InitGLXP(GLXContext shareList);

  // Requested properties
  private: int reqWidth, reqHeight;
  private: int reqColor, reqAlpha, reqDepth;

  // GLUT rendering info
  private: int win;
  
  // Common offscreen rendering info
  private: GLXContext context;  
  private: GLXDrawable drawable;

  // SGI pbuffer method
  private: GLXPbuffer pbuffer;

  // GLX method
  private: Pixmap Xpixmap;
  private: GLXPixmap pixmap;

  // Context index
  private: int contextIndex;
};



#endif

