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
/* Desc: A very simple generic monocular camera
 * Author: Andrew Howard (re-write from scratch)
 * Date: 15 Nov 2005
 * CVS: $Id: MonoCam.cc,v 1.20 2006/02/22 15:29:24 natepak Exp $
 */

/// @addtogroup models 
/// @{ 
/** @defgroup MonoCam Mono Camera

@htmlinclude MonoCam_view.html

The MonoCam model simulates a simple (generic) monocular camera.


@par libgazebo interfaces

This model supports the @ref camera interfaces.

@par Player drivers

Image data is available through the %gz_camera driver.


@par Attributes

The following attributes are supported.

@htmlinclude default_attr_include.html

- updateRate (float, Hz)
  - Frames per second.
  - Default: 10
  
- imageSize (integer tuple, pixels)
  - Image dimensions (width and height).
  - Default: 320 240

- hfov (float, degrees)
  - Horizontal field of view for a perspective lens.
  - Default: 60

- nearClip, farClip (float, meters)
  - Near and far clipping planes; see @ref gazebo_opengl.
  - Default: 0.5 100.0

- renderMethod (string)
  - OpenGL rendering method: SGIX, GLX, XLIB, or AUTO.  See @ref gazebo_opengl.
  - Default: AUTO

- zBufferDepth (integer)
	- Z buffer depth in bits (8, 16, 24 or 32)
	- Default: 16

- savePath (string, path)
  - Directory for saving image frames.
  - Default: .

- saveFrames (bool)
  - Set to true to save image frames.
  - Default: false

- useFrustrum (bool)
  - Create a frustrum to provide user feedback in the ObserveCam.
  - Default: true


@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:MonoCam>
  <id>camera1</id>
  <xyz>0 0 0</xyz>  
</model:MonoCam>
@endverbatim

@par Views

@htmlinclude MonoCam_more_views.html

@par Author

Andrew Howard

*/
/// @} 

#include <assert.h>
#include <float.h>
#include "gazebo.h"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "CylinderGeom.hh"
#include "BoxGeom.hh"
#include "FrustrumGeom.hh"
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
//////////////////////////////////////////
//#include "Camera/Camera.hh"
//////////////////////////////////////////

#include "MonoCam.hh"


//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("MonoCam", MonoCam);


