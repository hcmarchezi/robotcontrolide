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
/* Desc: A Sony VID30 PTZ camera
 * Author: Nate Koenig
 * Date: 16 July 2003
 * CVS: $Id: SonyVID30.cc,v 1.59 2006/02/22 15:29:24 natepak Exp $
 */

/// @addtogroup models 
/// @{ 
/** @defgroup SonyVID30 Sony VID 30

@htmlinclude SonyVID30_view.html

The SonyVID30 model simulates the Sony VID 30 pan-tilt-zoom camera.


@par libgazebo interfaces

This model supports the @ref ptz and @ref camera interfaces.

@par Player drivers

Pan/tilt/zoom information is available through the %gz_ptz driver;
image data is available through the %gz_camera driver.


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
  
- zoomLimits (float tuple)
  - Minimum and maximum zoom factor.
  - Default: 10 60

- motionGain (float)
  - Servo gain term (P controller on pan, tilt axes).
  - Default: 2

- zoomGain (float)
  - Servo gain term (P controller on zoom axis).
  - Default: 2

@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:SonyVID30>
  <id>ptz1</id>
  <xyz>0 0 0</xyz>  
</model:SonyVID30>
@endverbatim

@par Views

@htmlinclude SonyVID30_more_views.html

@par Authors

Nate Koenig, Andrew Howard

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
#include "HingeJoint.hh"
#include "Camera.hh" // "Camera/Camera.hh" // MODIFICADO PARA FACILITAR REUSO DO CODIGO

#include "SonyVID30.hh"



//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("SonyVID30", SonyVID30);


//////////////////////////////////////////////////////////////////////////////
// Constructor
SonyVID30::SonyVID30( World *world )
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
  this->SetField(new DoubleField("minZoom",0.0));
  this->SetField(new DoubleField("maxZoom",0.0));
  this->SetField(new DoubleField("motionGain",0.0));
  this->SetField(new DoubleField("zoomGain",0.0));
  /////////////////////////////////////////////


  int i;

/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   this->camera = new Camera(this->world);
//   this->cameraIface = gz_camera_alloc();
//  this->ptzIface = gz_ptz_alloc();

  for (i=0; i<8; i++)
    this->bodyGeoms[i] = NULL;

  this->frustrum = NULL;
  this->body = NULL;
  this->middle = NULL;
  this->head = NULL;
  this->lensGeom = NULL;
  this->panJoint = NULL;
  this->tiltJoint = NULL;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
SonyVID30::~SonyVID30()
{
  int i;

/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   delete this->ptzIface;
//   delete this->cameraIface;

  delete this->camera;

  if (this->frustrum)
    delete this->frustrum;
  if (this->body)
    delete this->body;
  if (this->middle)
    delete this->middle;
  if (this->head)
    delete this->head;
  if (this->lensGeom)
    delete this->lensGeom;
  if (this->panJoint)
    delete this->panJoint;
  if (this->tiltJoint)
    delete this->tiltJoint;

  for (i=0; i<8; i++)
  {
    if (this->bodyGeoms[i])
      delete this->bodyGeoms;
    this->bodyGeoms[i] = NULL;
  }

/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   this->ptzIface = NULL;
//   this->cameraIface = NULL;

  this->camera = NULL;
  this->frustrum = NULL;
  this->body = NULL;
  this->middle = NULL;
  this->head = NULL;
  this->lensGeom = NULL;
  this->panJoint = NULL;
  this->tiltJoint = NULL;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Load the model
int SonyVID30::Load( WorldFile *file, WorldFileNode *node )
{
//////////////////////////////////////////////////
// COMENTADO PARA FACILITAR REUSO DE CODIGO
//////////////////////////////////////////////////
//   int imgWidth, imgHeight, zDepth;
//   double nearClip, farClip;
//   const char *method;
// 
//   // Get the time between ptz updates
//   this->ptzUpdatePeriod = 0.100;
//   this->ptzUpdateTime = -this->ptzUpdatePeriod;
// 
//   // Get the time between camera updates
//   this->cameraUpdatePeriod = 1.0 / (node->GetDouble("updateRate", 10) + 1e-6);
//   this->cameraUpdateTime = -this->cameraUpdatePeriod;
// 
//   // Lens field of view
//   this->hfov = node->GetAngle("hfov", DTOR(60));
// 
//   // Zoom limits
//   this->zoomMin = node->GetTupleAngle("zoomLimits", 0, 1);
//   this->zoomMax = node->GetTupleAngle("zoomLimits", 1, 10);
//   
//   // Gain term on PTZ controller
//   this->motionGain = node->GetDouble( "motionGain", 2.0 );
//   this->zoomGain = node->GetDouble( "zoomGain", 2.0 );
// 
//   // Initial camera settings
//   this->cmdZoom = this->zoomMin;
//   this->zoom = this->cmdZoom;
// 
//   // Get image dimensions
//   imgWidth = node->GetTupleInt("imageSize", 0, 320);
//   imgHeight = node->GetTupleInt("imageSize", 1, 240);
// 
//   // Get camera parameters
//   nearClip = node->GetLength("nearClip", 0.50, 0);
//   farClip = node->GetLength("farClip", 50.0, 0);
//   method = node->GetString("renderMethod", "auto");
//   zDepth = node->GetInt("zBufferDepth", 16);
//   if (((zDepth % 8) != 0) || (zDepth < 8) || (zDepth  > 32))
//     zDepth = 16;
//   
//   // Initialize the sensor
//   if (this->camera->Init(imgWidth, imgHeight, this->hfov * this->cmdZoom,
//                          nearClip, farClip, method, zDepth) != 0)
//     return -1;
// 
//   // Path for saving frames
//   this->camera->SetSavePath(node->GetString("savePath", "."));
//   this->camera->EnableSaveFrame(node->GetString("saveFrames", false));
// 
//   // Create the ODE objects
//   if (this->OdeLoad( file, node ) != 0)
//     return -1;
//   
//   // Create a frustrum for user feedback
//   if (node->GetBool("useFrustrum", true))
//   {
//     this->frustrum = new FrustrumGeom(this->head, this->modelSpaceId,
//                                       (double) imgWidth / imgHeight, nearClip, farClip);
//   }


  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load ODE objects
int SonyVID30::OdeLoad( WorldFile *file, WorldFileNode *node )
{
//////////////////////////////////////////////////
// COMENTADO PARA FACILITAR REUSO DE CODIGO
//////////////////////////////////////////////////
//   dReal mass;
//   dReal baseRadius, baseHeight;
//   dReal uprightLength, uprightWidth, uprightHeight;
//   dReal headLength, headWidth, headHeight;
//   dReal lensRadius, lensHeight;
//   dReal zOffset;
// 
//   mass=0.4; //kg
// 
//   baseRadius = 0.071; //meters
//   baseHeight = 0.028; //meters
// 
//   uprightLength = 0.057; //meters
//   uprightWidth = 0.105; //meters
//   uprightHeight = 0.025; //meters
// 
//   headLength = 0.120; //meters
//   headWidth = 0.120; //meters
//   headHeight = 0.055; //meters
// 
//   lensRadius = 0.024; //meters
//   lensHeight = 0.01; //meters
// 
//   zOffset = 0.0;
// 
//   // The primary cylindrical base
//   this->body = new Body( this->world );
//   this->bodyGeoms[0] = new CylinderGeom( this->body, this->modelSpaceId, baseRadius, baseHeight);
//   this->bodyGeoms[0]->SetRelativePosition( GzVectorSet(0, 0, 0) );
//   this->bodyGeoms[0]->SetMass( mass*.5 );
//   this->bodyGeoms[0]->SetColor( GzColor(0.3, 0.3, 0.3) );
//   this->AddBody( this->body, true );
//   zOffset += baseHeight;
// 
//   // Rectagular upright
//   this->middle = new Body( this->world );
//   this->bodyGeoms[1] = new BoxGeom( this->middle, this->modelSpaceId,
//                                     uprightLength,uprightWidth, uprightHeight );
//   this->bodyGeoms[1]->SetRelativePosition( GzVectorSet(0, 0, zOffset) );
//   this->bodyGeoms[1]->SetMass( mass*.25 );
//   this->bodyGeoms[1]->SetColor( GzColor(0.3, 0.3, 0.3) );
//   this->AddBody( this->middle );
//   zOffset += uprightHeight + uprightHeight * 0.5;
// 
//   // Rectangular head unit
//   this->head = new Body( this->world, "head" );
//   this->bodyGeoms[2] = new BoxGeom( this->head, this->modelSpaceId,
//                                     headLength, headWidth, headHeight );
//   this->bodyGeoms[2]->SetRelativePosition( GzVectorSet(0, 0, zOffset) );
//   this->bodyGeoms[2]->SetMass( mass*0.25 );
//   this->bodyGeoms[2]->SetColor( GzColor(0.3,0.3,0.3) );
// 
//   // The lens
//   this->bodyGeoms[3] = new CylinderGeom( this->head, this->modelSpaceId,
//                                          lensRadius, lensHeight );
//   this->bodyGeoms[3]->SetRelativePosition( GzVectorSet(headWidth/2.0 + lensHeight/2, 0, zOffset) );
//   //  this->bodyGeoms[3]->SetColor( GzColor(1, 1, 1) );
//   //  this->bodyGeoms[3]->SetAmbientColor( GzColor(1, 1, 1) );
//   this->bodyGeoms[3]->SetMass( 0 );
//   this->AddBody( this->head );
// 
//   GzQuatern a,b,c;
//   a = GzQuaternFromAxis(0,1,0,+M_PI/2.0);
//   b = GzQuaternFromAxis(0,0,1,+M_PI/2.0);
//   c = GzQuaternMul(a,b);
//   this->bodyGeoms[3]->SetRelativeRotation( c );
// 
//   this->lensGeom = this->bodyGeoms[3];
// 
//   //pan joint
//   this->panJoint = new HingeJoint( this->world );
//   this->panJoint->Attach(this->middle, this->body);
//   GzVector ab = this->middle->GetPosition();
//   this->panJoint->SetAnchor( ab );
//   this->panJoint->SetAxis(GzVectorSet(0,0,1));
//   this->panJoint->SetParam( dParamFMax, 10.1 ); 
// 
//   //tilt joint
//   this->tiltJoint = new HingeJoint( this->world );
//   this->tiltJoint->Attach(this->head, this->middle);
//   this->tiltJoint->SetAnchor( GzVectorSet(ab.x, ab.y, ab.z+uprightHeight));
//   this->tiltJoint->SetAxis(GzVectorSet(0,1,0));
//   this->tiltJoint->SetParam( dParamFMax, 10.1 ); 
// 
//   this->middle->SetFiniteRotationMode(1);
//   this->head->SetFiniteRotationMode(1);

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int SonyVID30::Init( WorldFile *file, WorldFileNode *node )
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   int width, height;
//   RenderOptions *renderOpts;
//   gz_camera_data_t *data;
// 
//   // Camera interface
//   assert(this->cameraIface);
//   if (gz_camera_create(this->cameraIface, this->world->gz_server, this->GetId(),
//                        "SonyVID30", this->GetIntId(), this->GetParentIntId()) !=0)
//     return -1;
//   
//   // PTZ interface
//   assert(this->ptzIface);
//   if (gz_ptz_create(this->ptzIface, this->world->gz_server, this->GetId(),
//                     "SonyVID30", this->GetIntId(), this->GetParentIntId()) !=0)
//     return -1;
// 
//   // Initialize PTZ interface to sensible values
//   this->ptzIface->data->cmd_zoom = M_PI;
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
int SonyVID30::Fini()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   // Finalize external interface  
//   gz_camera_destroy(this->cameraIface);
//   gz_ptz_destroy(this->ptzIface);
// 
//   this->camera->Fini();
    
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update the model
void SonyVID30::Update( double step )
{
//////////////////////////////////////////////////
// COMENTADO PARA FACILITAR REUSO DE CODIGO
//////////////////////////////////////////////////
//   double pan, tilt;    
// 
//   if (this->world->GetSimTime() - this->ptzUpdateTime >= this->ptzUpdatePeriod)
//   {
//     step = this->world->GetSimTime() - this->ptzUpdateTime;
//     
//     this->ptzUpdateTime = this->world->GetSimTime();
//   
//     // Get new commands
//     this->GetPtzCmd();
// 
//     // Apply joint limits to commanded pan/tilt angles
//     if (this->cmdTilt > M_PI*0.5)
//       this->cmdTilt = M_PI*0.5;
//     else if (this->cmdTilt < -M_PI*0.5)
//       this->cmdTilt = -M_PI*0.5;
// 
//     if (this->cmdPan > M_PI*0.3)
//       this->cmdPan = M_PI*0.3;
//     else if (this->cmdPan < -M_PI*0.3)
//       this->cmdPan = -M_PI*0.3;
// 
//     // Apply limits on commanded zoom
//     if (this->cmdZoom < this->zoomMin)
//       this->cmdZoom = this->zoomMin;
//     if (this->cmdZoom > this->zoomMax)
//       this->cmdZoom = this->zoomMax;
//   
//     // Set the pan and tilt motors; can't set angles so track cmds with
//     // a proportional control
//     tilt = this->cmdTilt - this->tiltJoint->GetAngle();
//     pan = this->cmdPan - this->panJoint->GetAngle();
//     this->tiltJoint->SetParam( dParamVel, this->motionGain * tilt);
//     this->panJoint->SetParam( dParamVel, this->motionGain * pan);
// 
//     // Set the zoom using a proportional controller
//     this->zoom += (this->cmdZoom - this->zoom) * this->zoomGain * step;
// 
//     // Update the ptz interface
//     this->PutPtzData();
//   }
// 
//   if (this->world->GetSimTime() - this->cameraUpdateTime >= this->cameraUpdatePeriod)
//   {
//     this->cameraUpdateTime = this->world->GetSimTime();
//             
//     // Update the image; there is an ugly little hack here to set the
//     // imaging plane slightly in front of the camera (so the camera
//     // doesnt image itself)
//     GzVector pos;
//     GzQuatern rot;
//     rot = this->head->GetRotation();
//     pos = this->head->GetPosition();
//     pos = GzCoordPositionAdd(GzVectorSet(0.10, 0, 0), pos, rot);
// 
//     this->camera->SetFOV(this->hfov / this->zoom);
//     this->camera->SetPose(GzPoseSet(pos, rot));
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
// Send PTZ data
void SonyVID30::PutPtzData()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   // Update the PTZ interface
//   gz_ptz_lock(this->ptzIface, 1);
//   this->ptzIface->data->time = this->world->GetSimTime();
//   this->ptzIface->data->pan = this->panJoint->GetAngle();
//   this->ptzIface->data->tilt = this->tiltJoint->GetAngle();
//   this->ptzIface->data->zoom = this->camera->GetFOV();  
//   gz_ptz_unlock(this->ptzIface);  
//   gz_ptz_post(this->ptzIface);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Send data camera data
void SonyVID30::PutCameraData()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////
// Get commands
void SonyVID30::GetPtzCmd()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   gz_ptz_lock(this->ptzIface, 1);
//   
//   this->cmdPan = this->ptzIface->data->cmd_pan;
//   this->cmdTilt = this->ptzIface->data->cmd_tilt;
//   this->cmdZoom = this->hfov / this->ptzIface->data->cmd_zoom;
//   
//   gz_ptz_unlock(this->ptzIface);
//   gz_ptz_post(this->ptzIface);
  
  return;
}

//////////////////////////////////////////////////////////////////
// Inicia a geometria do modelo e os valores padroes das
// propriedades
//////////////////////////////////////////////////////////////////
void SonyVID30::Start()
{
  int imgWidth, imgHeight, zDepth;
  double nearClip, farClip;
  const char *method;

  // Get the time between ptz updates
  this->ptzUpdatePeriod = 0.100;
  this->ptzUpdateTime = -this->ptzUpdatePeriod;

  // Get the time between camera updates
  this->cameraUpdatePeriod = 1.0 /  10 + 1e-6;
  this->cameraUpdateTime = -this->cameraUpdatePeriod;

  // Lens field of view
  this->hfov = 60;

  // Zoom limits
  this->zoomMin = 1;
  this->zoomMax = 10;
  
  // Gain term on PTZ controller
  this->motionGain = 2.0;
  this->zoomGain = 2.0;

  // Initial camera settings
  this->cmdZoom = this->zoomMin;
  this->zoom = this->cmdZoom;

  // Get image dimensions
  imgWidth = 320;
  imgHeight = 240;

  // Get camera parameters
  nearClip = 0.50;
  farClip = 50.0;
  method = "auto";
  zDepth = 16;

  // Path for saving frames
  const char * savePath = ".";
  bool enableSaveFrame = false;
  bool useFrustum = true;


  ////////////////////////// GEOMETRIA /////////////////////////////////////

  dReal mass;
  dReal baseRadius, baseHeight;
  dReal uprightLength, uprightWidth, uprightHeight;
  dReal headLength, headWidth, headHeight;
  dReal lensRadius, lensHeight;
  dReal zOffset;

  mass=0.4; //kg

  baseRadius = 0.071; //meters
  baseHeight = 0.028; //meters

  uprightLength = 0.057; //meters
  uprightWidth = 0.105; //meters
  uprightHeight = 0.025; //meters

  headLength = 0.120; //meters
  headWidth = 0.120; //meters
  headHeight = 0.055; //meters

  lensRadius = 0.024; //meters
  lensHeight = 0.01; //meters

  zOffset = 0.0;

  // The primary cylindrical base
  this->body = new Body( this->world );
  this->bodyGeoms[0] = new CylinderGeom( this->body, this->modelSpaceId, baseRadius, baseHeight);
  this->bodyGeoms[0]->SetRelativePosition( GzVectorSet(0, 0, 0) );
  this->bodyGeoms[0]->SetMass( mass*.5 );
  this->bodyGeoms[0]->SetColor( GzColor(0.3, 0.3, 0.3) );
  this->AddBody( this->body, true );
  zOffset += baseHeight;

  // Rectagular upright
  this->middle = new Body( this->world );
  this->bodyGeoms[1] = new BoxGeom( this->middle, this->modelSpaceId,
                                    uprightLength,uprightWidth, uprightHeight );
  this->bodyGeoms[1]->SetRelativePosition( GzVectorSet(0, 0, zOffset) );
  this->bodyGeoms[1]->SetMass( mass*.25 );
  this->bodyGeoms[1]->SetColor( GzColor(0.3, 0.3, 0.3) );
  this->AddBody( this->middle );
  zOffset += uprightHeight + uprightHeight * 0.5;

  // Rectangular head unit
  this->head = new Body( this->world, "head" );
  this->bodyGeoms[2] = new BoxGeom( this->head, this->modelSpaceId,
                                    headLength, headWidth, headHeight );
  this->bodyGeoms[2]->SetRelativePosition( GzVectorSet(0, 0, zOffset) );
  this->bodyGeoms[2]->SetMass( mass*0.25 );
  this->bodyGeoms[2]->SetColor( GzColor(0.3,0.3,0.3) );

  // The lens
  this->bodyGeoms[3] = new CylinderGeom( this->head, this->modelSpaceId,
                                         lensRadius, lensHeight );
  this->bodyGeoms[3]->SetRelativePosition( GzVectorSet(headWidth/2.0 + lensHeight/2, 0, zOffset) );
  //  this->bodyGeoms[3]->SetColor( GzColor(1, 1, 1) );
  //  this->bodyGeoms[3]->SetAmbientColor( GzColor(1, 1, 1) );
  this->bodyGeoms[3]->SetMass( 0 );
  this->AddBody( this->head );

  GzQuatern a,b,c;
  a = GzQuaternFromAxis(0,1,0,+M_PI/2.0);
  b = GzQuaternFromAxis(0,0,1,+M_PI/2.0);
  c = GzQuaternMul(a,b);
  this->bodyGeoms[3]->SetRelativeRotation( c );

  this->lensGeom = this->bodyGeoms[3];

  //pan joint
  this->panJoint = new HingeJoint( this->world );
  this->panJoint->Attach(this->middle, this->body);
  GzVector ab = this->middle->GetPosition();
  this->panJoint->SetAnchor( ab );
  this->panJoint->SetAxis(GzVectorSet(0,0,1));
  this->panJoint->SetParam( dParamFMax, 10.1 ); 

  //tilt joint
  this->tiltJoint = new HingeJoint( this->world );
  this->tiltJoint->Attach(this->head, this->middle);
  this->tiltJoint->SetAnchor( GzVectorSet(ab.x, ab.y, ab.z+uprightHeight));
  this->tiltJoint->SetAxis(GzVectorSet(0,1,0));
  this->tiltJoint->SetParam( dParamFMax, 10.1 ); 

  this->middle->SetFiniteRotationMode(1);
  this->head->SetFiniteRotationMode(1);
}


