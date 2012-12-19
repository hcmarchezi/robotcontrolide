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
/* Desc: Model for Pioneer2 Gripper
 * Author: Carle Cote 
 */

/// @addtogroup models 
/// @{
/** @defgroup Pioneer2Gripper Pioneer 2 Gripper


  @htmlinclude Pioneer2Gripper_view.html


  The Pioneer2Gripper model simulates the ActivMedia Pioneer2 Gripper. 

  @par libgazebo interfaces

  This model supports the @ref gripper interface.

  @par Player drivers

  Control of the gripper position is available through the %gz_gripper driver.


  @par Attributes

  The following attributes are supported.

  @htmlinclude default_attr_include.html


  @par Bodies

  The following bodies are created by this model.

  @htmlinclude default_body_include.html

  @par Example

  @verbatim
  <model:Pioneer2Gripper>
  <xyz>0 0 0</xyz>
  </model:Pioneer2Gripper>
  @endverbatim

  @par Views

  @htmlinclude Pioneer2Gripper_more_views.html

  @par Authors

  Carle Cote

*/
/// @}


#include <assert.h>

#include "gazebo.h"
#include "World.hh"
#include "Body.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "RayGeom.hh"
#include "BoxGeom.hh"
#include "Pioneer2Gripper.hh"
#include "SliderJoint.hh"
#include "RayProximity.hh"


//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("Pioneer2Gripper", Pioneer2Gripper);


