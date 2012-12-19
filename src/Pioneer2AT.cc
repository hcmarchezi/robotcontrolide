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
/* Desc: Model for a Pioneer2AT
 * Author: Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: Pioneer2AT.cc,v 1.52 2006/02/22 14:58:57 natepak Exp $
 */

/// @addtogroup models 
/// @{
/** @defgroup Pioneer2AT Pioneer 2 AT

@htmlinclude Pioneer2AT_view.html

The Pioneer2AT model simulates an ActivMedia Pioneer2AT mobile robot
base with 16 sonars.

@par libgazebo interfaces

- Position information is available through the @ref position interface.
- Power information is available through the @ref power interface.
- Sonar information is available through the @ref sonar interface.


@par Player drivers

- Position information is available through the %gz_position driver.
- Power information is available through the %gz_power driver.
- Sonar information is available through the %gz_sonar driver.

@par Attributes

The following attributes are supported.

@htmlinclude default_attr_include.html

- updateRate (float, Hz)
  - Updates per second
  - Default: 10

- batteryLevel (float, volts)
  - Initial battery level
  - Default: 12.4

- batteryCurve (float)
  - Discharge curve: about one hour quiescent
  - Default: 2/3600  2/1e4


@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:Pioneer2AT>
  <xyz>0 0 0</xyz>
</model:Pioneer2AT>
@endverbatim

@par Views

@htmlinclude Pioneer2AT_more_views.html

@par Authors

Andrew Howard, Nate Koenig

*/
/// @} 

#include <assert.h>
#include "gazebo.h"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "Body.hh"
#include "BoxGeom.hh"
#include "SphereGeom.hh"
#include "WheelGeom.hh"
#include "HingeJoint.hh"
//#include "RayProximity/RayProximity.hh" ///COMENTADO PARA PERMITIR REUSO DE CODIGO DO MODELO

#include "Pioneer2AT.hh"



//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("Pioneer2AT", Pioneer2AT);