//////////////////////////////////////////////////////////////////////////////
// Constructor
MonoCam::MonoCam( World *world )
    : Model( world )
{
  /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  /////////////////////////////////////////////
  this->SetField(new DoubleField("updateRate",0.0));
  this->SetField(new IntegerTupleField("imageSize",IntegerTupleSet(320,240)));
  this->SetField(new DoubleField("nearClip",0.0));
  this->SetField(new DoubleField("farClip",0.0));
  this->SetField(new StringField("renderMethod",""));
  this->SetField(new IntegerField("zBufferDepth",0));
  this->SetField(new StringField("savePath",""));
  this->SetField(new BoolField("saveFrames",false));
  this->SetField(new BoolField("useFrustum",false));
  /////////////////////////////////////////////

////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   this->camera = new Camera(this->world);
//   this->cameraIface = gz_camera_alloc();

  this->frustrum = NULL;
  this->body = NULL;
 
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
MonoCam::~MonoCam()
{
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   delete this->cameraIface;
//   delete this->camera;

  if (this->body)
    delete this->body;

  if (this->frustrum)
    delete this->frustrum;
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   this->camera = NULL;
//   this->cameraIface = NULL;
  this->frustrum = NULL;
  this->body = NULL;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Load the model
int MonoCam::Load( WorldFile *file, WorldFileNode *node )
{
  int imgWidth, imgHeight, zDepth;
  double nearClip, farClip;
  const char *method;

  // Get the time between camera updates
  this->updatePeriod = 1.0 / (node->GetDouble("updateRate", 10) + 1e-6);
  this->updateTime = -this->updatePeriod;

  // Lens field of view
  this->hfov = node->GetAngle("hfov", DTOR(60));

  // Get image dimensions
  imgWidth = node->GetTupleInt("imageSize", 0, 320);
  imgHeight = node->GetTupleInt("imageSize", 1, 240);

  // Get camera parameters
  nearClip = node->GetLength("nearClip", 0.50, 0);
  farClip = node->GetLength("farClip", 50.0, 0);
  method = node->GetString("renderMethod", "auto");
  zDepth = node->GetInt("zBufferDepth", 16);

  if (((zDepth % 8) != 0) || (zDepth < 8) || (zDepth > 32))
    zDepth = 16;

////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   // Initialize the sensor
//   if (this->camera->Init(imgWidth, imgHeight, this->hfov,
//                          nearClip, farClip, method, zDepth) != 0)
//     return -1;
// 
//   // Path for saving frames
//   this->camera->SetSavePath(node->GetString("savePath", "."));
  
  // Create the ODE objects
  if (this->OdeLoad( file, node ) != 0)
    return -1;

  // Create a frustrum for user feedback
  if (node->GetBool("useFrustrum", true))
  {
    this->frustrum = new FrustrumGeom(this->body, this->modelSpaceId,
                                      (double) imgWidth / imgHeight, nearClip, farClip);
  }

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load ODE objects
int MonoCam::OdeLoad( WorldFile *file, WorldFileNode *node )
{
  dReal mass;
  dReal headLength, headWidth, headHeight;
  dReal lensRadius, lensHeight;
  dReal zOffset;
  Geom *geom;

  mass=0.4; //kg

  headLength = 0.050; //meters
  headWidth = 0.050; //meters
  headHeight = 0.050; //meters

  lensRadius = 0.024; //meters
  lensHeight = 0.01; //meters

  zOffset = headHeight / 2;

  this->body = new Body( this->world );
  this->AddBody( this->body, true );
  
  // Rectangular head unit
  geom = new BoxGeom( this->body, this->modelSpaceId,
                      headLength, headWidth, headHeight );
  geom->SetRelativePosition( GzVectorSet(0, 0, zOffset) );
  geom->SetMass( mass*0.25 );
  geom->SetColor( GzColor(0.3,0.3,0.3) );

  // We use the head to define the origin of the imaging plane
  this->imager = geom;
  
  // The lens
  geom = new CylinderGeom( this->body, this->modelSpaceId,
                           lensRadius, lensHeight );
  geom->SetRelativePosition( GzVectorSet(headWidth/2.0 + lensHeight/2, 0, zOffset) );
  geom->SetColor( GzColor(1, 1, 1) );
  geom->SetMass( 0 );
  geom->SetRelativeRotation( GzQuaternFromEuler(0, M_PI/2, 0) );
  
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int MonoCam::Init( WorldFile *file, WorldFileNode *node )
{
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   int width, height;
//   RenderOptions *renderOpts;
//   gz_camera_data_t *data;
//   
//   // Camera interface
//   assert(this->cameraIface);
//   if (gz_camera_create(this->cameraIface, this->world->gz_server, this->GetId(),
//                        "MonoCam", this->GetIntId(), this->GetParentIntId()) !=0)
//     return -1;
// 
// 
//   // Set image dimensions (needed by GUI)
//   gz_camera_lock(this->cameraIface, 1);
//   data = this->cameraIface->data;
//   this->camera->GetImageSize(&width, &height);
//   data->width = width;
//   data->height = height;
//   gz_camera_unlock(this->cameraIface);
// 
//   // Set initial rendering options
//   renderOpts = new RenderOptions();
//   this->camera->GetRenderOptions(renderOpts);
//   renderOpts->displaySkins = true;  
//   this->camera->SetRenderOptions(renderOpts);


  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the model
int MonoCam::Fini()
{
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   // Finalize external interface  
//   gz_camera_destroy(this->cameraIface);
// 
//   this->camera->Fini();
    
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update the model
void MonoCam::Update( double step )
{
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   GzPose pose;
//   
//   if (this->world->GetSimTime() - this->updateTime >= this->updatePeriod)
//   {
//     this->updateTime = this->world->GetSimTime();
// 
//     // Get the imager pose (which may be offset from the model pose)
//     pose = this->imager->GetPose();
// 
//     // Set the camera sensor pose
//     this->camera->SetPose(pose);
// 
//     // Update the GUI
//     if (this->frustrum)
//     {
//       int width, height;
//       const uint8_t *data;
//       this->camera->GetImageSize(&width, &height);
//       data = this->camera->GetImageData();
//       this->frustrum->SetFOV(this->camera->GetFOV());
//       this->frustrum->SetTexture2D(width, height, data);
//     }
// 
//     // Update the camera
//     this->camera->Update();
//       
//     // Update the camera interface
//     this->PutCameraData();
//   }
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Send data camera data
void MonoCam::PutCameraData()
{
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   int i;
//   int len;
//   const unsigned char *src;
//   unsigned char *dst;
//   int width, height;
//   gz_camera_data_t *data;
// 
//   gz_camera_lock(this->cameraIface, 1);
// 
//   data = this->cameraIface->data;
// 
//   // Get image dimensions
//   this->camera->GetImageSize(&width, &height);
//   
//   data->time = this->world->GetSimTime();
//   data->width = width;
//   data->height = height;
//   data->image_size = width * height * 3;
// 
//   // Make sure there is room to store the image
//   assert(data->image_size <= sizeof(data->image));
//   
//   // Copy the pixel data to the interface, but flip the y axis
//   len = width * 3;
//   src = this->camera->GetImageData() + (height - 1) * len;
//   dst = data->image;    
//   for (i = 0; i < height; i++, src -= len, dst += len)
//     memcpy(dst, src, len);
// 
//   gz_camera_unlock(this->cameraIface);
//   gz_camera_post(this->cameraIface);

  return;
}

/////////////////////////////////////////////////////////////////////
// Inicia a geometria e as propriedades do modelo
/////////////////////////////////////////////////////////////////////
void MonoCam::Start()
{
////////////////////////////////////
// Inicia as propriedades do modelo
////////////////////////////////////
  int imgWidth, imgHeight, zDepth;
  double nearClip, farClip;
  const char *method;

  // Get the time between camera updates
  this->updatePeriod = 1.0 / (10 + 1e-6);
  this->updateTime = -this->updatePeriod;

  // Lens field of view
  this->hfov = DTOR(60);

  // Get image dimensions
  imgWidth = 320;
  imgHeight = 240;

  // Get camera parameters
  nearClip = 0.50;
  farClip = 50.0;
  method = "auto";
  zDepth = 16;

  if (((zDepth % 8) != 0) || (zDepth < 8) || (zDepth > 32))
    zDepth = 16;

  const char * savePath = ".";

  bool useFrustum = true;

  ///////////////////// GEOMETRIA //////////////////////////
  dReal mass;
  dReal headLength, headWidth, headHeight;
  dReal lensRadius, lensHeight;
  dReal zOffset;
  Geom *geom;

  mass=0.4; //kg

  headLength = 0.050; //meters
  headWidth = 0.050; //meters
  headHeight = 0.050; //meters

  lensRadius = 0.024; //meters
  lensHeight = 0.01; //meters

  zOffset = headHeight / 2;

  this->body = new Body( this->world );
  this->AddBody( this->body, true );
  
  // Rectangular head unit
  geom = new BoxGeom( this->body, this->modelSpaceId,
                      headLength, headWidth, headHeight );
  geom->SetRelativePosition( GzVectorSet(0, 0, zOffset) );
  geom->SetMass( mass*0.25 );
  geom->SetColor( GzColor(0.3,0.3,0.3) );

  // We use the head to define the origin of the imaging plane
  this->imager = geom;
  
  // The lens
  geom = new CylinderGeom( this->body, this->modelSpaceId,
                           lensRadius, lensHeight );
  geom->SetRelativePosition( GzVectorSet(headWidth/2.0 + lensHeight/2, 0, zOffset) );
  geom->SetColor( GzColor(1, 1, 1) );
  geom->SetMass( 0 );
  geom->SetRelativeRotation( GzQuaternFromEuler(0, M_PI/2, 0) );
  
  
}

