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
/* Desc: An Observer camera model 
 * Author: Nate Koenig
 * Date: 20 May 2004
 * CVS: $Id: ObserverCam.cc,v 1.53 2006/02/22 15:29:24 natepak Exp $
 */

/// @addtogroup models 
/// @{ 
/** @defgroup ObserverCam Observer Camera

The ObserverCam model provides a interactive viewport through which
users can view the simulation (think of it as a flying god's eye).


@par libgazebo interfaces

This model supports the @ref guicam interface.

@par Player drivers

There are no Player drivers for this model.


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
  - Z buffer depth in bits (8, 16, 24, 32)
  - Default: 16

- savePath (string, path)
  - Directory for saving image frames.
  - Default: .

- saveFrames (bool)
  - Set to true to save image frames.
  - Default: false

- displayBBox (bool)
  - Display model bounding boxes.
  - Default: false

- displayAxes (bool)
  - Display body axes for each body.
  - Default: false

- displayCoM (bool)
  - Display center of mass for each body.
  - Default: false

- displayRays (bool)
  - Display sensor rays (e.g., laser, sonar).
  - Default: false

- displayFrustrums (bool)
  - Display camera sensor frustrums.
  - Default: false

- displayMaterials (bool)
  - Use material colors.
  - Default: true

- displayTextures (bool)
  - Use material textures.
  - Default: true

- displaySkins (bool)
  - Display object skins.
  - Default: true

- polygonFill (bool)
  - Display filled polygons.
  - Default: true

- shadeSmooth (bool)
  - Use smooth shading.
  - Default: true

- rollLock (bool)
  - Lock out changes to roll (keeps the camera pointed right-way-up).


@par Bodies

This model does not have a physical representation.


@par Example

@verbatim
<model:ObserverCam>
  <id>userCam0</id>
  <xyz>-0.506 0.894 0.865</xyz>
  <rpy>0 26 -23</rpy>
  <imageSize>640 480</imageSize>
  <updateRate>.10</updateRate>
</model:ObserverCam>
@endverbatim

@par Authors

Andrew Howard, Nate Koenig

*/
/// @} 

#include <assert.h>
#include "gazebo.h"
#include "Body.hh"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"


/////#include "Camera/Camera.hh" //// Comentado para facilitar reudo de código do modelo


#include "ObserverCam.hh"
#include "BoxGeom.hh"
#include "CylinderGeom.hh"

//////////////////////////////////////////////////////////////////////////////
// Register this class
GZ_REGISTER_STATIC("ObserverCam", ObserverCam);