//////////////////////////////////////////////////////////////////////////////
// Constructor
Pioneer2AT::Pioneer2AT( World *world )
    : Model( world )
{
  /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  /////////////////////////////////////////////
  this->SetField(new DoubleField("updateRate",0.0));
  this->SetField(new DoubleField("batteryLevel",0.0));
  this->SetField(new DoubleTupleField("batteryCurve",DoubleTupleSet(0.0,0.0)));
  /////////////////////////////////////////////

  int i;
  this->chassis = NULL;
  for (i=0; i<4; i++)
  {
    this->wheels[i] = NULL;
    this->wheelJoints[i] = NULL;
  }

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
Pioneer2AT::~Pioneer2AT()
{
  int i;
  if (this->chassis)
    delete this->chassis;

  this->chassis = NULL;
  for (i=0; i<4; i++)
  {
    if (this->wheels[i])
      delete this->wheels[i];

    if (this->wheelJoints[i])
      delete this->wheelJoints[i];

    this->wheels[i] = NULL;
    this->wheelJoints[i] = NULL;
  }


  return;
}


//////////////////////////////////////////////////////////////////////////////
// Load the model
int Pioneer2AT::Load( WorldFile *file, WorldFileNode *node )
{
  this->wheelSep = 0.381;
  this->wheelDiam = 0.20;

  this->updatePeriod = 1.0 / (node->GetDouble("updateRate", 10) + 1e-6);
  this->updateTime = -updatePeriod;

  // Create the ODE objects
  if (this->OdeLoad(file, node) != 0)
    return -1;

  // Create the sonar sensor
  if (this->LoadSonar(file, node) != 0)
    return -1;

  // Initial battery level
  this->batteryLevel = node->GetDouble("batteryLevel", 12.4);

  // Discharge curve: about one hour quiescent
  this->batteryCurve[0] = node->GetTupleDouble("batteryCurve", 0, 2 / 3600.0);
  this->batteryCurve[1] = node->GetTupleDouble("batteryCurve", 1, 2 / 1e4);  

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load ODE objects
int Pioneer2AT::OdeLoad( WorldFile *file, WorldFileNode *node )
{
  int i;
  double length, width, height, oz;
  double mass;
  double wheelSep, wheelDiam, wheelThick;
  Geom *geom;

  length = 0.5334;
  width = 0.381;
  height = 0.1905;
  mass = 10.0;

  wheelSep = this->wheelSep;
  wheelDiam = this->wheelDiam;
  wheelThick = 0.07;

  oz = wheelDiam / 2 + height / 2;
  
  // Create the main chassis of the robot
  this->chassis = new Body( this->world );

  geom = new BoxGeom( this->chassis, this->modelSpaceId, 0.9 * length, 0.9 * width, height);
  geom->SetRelativePosition( GzVectorSet(0, 0, oz) );
  geom->SetMass( mass );
  geom->SetColor( GzColor(1, 0, 0) );  

  geom = new BoxGeom( this->chassis, this->modelSpaceId, length, width, 0.02 );
  geom->SetRelativePosition( GzVectorSet(0, 0, oz + 0.55 * height) );
  geom->SetMass( 0 );
  geom->SetColor( GzColor(0, 0, 0) );  

  this->AddBody( this->chassis, true );

  // Create the wheels
  for (i = 0; i < 4; i++)
  {
    this->wheels[i] = new Body( this->world );

    geom = new WheelGeom( this->wheels[i], this->modelSpaceId, wheelDiam/2, wheelThick/2);
    geom->SetRelativePosition( GzVectorSet(0, 0, 0) );
    geom->SetMass( 1.0 );
    geom->SetColor( GzColor(0.3, 0.3, 0.3) );

    this->AddBody( this->wheels[i] );
  }

  this->wheels[0]->SetPosition(GzVectorSet(0.25*length, +0.5 * wheelSep, oz - 0.1));
  this->wheels[0]->SetRotation(GzQuaternFromAxis(1, 0, 0, -M_PI / 2));
  this->wheels[1]->SetPosition(GzVectorSet(0.25*length, -0.5 * wheelSep, oz - 0.1));
  this->wheels[1]->SetRotation(GzQuaternFromAxis(1, 0, 0, +M_PI / 2));
  this->wheels[2]->SetPosition(GzVectorSet(-0.25*length, +0.5 * wheelSep, oz - 0.1));
  this->wheels[2]->SetRotation(GzQuaternFromAxis(1, 0, 0, -M_PI / 2));
  this->wheels[3]->SetPosition(GzVectorSet(-0.25*length, -0.5 * wheelSep, oz - 0.1));
  this->wheels[3]->SetRotation(GzQuaternFromAxis(1, 0, 0, +M_PI / 2));
  
  // Attach the wheels to the chassis
  for (i = 0; i < 4; i++)
  {
    this->wheelJoints[i] = new HingeJoint( this->world );
    this->wheelJoints[i]->Attach( this->wheels[i], this->chassis );

    GzVector a = wheels[i]->GetPosition();    
    this->wheelJoints[i]->SetAnchor( a );
    this->wheelJoints[i]->SetAxis( GzVectorSet(0, 1, 0) );
    this->wheelJoints[i]->SetParam( dParamSuspensionERP, 0.4 );
    this->wheelJoints[i]->SetParam( dParamSuspensionCFM, 0.8 );
  }

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load the sonar
int Pioneer2AT::LoadSonar( WorldFile *file, WorldFileNode *node )
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
// /*  int i;
//   GzVector a, b, p;
//   
//   this->sonarMaxRange = 5.0;
// 
//   // TODO: check dimensions
//   p.x = +0.24103;
//   p.y = +0.135;
//   p.z = +0.25;
//     
//   pos[0] = GzVectorSet( p.x, +p.y, p.z );
//   pos[1] = GzVectorSet( p.x, +p.y, p.z );
//   pos[2] = GzVectorSet( p.x, +p.y - 2 * 0.03429, p.z );
//   pos[3] = GzVectorSet( p.x, +p.y - 4 * 0.03429, p.z );
//   pos[4] = GzVectorSet( p.x, -p.y + 4 * 0.03429, p.z );
//   pos[5] = GzVectorSet( p.x, -p.y + 2 * 0.03429, p.z );
//   pos[6] = GzVectorSet( p.x, -p.y, p.z );
//   pos[7] = GzVectorSet( p.x, -p.y, p.z );
// 
//   p.x = -0.24103;
//   p.y = +0.135;
//   p.z = +0.25;
//   
//   pos[8] = GzVectorSet( p.x, -p.y, p.z );
//   pos[9] = GzVectorSet( p.x, -p.y, p.z );
//   pos[10] = GzVectorSet( p.x, -p.y + 2 * 0.03429, p.z );
//   pos[11] = GzVectorSet( p.x, -p.y + 4 * 0.03429, p.z );
//   pos[12] = GzVectorSet( p.x, +p.y - 4 * 0.03429, p.z );
//   pos[13] = GzVectorSet( p.x, +p.y - 2 * 0.03429, p.z );
//   pos[14] = GzVectorSet( p.x, +p.y, p.z );
//   pos[15] = GzVectorSet( p.x, +p.y, p.z );
//   
//   dir[0] = 90;
//   dir[1] = 50;
//   dir[2] = 30;
//   dir[3] = 10;
//   dir[4] = -10;
//   dir[5] = -30;
//   dir[6] = -50;
//   dir[7] = -90;
//   dir[8] = -90;
//   dir[9] = -130;
//   dir[10] = -150;
//   dir[11] = -170;
//   dir[12] = 170;
//   dir[13] = 150;
//   dir[14] = 130;
//   dir[15] = 90;
// 
//   this->sonarSensor = new RayProximity(this->world, this->chassis, 16);
//   for (i = 0; i < 16; i++)
//   {
//     a = pos[i];
//     b = GzVectorSet(this->sonarMaxRange * cos(dir[i] * M_PI / 180),
//                     this->sonarMaxRange * sin(dir[i] * M_PI / 180), 0.0);
//     b = GzVectorAdd(a, b); 
//     this->sonarSensor->SetRay(i, a, b);
//   }*/
  
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int Pioneer2AT::Init( WorldFile *file, WorldFileNode *node )
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   // Create position interface
//   this->position_iface = gz_position_alloc();
//   assert(this->position_iface);
//   if (gz_position_create(this->position_iface, this->world->gz_server, this->GetId(),
//                          "Pioneer2AT", this->GetIntId(), this->GetParentIntId()) != 0)
//     return -1;
// 
//   // Create power inteface
//   this->power_iface = gz_power_alloc();
//   if (gz_power_create(this->power_iface, this->world->gz_server, this->GetId(),
//                       "Pioneer2AT", this->GetIntId(), this->GetParentIntId()) != 0)
//     return -1;
// 
//   // Create sonar inteface
//   this->sonar_iface = gz_sonar_alloc();
//   if (gz_sonar_create(this->sonar_iface, this->world->gz_server, this->GetId(),
//                        "Pioneer2AT", this->GetIntId(), this->GetParentIntId()) != 0)
//     return -1;
// 
//   // Reset odometric pose
//   this->odomPose[0] = 0.0;
//   this->odomPose[1] = 0.0;
//   this->odomPose[2] = 0.0;

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the model
int Pioneer2AT::Fini()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   // Close sonar interface
//   gz_sonar_destroy( this->sonar_iface );
//   gz_sonar_free( this->sonar_iface );
//   this->sonar_iface = NULL;
// 
//   // Close power interface
//   gz_power_destroy( this->power_iface );
//   gz_power_free( this->power_iface );
//   this->power_iface = NULL;
// 
//   // Close position interface
//   gz_position_destroy( this->position_iface );
//   gz_position_free( this->position_iface );
//   this->position_iface = NULL;

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update model
void Pioneer2AT::Update( double step )
{
  // Do nothing if paused
  if (step == 0)
    return;
  
  // Update the odometry (do this always for better accuracy)
  this->UpdateOdometry( step );

  // Otherwise, update periodically
  if (this->world->GetSimTime() - this->updateTime > this->updatePeriod)
  {
    this->updateTime = this->world->GetSimTime();

    // Get commands from the external interface
    this->GetPositionCmd();

    this->wheelJoints[0]->SetParam( dParamVel, 
                                    this->wheelSpeed[1] / this->wheelDiam * 2 );
    this->wheelJoints[1]->SetParam( dParamVel, 
                                    this->wheelSpeed[0] / this->wheelDiam * 2 );
    this->wheelJoints[2]->SetParam( dParamVel, 
                                    this->wheelSpeed[1] / this->wheelDiam * 2 );
    this->wheelJoints[3]->SetParam( dParamVel, 
                                    this->wheelSpeed[0] / this->wheelDiam * 2 );

    this->wheelJoints[0]->SetParam( dParamFMax, 10.0 );
    this->wheelJoints[1]->SetParam( dParamFMax, 10.0 );
    this->wheelJoints[2]->SetParam( dParamFMax, 10.0 );
    this->wheelJoints[3]->SetParam( dParamFMax, 10.0 );
  
    // Update the sonar sensor
//    this->sonarSensor->Update(); /// COMENTADO PARA PERMITIR REUSO DO CODIGO

    // Update the interface
    this->PutPositionData();
    this->PutPowerData();
    this->PutSonarData();
  }
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Update the odometry
void Pioneer2AT::UpdateOdometry( double step )
{
  double wd, ws;
  double d1, d2;
  double dr, da;

  wd = this->wheelDiam;
  ws = this->wheelSep;

  // Average distance travelled by left and right wheels
  d1 = step * wd / 2 * 
    (wheelJoints[0]->GetAngleRate() + wheelJoints[2]->GetAngleRate()) / 2;
  d2 = step * wd / 2 * 
    (wheelJoints[1]->GetAngleRate() + wheelJoints[3]->GetAngleRate()) / 2;

  dr = (d1 + d2) / 2;
  da = (d2 - d1) / ws;
  
  // Compute odometric pose
  this->odomPose[0] += dr * cos( this->odomPose[2] );
  this->odomPose[1] += dr * sin( this->odomPose[2] );
  this->odomPose[2] += da;

  // Compute odometric instantaneous velocity
  this->odomVel[0] = dr / step;
  this->odomVel[1] = 0.0;
  this->odomVel[2] = da / step;

  // Update the power discharge; this is probably completely bogus
  this->batteryLevel -= this->batteryCurve[0] * step;
  this->batteryLevel -= this->batteryCurve[1] * d1 * d1;
  this->batteryLevel -= this->batteryCurve[1] * d2 * d2;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get commands from the external interface
void Pioneer2AT::GetPositionCmd()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   double vr, va;
// 
//   gz_position_lock(this->position_iface, 1);
//   vr = this->position_iface->data->cmd_vel_pos[0];
//   va = this->position_iface->data->cmd_vel_rot[2];
//   gz_position_unlock(this->position_iface);
//   
//   this->wheelSpeed[0] = vr + va * this->wheelSep / 2;
//   this->wheelSpeed[1] = vr - va * this->wheelSep / 2;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Update the data in the erinterface
void Pioneer2AT::PutPositionData()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   gz_position_lock(this->position_iface, 1);
//     
//   // Data timestamp
//   this->position_iface->data->time = this->world->GetSimTime();
// 
//   this->position_iface->data->pos[0] = this->odomPose[0];
//   this->position_iface->data->pos[1] = this->odomPose[1];
//   this->position_iface->data->rot[2] = this->odomPose[2];
// 
//   this->position_iface->data->vel_pos[0] = this->odomVel[0];
//   this->position_iface->data->vel_rot[2] = this->odomVel[2];
// 
//   gz_position_unlock(this->position_iface);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Update the data in the power interface
void Pioneer2AT::PutPowerData()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   gz_power_lock(this->power_iface, 1);
//     
//   this->power_iface->data->time = this->world->GetSimTime();
//   this->power_iface->data->levels[0] = this->batteryLevel;
// 
//   gz_power_unlock(this->power_iface);
    
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Update the data in the sonar interface
void Pioneer2AT::PutSonarData()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   int i;
//   double r;
// 
//   gz_sonar_lock(this->sonar_iface, 1);
//     
//   // Data timestamp
//   this->sonar_iface->data->time = this->world->GetSimTime();
// 
//   // Sonar count valid
//   this->sonar_iface->data->sonar_count = 16;
//   
//   // Sonar's data
//   for (i = 0; i < 16; i++)
//   {
//     r = Min(this->sonarSensor->GetRange(i), this->sonarMaxRange);
// 
//     this->sonar_iface->data->sonar_pos[i][0] = pos[i].x;
//     this->sonar_iface->data->sonar_pos[i][1] = pos[i].y;
//     this->sonar_iface->data->sonar_pos[i][2] = pos[i].z;
// 
//     this->sonar_iface->data->sonar_rot[i][0] = 0;
//     this->sonar_iface->data->sonar_rot[i][1] = 0;
//     this->sonar_iface->data->sonar_rot[i][2] = dir[i] * M_PI / 180;
// 
//     this->sonar_iface->data->sonar_ranges[i] = r;
//   }
// 
//   gz_sonar_unlock(this->sonar_iface);
    
  return;
}

void Pioneer2AT::Start()
{
  this->wheelSep = 0.381;
  this->wheelDiam = 0.20;

  int i;
  double length, width, height, oz;
  double mass;
  double wheelSep, wheelDiam, wheelThick;
  Geom *geom;

  length = 0.5334;
  width = 0.381;
  height = 0.1905;
  mass = 10.0;

  wheelSep = this->wheelSep;
  wheelDiam = this->wheelDiam;
  wheelThick = 0.07;

  oz = wheelDiam / 2 + height / 2;
  
  // Create the main chassis of the robot
  this->chassis = new Body( this->world );

  geom = new BoxGeom( this->chassis, this->modelSpaceId, 0.9 * length, 0.9 * width, height);
  geom->SetRelativePosition( GzVectorSet(0, 0, oz) );
  geom->SetMass( mass );
  geom->SetColor( GzColor(1, 0, 0) );  

  geom = new BoxGeom( this->chassis, this->modelSpaceId, length, width, 0.02 );
  geom->SetRelativePosition( GzVectorSet(0, 0, oz + 0.55 * height) );
  geom->SetMass( 0 );
  geom->SetColor( GzColor(0, 0, 0) );  

  this->AddBody( this->chassis, true );

  // Create the wheels
  for (i = 0; i < 4; i++)
  {
    this->wheels[i] = new Body( this->world );

    geom = new WheelGeom( this->wheels[i], this->modelSpaceId, wheelDiam/2, wheelThick/2);
    geom->SetRelativePosition( GzVectorSet(0, 0, 0) );
    geom->SetMass( 1.0 );
    geom->SetColor( GzColor(0.3, 0.3, 0.3) );

    this->AddBody( this->wheels[i] );
  }

  this->wheels[0]->SetPosition(GzVectorSet(0.25*length, +0.5 * wheelSep, oz - 0.1));
  this->wheels[0]->SetRotation(GzQuaternFromAxis(1, 0, 0, -M_PI / 2));
  this->wheels[1]->SetPosition(GzVectorSet(0.25*length, -0.5 * wheelSep, oz - 0.1));
  this->wheels[1]->SetRotation(GzQuaternFromAxis(1, 0, 0, +M_PI / 2));
  this->wheels[2]->SetPosition(GzVectorSet(-0.25*length, +0.5 * wheelSep, oz - 0.1));
  this->wheels[2]->SetRotation(GzQuaternFromAxis(1, 0, 0, -M_PI / 2));
  this->wheels[3]->SetPosition(GzVectorSet(-0.25*length, -0.5 * wheelSep, oz - 0.1));
  this->wheels[3]->SetRotation(GzQuaternFromAxis(1, 0, 0, +M_PI / 2));
  
  // Attach the wheels to the chassis
  for (i = 0; i < 4; i++)
  {
    this->wheelJoints[i] = new HingeJoint( this->world );
    this->wheelJoints[i]->Attach( this->wheels[i], this->chassis );

    GzVector a = wheels[i]->GetPosition();    
    this->wheelJoints[i]->SetAnchor( a );
    this->wheelJoints[i]->SetAxis( GzVectorSet(0, 1, 0) );
    this->wheelJoints[i]->SetParam( dParamSuspensionERP, 0.4 );
    this->wheelJoints[i]->SetParam( dParamSuspensionCFM, 0.8 );
  }
}






