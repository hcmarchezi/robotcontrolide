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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* Desc: A camera sensor using OpenGL
 * Author: Nate Koenig
 * Date: 15 July 2003
 * CVS: $Id: Camera.cc,v 1.74 2006/02/14 03:08:17 natepak Exp $
 */

#if HAVE_CONFIG_H
  #include <config.h>
#endif

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>

#include "World.hh"
#include "Model.hh"
#include "Body.hh"
#include "WorldFile.hh"
#include "Camera.hh"
#include "LightGeom.hh"
//#include "ShaderManager.hh"
//#include "ShaderObject.hh"

//////////////////////////////////////////////////////////////////////////////
// Constructor 
Camera::Camera( World *world )
    : Sensor( world )
{
  this->cameraPose = GzPoseSet(GzVectorZero(), GzQuaternIdent());

  this->imageWidth = this->imageHeight = 0;
  this->rgbImage = NULL;

  this->saveEnable = false;
  this->saveCount = 0;
  this->savePathname = NULL;

  //this->shaderManager = new ShaderManager();

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
Camera::~Camera()
{
  //delete this->shaderManager;

  //this->shaderManager = NULL;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the sensor
int Camera::Init(int width, int height, double hfov, double minDepth, 
    double maxDepth, const char *method, int zBufferDepth)
{
  this->imageWidth = width;
  this->imageHeight = height;

  this->hfov = hfov;
  this->nearClip = minDepth;
  this->farClip = maxDepth;

  // Do some sanity checks
  if (this->imageWidth == 0 || this->imageHeight == 0)
  {
    PRINT_ERR("image has zero size");
    return -1;
  }
  if (this->hfov < 0.01 || this->hfov > M_PI)
  {
    PRINT_ERR("bad field of view");
    return -1;
  }
  if (this->nearClip < 0.01)
  {
    PRINT_ERR("near clipping plane (min depth) is zero");
    return -1;
  }

  // Initialize the GL rendering context
  if (this->context.Init(this->imageWidth, this->imageHeight, 5, 0, 
        zBufferDepth, method) != 0)
  {
    PRINT_ERR("camera initialization failed: no rendering context");
    return -1;
  } else {
    //this->context.MakeCurrent();
    this->InitContext(this->context);
  }

  // Initialize image buffers
  int imageSize = this->imageWidth * this->imageHeight;
  this->rgbImage = new uint8_t[imageSize * 3];
    
  // Initialize display options
  this->renderOpt.displayBBox = false;
  this->renderOpt.displayAxes = false;
  this->renderOpt.displayCoM = false;
  this->renderOpt.displaySkins = false;  
  this->renderOpt.displayRays = false;
  this->renderOpt.displayFrustrums = false;
  this->renderOpt.displayMaterials = true;
  this->renderOpt.displayTextures = true;
  this->renderOpt.polygonMode = GL_FILL;
  this->renderOpt.shadeModel = GL_SMOOTH;

//  this->shaderObject = this->shaderManager->LoadFromFile("defaultShader.vert", "defaultShader.frag");
  //this->shaderObject->SendUniform1i("NumEnabledLights",1);
  //this->shaderObject->SendUniform1i("SeparateSpecular",0);

  return 0;
}

void Camera::InitContext( GLContext &context )
{
  context.MakeCurrent();

  glClearColor(this->world->skyColor.r, this->world->skyColor.g,
               this->world->skyColor.b, this->world->skyColor.a);

  // Normalize normals
  glEnable(GL_NORMALIZE);

  // Light model (two-sided)
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
  PRINT_GL_ERR();

  // Enable blending for transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  PRINT_GL_ERR();

  // Use depth buffering for hidden surface elimination
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  // Enable back face culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
 
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // Configure fog
  if (this->world->fogEnable)
  {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, this->world->fogDensity);
    glFogfv(GL_FOG_COLOR, this->world->skyColor);
    PRINT_GL_ERR();
  }
}

//////////////////////////////////////////////////////////////////////////////
// Finalize the camera
int Camera::Fini()
{
  this->context.Fini();
  
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update the drawing
void Camera::Update()
{  
  // Render the image
  this->Render();

  // Save image frames
  if (this->saveEnable)
    this->SaveFrame();

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Render the scene from the camera perspective
void Camera::Render()
{
  GzPose pose;
  GzVector a, b;

  // Compute the point to look at, and the orientation vector
  pose = this->cameraPose;
  a = GzVectorSet(1, 0, 0);
  b = GzVectorSet(1, 0, 1);
  a = GzCoordPositionAdd(a, pose.pos, pose.rot);
  b = GzCoordPositionAdd(b, pose.pos, pose.rot);
  b = GzVectorSub(b, a);

  // Fill in the current camera index
  this->renderOpt.cameraIndex = this->context.GetContextIndex();
  this->renderOpt.cameraPose = this->cameraPose;
  this->renderOpt.farClip = this->farClip;

  // Select the GL context
  if (this->context.MakeCurrent() != 0)
    return;

  // Clear the window
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  PRINT_GL_ERR();

  // Set the camera lens propeties (assumes square pixels)
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(this->hfov * 180 / M_PI, 
      (double) this->imageWidth / this->imageHeight, this->nearClip, 
      this->farClip); PRINT_GL_ERR();

  // Store the projection matrix for later use
  glGetFloatv(GL_PROJECTION_MATRIX, this->cameraProjectionMatrix);

  // Set the camera pose
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(pose.pos.x, pose.pos.y, pose.pos.z, a.x, a.y, a.z, b.x, b.y, b.z);
  PRINT_GL_ERR();

  glGetFloatv(GL_MODELVIEW_MATRIX, this->cameraViewMatrix);

  // Counter Clockwise rotation are front faces
  glFrontFace(GL_CCW);

  // Enable lighting
  glEnable(GL_LIGHTING);

  // Set global rendering properties
  glPolygonMode( GL_FRONT_AND_BACK, this->renderOpt.polygonMode );  
  PRINT_GL_ERR();    
  glShadeModel(this->renderOpt.shadeModel);
  PRINT_GL_ERR();

  // Reset material properties
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, GzColor(0.2, 0.2, 0.2, 1.0));
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, GzColor(0.8, 0.8, 0.8, 1.0));
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, GzColor(0.0, 0.0, 0.0, 1.0));
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
  PRINT_GL_ERR();


  //this->shaderObject->Begin();
  // Make multiple passes to ensure geoms get rendered in correct
  // order (needed for lights, for example)
  for (int p = 0; p < GZ_RENDER_MAX_PASSES; p++)
  {
    this->RenderModels(p);
    PRINT_GL_ERR();
  }
  //this->shaderObject->End();

  // Make sure we are done
  glFinish();
  PRINT_GL_ERR();

  // Get the image from the frame buffer
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, this->imageWidth, this->imageHeight,
      GL_RGB, GL_UNSIGNED_BYTE, this->rgbImage);
  PRINT_GL_ERR();

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the Z-buffer value at the given image coordinate
double Camera::GetZValue(int x, int y)
{
  GLfloat iz;

  // Flip y axis
  y = this->imageHeight - 1 - y;

  // Get image z value of first spot
  glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &iz);

  return iz;
}