//////////////////////////////////////////////////////////////////////////////
// Constructory
ObserverCam::ObserverCam( World *world )
  : Model(world)
{
  /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  ////////////////////////////////////////////
  this->SetField(new DoubleField("updateRate",0.0));
  this->SetField(new IntegerTupleField("imageSize",IntegerTupleSet(320,240)));
  this->SetField(new DoubleField("nearClip",0.0));
  this->SetField(new DoubleField("farClip",0.0));
  this->SetField(new StringField("renderMethod",""));
  this->SetField(new IntegerField("zBufferDepth",0));
  this->SetField(new StringField("savePath",""));
  this->SetField(new BoolField("saveFrames",false));

  this->SetField(new BoolField("displayBBox",false));
  this->SetField(new BoolField("displayAxes",false));
  this->SetField(new BoolField("displayCoM",false));
  this->SetField(new BoolField("displayRays",false));
  this->SetField(new BoolField("displayFrustums",false));
  this->SetField(new BoolField("displayMaterials",true));
  this->SetField(new BoolField("displayTextures",false));
  this->SetField(new BoolField("displaySkins",false));
  this->SetField(new BoolField("polygonFill",true));
  this->SetField(new BoolField("shadeSmooth",true));
  this->SetField(new BoolField("rollLock",false));
  ////////////////////////////////////////////

////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   this->camera = new Camera(world);
//   this->guicam = NULL;


  this->body=NULL;

  this->lastSpotState = 0;
  
  return;
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
ObserverCam::~ObserverCam()
{
  if (this->body)
    delete this->body;

////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   if (this->guicam)
//     delete this->guicam;
// 
//   delete this->camera;

  this->body=NULL;

////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   this->camera = NULL;
//   this->guicam = NULL;

  return;
}

//////////////////////////////////////////////////////////////////////////////
// Load the model
int ObserverCam::Load( WorldFile *file, WorldFileNode *node )
{
  // Create a dummy body (we have to have one)
  this->body = new Body(this->world, NULL, true);
  this->AddBody(this->body, true);

  // Get the time between updates
  this->updatePeriod = 1 / (node->GetDouble("updateRate", 10.0, 0) + 1e-6);
  this->lastSimTime = -this->updatePeriod;
  this->lastPauseTime = 0.0;

  // Get image dimensions
  int imgWidth = node->GetTupleInt("imageSize", 0, 320);
  int imgHeight = node->GetTupleInt("imageSize", 1, 240);

  // Get camera parameters
  double hfov = node->GetAngle("hfov", 60.0 * M_PI / 180, 0);
  double nearClip = node->GetLength("nearClip", 0.50, 0);
  double farClip = node->GetLength("farClip", 100.0, 0);
  const char *method = node->GetString("renderMethod", "auto");
  int zDepth = node->GetInt("zBufferDepth", 16);
  if (((zDepth % 8) != 0) || (zDepth < 8) || (zDepth > 32))
    zDepth = 16;

////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   // Load the sensor
//   if (this->camera->Init(imgWidth, imgHeight, hfov, nearClip, farClip, 
//         method, zDepth) != 0)
//     return -1;
// 
//   // Path for saving frames
//   this->camera->SetSavePath(node->GetString("savePath", "."));

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int ObserverCam::Init( WorldFile *file, WorldFileNode *node )
{
  int width, height;


////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   gz_guicam_data_t *data;
//   
//   // Initialize the interface
//   this->guicam = gz_guicam_alloc();
//   if (gz_guicam_create(this->guicam, this->world->gz_server, this->GetId(),
//                        "ObserverCam",  this->GetIntId(), this->GetParentIntId()) != 0)
//     return -1;
//   
//   gz_guicam_lock(this->guicam, 1);
// 
//   data = this->guicam->data;
// 
//   // Set image dimensions (used by GUI to set the window size)
//   this->camera->GetImageSize(&width, &height);
//   data->width = width;
//   data->height = height;
//   
//   // Set initial rendering options; note that we set the values in the
//   // interface, which will get copied to the local renderOpt structure
//   // later.
//   data->display_bbox = node->GetBool("displayBBox", false);
//   data->display_axes = node->GetBool("displayAxes", false);
//   data->display_com = node->GetBool("displayCoM", false);
//   data->display_rays = node->GetBool("displayRays", false);
//   data->display_frustrums = node->GetBool("displayFrustrums", false);
//   data->display_materials = node->GetBool("displayMaterials", true);  
//   data->display_textures = node->GetBool("displayTextures", true);
//   data->display_skins = node->GetBool("displaySkins", true);  
//   data->polygon_fill = node->GetBool("polygonFill", true);
//   data->shade_smooth = node->GetBool("shadeSmooth", true);
// 
//   // Set some initial states  
//   data->roll_lock = node->GetBool("rollLock", true);
//   data->save_frames = node->GetBool("saveFrames", false);
// 
//   gz_guicam_unlock(this->guicam);


  // Record our initial pose relative to our parent model (for z-lock)
  if (this->parent)
    this->initialPose = GzCoordPoseSub(this->GetPose(), this->parent->GetPose());

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the model
int ObserverCam::Fini()
{
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////
//   gz_guicam_destroy( this->guicam );
//   gz_guicam_free( this->guicam );
//   this->guicam = NULL;
// 
//   this->camera->Fini();

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update the model state
void ObserverCam::Update( double step )
{
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////

//   GzVector at, up;
//   GzPose pose, delta;
//   double simTime, pauseTime;
// 
//   // We do something odd with the time here, to allow this model to
//   // update when the server is paused.  We will report both times
//   // the interface
//   simTime = this->world->GetSimTime();
//   pauseTime = this->world->GetPauseTime();
// 
//   if (simTime + pauseTime - (this->lastSimTime + this->lastPauseTime) >= this->updatePeriod)
//   {
//     this->lastSimTime = simTime;
//     this->lastPauseTime = pauseTime;
// 
//     // Get current commands
//     this->GetCameraCmd();
// 
//     // Get global pose of model
//     pose = this->GetPose();
// 
//     // Check for state changes
//     if (this->lastSpotState == 0 && this->cmdSpotState == 1)
//     {
//       // Re-generate image data (need to z-value)
//       this->camera->Update();
// 
//       // This is a start event, so get the z value of the hotspot
//       this->cmdSpotA.z = this->camera->GetZValue((int) this->cmdSpotA.x, (int) this->cmdSpotA.y);
//       this->lastSpotState = this->cmdSpotState;
//     }
//     else if (this->lastSpotState == 1 && this->cmdSpotState == 0)
//     {
//       // This is a finish event, so set the model pose
//       delta = this->camera->CalcCameraDelta(this->cmdSpotMode, this->cmdSpotA, this->cmdSpotB);
//       pose = GzCoordPoseAdd(delta, pose);
// 
//       // Apply roll lock
//       if (this->rollLock)
//       {
//         at = GzVectorSet(1, 0, 0);
//         at = GzCoordPositionAdd(at, pose.pos, pose.rot);
//         up = GzVectorSet(0, 0, 1);
//         pose = GzPosePointAt(pose.pos, at, up);
//       }
//         
//       // Compute the pose relative to our parent body
//       if (this->parentBody)
//         delta = GzCoordPoseSub(pose, this->parentBody->GetPose());
//       else
//         delta = pose;
//   
//       // Set the model pose based on the gui feedback.  We may have child
//       // models, so call the world-level function to do a recursive move.
//       this->world->SetModelPose(this, delta);
//       this->lastSpotState = this->cmdSpotState;      
//     }
//       
//     if (this->cmdSpotState != 0)
//     {
//       delta = this->camera->CalcCameraDelta(this->cmdSpotMode, this->cmdSpotA, this->cmdSpotB);
//       pose = GzCoordPoseAdd(delta, pose);
//   
//       // Apply roll lock
//       if (this->rollLock)
//       {
//         at = GzVectorSet(1, 0, 0);
//         at = GzCoordPositionAdd(at, pose.pos, pose.rot);
//         up = GzVectorSet(0, 0, 1);
//         pose = GzPosePointAt(pose.pos, at, up);
//       }
//     }
// 
//     // Set the camera sensor pose (not the model pose)
//     this->camera->SetPose(pose);
//     
//     // See if we should be saving frames or not
//     this->camera->EnableSaveFrame(this->saveFrames);
// 
//     // Set current options
//     this->camera->SetRenderOptions(&this->renderOpt);
// 
//     // Re-generate image data
//     this->camera->Update();
// 
//     // Update the interface
//     this->PutCameraData();
//   }

  return;
}



//////////////////////////////////////////////////////////////////////////////
// Get camera commands
void ObserverCam::GetCameraCmd()
{
////////////////////////////////////////////
// Comentado para simplificar a reutilizacao
// do codigo do modelo
////////////////////////////////////////////

//   gz_guicam_data_t *data;
// 
//   gz_guicam_lock(this->guicam, 1);
// 
//   data = this->guicam->data;
// 
//   this->cmdSpotState = data->cmd_spot_state;
//   this->cmdSpotMode = data->cmd_spot_mode;
//   this->cmdSpotA = GzVectorSet(data->cmd_spot_a[0], data->cmd_spot_a[1], this->cmdSpotA.z);
//   this->cmdSpotB = GzVectorSet(data->cmd_spot_b[0], data->cmd_spot_b[1], this->cmdSpotB.z);
// 
//   this->rollLock = data->roll_lock;
//   this->saveFrames = data->save_frames;
// 
//   this->renderOpt.displayBBox = data->display_bbox;
//   this->renderOpt.displayAxes = data->display_axes;
//   this->renderOpt.displayCoM = data->display_com;  
//   this->renderOpt.displaySkins = data->display_skins;
//   this->renderOpt.displayRays = data->display_rays;
//   this->renderOpt.displayFrustrums = data->display_frustrums;
//   this->renderOpt.displayMaterials = data->display_materials;
//   this->renderOpt.displayTextures = data->display_textures;
//   this->renderOpt.polygonMode = (data->polygon_fill ? GL_FILL : GL_LINE);
//   this->renderOpt.shadeModel = (data->shade_smooth ? GL_SMOOTH : GL_FLAT);
// 
//   gz_guicam_unlock(this->guicam);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Send camera data
void ObserverCam::PutCameraData()
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
//   gz_guicam_data_t *data;
// 
//   gz_guicam_lock(this->guicam, 1);
// 
//   data = this->guicam->data;
// 
//   // Get image dimensions
//   this->camera->GetImageSize(&width, &height);
// 
//   data->sim_time = this->lastSimTime;
//   data->pause_time = this->lastPauseTime;
//   
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
//   gz_guicam_unlock(this->guicam);
//   gz_guicam_post(this->guicam);

  return;
}


/*!
    \fn ObserverCam::Start
 */
void ObserverCam::Start()
{
  //////////////////////////////////////////////////////////
  // Uma geometria foi criada para o ObserverCam para que o 
  // usuario pudesse visualizar o modelo no IRCE e arrastá-lo
  // no mundo virtual
  //////////////////////////////////////////////////////////

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
  geom->SetColor( GzColor(0.1,0.1,0.1) );
  
  // The lens
  geom = new CylinderGeom( this->body, this->modelSpaceId,
                           lensRadius, lensHeight );
  geom->SetRelativePosition( GzVectorSet(headWidth/2.0 + lensHeight/2, 0, zOffset) );
  geom->SetColor( GzColor(1.0, 1.0, 1.0) );
  geom->SetMass( 0 );
  geom->SetRelativeRotation( GzQuaternFromEuler(0, M_PI/2, 0) );

}