//////////////////////////////////////////////////////////////////////////////
// Constructor
Pioneer2Gripper::Pioneer2Gripper( World *world ) : Model( world )
{  
  this->rightVel = -0.5;
  this->leftVel = 0.5;
  this->liftVel = 0.5;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
Pioneer2Gripper::~Pioneer2Gripper()
{
}

//////////////////////////////////////////////////////////////////////////////
// Load the model
int Pioneer2Gripper::Load( WorldFile *file, WorldFileNode *node )
{
  // Create the ODE objects
  if ( this->OdeLoad( file, node ) != 0 )
    return -1;

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Load ODE objects
int Pioneer2Gripper::OdeLoad( WorldFile *file, WorldFileNode *node )
{
  Geom *geom = NULL;
  GzVector baseSize = GzVectorSet(0.05, 0.125, 0.13);
  GzVector horizBarSize = GzVectorSet(0.07, 0.315, 0.065);
  GzVector paddleSize = GzVectorSet(0.095, 0.025, 0.04);
  GzVector pos;


  // Create the base mount for the gripper
  this->base = new Body(this->world);
  geom = new BoxGeom(this->base, this->modelSpaceId, baseSize.x, baseSize.y, baseSize.z);
  geom->SetMass(0.10);
  geom->SetColor(GzColor(0, 0, 0));
  this->AddBody(this->base, true);


  // Setup the horizontal bar. Moves vertically
  this->horizBar = new Body(this->world);
  geom = new BoxGeom(this->horizBar, this->modelSpaceId, horizBarSize.x, horizBarSize.y,horizBarSize.z);
  geom->SetMass(0.00);
  geom->SetColor(GzColor(0.5, 0.5 ,0.5));
  this->AddBody(this->horizBar);
  pos = this->base->GetPosition();
  pos.x += (baseSize.x * 0.5) + (horizBarSize.x * 0.5);
  pos.z -= baseSize.z * 0.5;
  this->horizBar->SetPosition(pos);


  // Create the left paddle
  this->leftPaddle = new Body(this->world);
  geom = new BoxGeom(this->leftPaddle, this->modelSpaceId, paddleSize.x, paddleSize.y, paddleSize.z);
  geom->SetMass(0.0);
  geom->SetColor(GzColor(0.2, 0.2, 0.2));
  geom->SetFriction(dInfinity,dInfinity);
  this->AddBody(this->leftPaddle);
  pos = this->horizBar->GetPosition();
  pos.x += (horizBarSize.x * 0.5) + (paddleSize.x * 0.5);
  pos.y -= (horizBarSize.y-0.08)*0.5;
  this->leftPaddle->SetPosition(pos);


  // Create the right paddle
  this->rightPaddle = new Body(this->world);
  geom = new BoxGeom(this->rightPaddle, this->modelSpaceId, paddleSize.x, paddleSize.y, paddleSize.z);
  geom->SetMass(0.0);
  geom->SetColor(GzColor(0.2, 0.2, 0.2));
  geom->SetFriction(dInfinity,dInfinity);
  this->AddBody(this->rightPaddle);
  pos = this->horizBar->GetPosition();
  pos.x += (horizBarSize.x * 0.5) + (paddleSize.x * 0.5);
  pos.y += (horizBarSize.y-0.08)*0.5;
  this->rightPaddle->SetPosition(pos);


  // Setup the slider joint for the horizontal bar
  this->horizBarSlider = new SliderJoint(this->world->worldId); 
  this->horizBarSlider->Attach(this->horizBar, this->base);
  this->horizBarSlider->SetAxis(0,0,1);
  this->horizBarSlider->SetParam(dParamLoStop,0.0);//-baseSize.y*0.5);
  this->horizBarSlider->SetParam(dParamHiStop,baseSize.y*0.5);
  this->horizBarSlider->SetParam(dParamBounce,0.0);
  this->horizBarSlider->SetParam(dParamVel,0.0);
  this->horizBarSlider->SetParam(dParamFMax,50.0);


  // Create the slider for the right slider
  this->rightSlider = new SliderJoint(this->world->worldId);
  this->rightSlider->Attach(this->rightPaddle, this->horizBar);
  this->rightSlider->SetAxis(0,1,0);
  this->rightSlider->SetParam(dParamLoStop,-(horizBarSize.y-0.08-paddleSize.y)*0.5);//-baseSize.y*0.5);
  this->rightSlider->SetParam(dParamHiStop,0.0);
  this->rightSlider->SetParam(dParamBounce,0.0);
  this->rightSlider->SetParam(dParamVel,0.0);
  this->rightSlider->SetParam(dParamFMax,10.0);


  // Create the slider for the left slider
  this->leftSlider = new SliderJoint(this->world->worldId);
  this->leftSlider->Attach(this->leftPaddle, this->horizBar);
  this->leftSlider->SetAxis(0,1,0);
  this->leftSlider->SetParam(dParamLoStop,0.0);
  this->leftSlider->SetParam(dParamHiStop,(horizBarSize.y-0.08-paddleSize.y)*0.5);
  this->leftSlider->SetParam(dParamBounce,0.0);
  this->leftSlider->SetParam(dParamVel,0.0);
  this->leftSlider->SetParam(dParamFMax,10.0);

  this->frontBreakBeam = new RayProximity(this->world,this->leftPaddle,1);
  this->rearBreakBeam = new RayProximity(this->world,this->leftPaddle,1);

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int Pioneer2Gripper::Init( WorldFile *file, WorldFileNode *node )
{  
  // Initialize ODE objects
  if ( this->OdeInit( file, node ) != 0 )
    return -1;

  // Initialize external interface
  if ( this->IfaceInit() != 0 )
    return -1;
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize ODE
int Pioneer2Gripper::OdeInit( WorldFile *file, WorldFileNode *node )
{
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Initialize the external interface
int Pioneer2Gripper::IfaceInit()
{
//////////////////////////////////////////////////////////////////
// COMENTADO PARTES NAO UTILIZADAS PARA FACILTIAR REUSO DO CODIGO
//////////////////////////////////////////////////////////////////
//   this->iface = gz_gripper_alloc();
// 
//   if (gz_gripper_create(this->iface, this->world->gz_server, this->GetId(), "Pioneer2Gripper", this->GetIntId(), this->GetParentIntId()) != 0)
//     return -1;

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the model
int Pioneer2Gripper::Fini()
{
  // Finalize external interface  
  this->IfaceFini();

  // Finalize ODE objects
  this->OdeFini();

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize ODE
int Pioneer2Gripper::OdeFini()
{
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Finalize the external interface
int Pioneer2Gripper::IfaceFini()
{
//////////////////////////////////////////////////////////////////
// COMENTADO PARTE NAO UTILIZAVEL PARA FACILITAR REUSO DE CODIGO
//////////////////////////////////////////////////////////////////
//   gz_gripper_destroy( this->iface );
//   gz_gripper_free( this->iface );
//   this->iface = NULL;

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Get commands from the external interface
void Pioneer2Gripper::IfaceGetCmd()
{

  // Receives command from external interface.

  // Commands are defined in playerclient.h in Player's client directory.
  // GRIPopen   1
  // GRIPclose  2
  // GRIPstop   3
  // LIFTup     4
  // LIFTdown   5
  // LIFTstop   6
  // GRIPstore  7
  // GRIPdeploy 8
  // GRIPhalt   15
  // GRIPpress  16 -- not supported
  // LIFTcarry  17 -- not supported 

  switch (this->iface->data->cmd)
  {
    case 1:
      this->GripOpen();
      break;
    case 2:
      this->GripClose();
      break;
    case 3:
      this->GripStop();
      break;
    case 4:
      this->LiftUp();
      break;
    case 5:
      this->LiftDown();
      break;
    case 6:
      this->LiftStop();
      break;
    case 7:
      this->GripClose();
      this->LiftUp();
      break;
    case 8:
      this->GripOpen();
      this->LiftDown();
      break;
    case 15:
      this->GripStop();
      this->LiftStop();
      break;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
// Update external interface
void Pioneer2Gripper::IfacePutData()
{
  // Data timestamp
  this->iface->data->time = this->world->GetSimTime();

  this->iface->data->left_paddle_open = this->leftPaddleState == OPEN;
  this->iface->data->right_paddle_open = this->rightPaddleState == OPEN;

  this->iface->data->lift_up = this->liftState == UP;
  this->iface->data->lift_down = this->liftState == DOWN;

  this->iface->data->outer_beam_obstruct = this->frontBreakBeam->GetRange(0) < DBL_MAX;
  this->iface->data->inner_beam_obstruct = this->rearBreakBeam->GetRange(0) < DBL_MAX;

  if (this->leftPaddleState == OPEN && this->rightPaddleState == OPEN)
    this->iface->data->state = OPEN;
  else if (this->leftPaddleState == CLOSED && this->rightPaddleState == CLOSED)
    this->iface->data->state = CLOSED;
  else 
    this->iface->data->state = MOVING;
}

//////////////////////////////////////////////////////////////////////////////
// Update the model
void Pioneer2Gripper::Update( double step )
{
  // Get commands from the external interface
  this->IfaceGetCmd();

  GzVector start;
  GzVector end;
  GzVector diff;

  this->frontBreakBeam->Update();

  // Reltative Distance between left and right paddles
  diff = GzCoordPositionSub(this->rightPaddle->GetPosition(), 
                           this->leftPaddle->GetPosition(),
                           this->GetPose().rot);

  // Update the front break beam
  {
    // Calc the start position of the front break beam 
    start = GzCoordPositionAdd(GzVectorSet(0,0,0),
        GzVectorSet(0.03,0.02,0),
        this->GetPose().rot);
    // Move the end position in to avoid hitting the paddle
    end = GzVectorSub(diff,GzVectorSet(0.0, 0.035,0));
    // Get the position relative to the start
    end = GzVectorAdd(start,end);
    this->frontBreakBeam->SetRay(0,start,end);
  }


  // Update the rear break beam
  {
    // Calc the start position of the rear break beam 
    start = GzCoordPositionAdd(GzVectorSet(0,0,0),
                               GzVectorSet(-0.02,0.02,0),
                               this->GetPose().rot);
    // Move the end position in to avoid hitting the paddle
    end = GzVectorSub(diff,GzVectorSet(0.0, 0.035,0));
    // Get the position relative to the start
    end = GzVectorAdd(start,end);

    this->rearBreakBeam->SetRay(0,start,end);
  }

  // Create temp variables
  // Update the mmap interface with the new data
  this->IfacePutData();
}

void Pioneer2Gripper::GripOpen()
{
  this->rightSlider->SetParam(dParamVel, -this->rightVel);
  this->leftSlider->SetParam(dParamVel, -this->leftVel );

  this->rightPaddleState = OPEN;
  this->leftPaddleState = OPEN;
}

void Pioneer2Gripper::GripClose()
{
  this->rightSlider->SetParam(dParamVel, this->rightVel);
  this->leftSlider->SetParam(dParamVel, this->leftVel );

  this->rightPaddleState = CLOSED;
  this->leftPaddleState = CLOSED;
}

void Pioneer2Gripper::GripStop()
{
  this->rightSlider->SetParam(dParamVel, 0.0);
  this->leftSlider->SetParam(dParamVel, 0.0 );
}

void Pioneer2Gripper::LiftUp()
{
  this->horizBarSlider->SetParam(dParamVel, this->liftVel);
  this->liftState = UP;
}

void Pioneer2Gripper::LiftDown()
{
  this->horizBarSlider->SetParam(dParamVel, -this->liftVel);
  this->liftState = DOWN;
}

void Pioneer2Gripper::LiftStop()
{
  this->horizBarSlider->SetParam(dParamVel, 0.0);
}

void Pioneer2Gripper::Start()
{
  Geom *geom = NULL;
  GzVector baseSize = GzVectorSet(0.05, 0.125, 0.13);
  GzVector horizBarSize = GzVectorSet(0.07, 0.315, 0.065);
  GzVector paddleSize = GzVectorSet(0.095, 0.025, 0.04);
  GzVector pos;


  // Create the base mount for the gripper
  this->base = new Body(this->world);
  geom = new BoxGeom(this->base, this->modelSpaceId, baseSize.x, baseSize.y, baseSize.z);
  geom->SetMass(0.10);
  geom->SetColor(GzColor(0, 0, 0));
  this->AddBody(this->base, true);


  // Setup the horizontal bar. Moves vertically
  this->horizBar = new Body(this->world);
  geom = new BoxGeom(this->horizBar, this->modelSpaceId, horizBarSize.x, horizBarSize.y,horizBarSize.z);
  geom->SetMass(0.00);
  geom->SetColor(GzColor(0.5, 0.5 ,0.5));
  this->AddBody(this->horizBar);
  pos = this->base->GetPosition();
  pos.x += (baseSize.x * 0.5) + (horizBarSize.x * 0.5);
  pos.z -= baseSize.z * 0.5;
  this->horizBar->SetPosition(pos);


  // Create the left paddle
  this->leftPaddle = new Body(this->world);
  geom = new BoxGeom(this->leftPaddle, this->modelSpaceId, paddleSize.x, paddleSize.y, paddleSize.z);
  geom->SetMass(0.0);
  geom->SetColor(GzColor(0.2, 0.2, 0.2));
  geom->SetFriction(dInfinity,dInfinity);
  this->AddBody(this->leftPaddle);
  pos = this->horizBar->GetPosition();
  pos.x += (horizBarSize.x * 0.5) + (paddleSize.x * 0.5);
  pos.y -= (horizBarSize.y-0.08)*0.5;
  this->leftPaddle->SetPosition(pos);


  // Create the right paddle
  this->rightPaddle = new Body(this->world);
  geom = new BoxGeom(this->rightPaddle, this->modelSpaceId, paddleSize.x, paddleSize.y, paddleSize.z);
  geom->SetMass(0.0);
  geom->SetColor(GzColor(0.2, 0.2, 0.2));
  geom->SetFriction(dInfinity,dInfinity);
  this->AddBody(this->rightPaddle);
  pos = this->horizBar->GetPosition();
  pos.x += (horizBarSize.x * 0.5) + (paddleSize.x * 0.5);
  pos.y += (horizBarSize.y-0.08)*0.5;
  this->rightPaddle->SetPosition(pos);


  // Setup the slider joint for the horizontal bar
  this->horizBarSlider = new SliderJoint(this->world->worldId); 
  this->horizBarSlider->Attach(this->horizBar, this->base);
  this->horizBarSlider->SetAxis(0,0,1);
  this->horizBarSlider->SetParam(dParamLoStop,0.0);//-baseSize.y*0.5);
  this->horizBarSlider->SetParam(dParamHiStop,baseSize.y*0.5);
  this->horizBarSlider->SetParam(dParamBounce,0.0);
  this->horizBarSlider->SetParam(dParamVel,0.0);
  this->horizBarSlider->SetParam(dParamFMax,50.0);


  // Create the slider for the right slider
  this->rightSlider = new SliderJoint(this->world->worldId);
  this->rightSlider->Attach(this->rightPaddle, this->horizBar);
  this->rightSlider->SetAxis(0,1,0);
  this->rightSlider->SetParam(dParamLoStop,-(horizBarSize.y-0.08-paddleSize.y)*0.5);//-baseSize.y*0.5);
  this->rightSlider->SetParam(dParamHiStop,0.0);
  this->rightSlider->SetParam(dParamBounce,0.0);
  this->rightSlider->SetParam(dParamVel,0.0);
  this->rightSlider->SetParam(dParamFMax,10.0);


  // Create the slider for the left slider
  this->leftSlider = new SliderJoint(this->world->worldId);
  this->leftSlider->Attach(this->leftPaddle, this->horizBar);
  this->leftSlider->SetAxis(0,1,0);
  this->leftSlider->SetParam(dParamLoStop,0.0);
  this->leftSlider->SetParam(dParamHiStop,(horizBarSize.y-0.08-paddleSize.y)*0.5);
  this->leftSlider->SetParam(dParamBounce,0.0);
  this->leftSlider->SetParam(dParamVel,0.0);
  this->leftSlider->SetParam(dParamFMax,10.0);

  this->frontBreakBeam = new RayProximity(this->world,this->leftPaddle,1);
  this->rearBreakBeam = new RayProximity(this->world,this->leftPaddle,1);
}