//////////////////////////////////////////////////////////////////////////////
// Determine change in camera pose based on two image points
GzPose Camera::CalcCameraDelta(int mode, GzVector a, GzVector b)
{
  GLfloat ix, iy, iz;

  // Image coordates of the first spot
  ix = a.x;
  iy = this->imageHeight - 1 - a.y;
  iz = a.z;

  double dx, dy, dz;
  
  // Convert to division coordinates
  dx = 2 * ix / this->imageWidth - 1;
  dy = 2 * iy / this->imageHeight - 1;
  dz = (2 * iz  - 1);

  double px, py, pz, pw;

  // Compute perspective divisor; assumes well-formed projection matrix
  pw = this->cameraProjectionMatrix[14] / (dz + this->cameraProjectionMatrix[10]);

  // Convert to projected coordinate
  px = dx * pw;
  py = dy * pw;
  pz = dz * pw;

  double cx, cy, cz, cw;
  
  // Convert to camera frame (inverse projection)
  cx = 1 / this->cameraProjectionMatrix[0] * px;
  cy = 1 / this->cameraProjectionMatrix[5] * py;
  cz = -pw;
  cw = 1 / this->cameraProjectionMatrix[14] * pz + 1 / this->cameraProjectionMatrix[10] * pw;

  double ix_, iy_;
  double dx_, dy_;
  
  // Image coordates of the second spot
  ix_ = b.x;
  iy_ = this->imageHeight - 1 - b.y;

  // Convert to division coordinates
  dx_ = 2 * ix_ / this->imageWidth - 1;
  dy_ = 2 * iy_ / this->imageHeight - 1;

  GzPose n;
  n.pos = GzVectorZero();
  n.rot = GzQuaternIdent();
  
  double nx, ny, nz;

  // Translate along x, y
  if (mode == 0)
  {
    nz = 0.0;
    nx = - (cz + nz) / this->cameraProjectionMatrix[0] * dx_ - cx;
    ny = - (cz + nz) / this->cameraProjectionMatrix[5] * dy_ - cy;
  }

  // Translate (zoom) along z
  else if (mode == 1)
  {
    ny = 0.0;
    nz = - this->cameraProjectionMatrix[5] / (fabs(dy_) + 1e-16) * fabs(cy) - cz;    
    nx = 0;

    // Bound so we dont go too close
    if (nz > -cz - 2 * this->nearClip)
      nz = -cz - 2 * this->nearClip;

    // Bound so we dont go too far away
    else if (nz < -0.5 * this->farClip - cz)
      nz = -0.5 * this->farClip - cz;
  }
  else
  {
    nx = 0;
    ny = 0;
    nz = 0;
  }
  
  // Convert to Gazebo coordinate system (camera axis along +x)
  n.pos.x = +nz;
  n.pos.y = +nx;
  n.pos.z = -ny;

  // Rotate (pitch and yaw)
  if (mode == 2)
  {
    double rx, ry;
    rx = atan((dx_ - dx)  / this->cameraProjectionMatrix[0]);
    ry = atan((dy_ - dy)  / this->cameraProjectionMatrix[5]);
    n.rot = GzQuaternFromEuler(0, ry, rx);
  }
  
  return n;
}


//////////////////////////////////////////////////////////////////////////////
// Render the models
void Camera::RenderModels( int pass )
{
  int i;
  Model *model;

  for (i = 0; i < this->world->GetNumModels(); i++)
  {
    model = this->world->GetModels()[i];
    model->Render(pass, &this->renderOpt);
  }
}

//////////////////////////////////////////////////////////////////////////////
// Set the pose of the camera
void Camera::SetPose(GzPose pose)
{
  this->cameraPose = pose;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the pose of the camera
GzPose Camera::GetPose()
{
  return this->cameraPose;
}


//////////////////////////////////////////////////////////////////////////////
// Set the camera FOV (horizontal)
void Camera::SetFOV(double fov)
{
  this->hfov = fov;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the camera FOV (horizontal)
double Camera::GetFOV() const
{
  return this->hfov;
}


//////////////////////////////////////////////////////////////////////////////
// Set the rendering options
void Camera::SetRenderOptions(const RenderOptions *opt)
{
  this->renderOpt = *opt;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the rendering options
void Camera::GetRenderOptions(RenderOptions *opt) const
{
  assert(opt != NULL);
  *opt = this->renderOpt;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the image dimensions
void Camera::GetImageSize(int *w, int *h)
{
  *w = this->imageWidth;
  *h = this->imageHeight;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the base filename for saved frames
void Camera::SetSavePath(const char *pathname)
{
  char tmp[1024];
    
  this->savePathname = pathname;
  this->saveCount = 0;

  sprintf(tmp, "mkdir %s 2>>/dev/null", this->savePathname);
  system(tmp);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Enable or disable saving
void Camera::EnableSaveFrame(bool enable)
{
  this->saveEnable = enable;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Save the current frame to disk
void Camera::SaveFrame()
{
  char tmp[1024];
  FILE *fp;
  
  sprintf(tmp, "%s/%04d.pnm", this->savePathname, this->saveCount);

  fp = fopen( tmp, "wb" );

  if (!fp)
  {
    PRINT_ERR1( "unable to open file %s\n for writing", tmp );
    return;
  }
  
  fprintf( fp, "P6\n# Gazebo\n%d %d\n255\n", this->imageWidth, this->imageHeight);
  for (int i = this->imageHeight-1; i >= 0; i--)
    fwrite( this->rgbImage + i * this->imageWidth * 3, 1, this->imageWidth * 3, fp );

  fclose( fp );
  this->saveCount++;

  return;
}
