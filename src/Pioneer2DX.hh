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
/* Desc: Model for a Pioneer2DX
 * Author: Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: Pioneer2DX.hh,v 1.10 2004/11/05 08:02:34 inspectorg Exp $
 */

#ifndef PIONEER2DX_HH
#define PIONEER2DX_HH

#include "Model.hh"

// Forward declarations
class Body;
class HingeJoint;
class BallJoint;
// class RayProximity; // COMENTADO PARA FACILITAR O REUSO DO MODELO


class Pioneer2DX : public Model
{
  // Construct, destructor
  public: Pioneer2DX( World *world );
  public: virtual ~Pioneer2DX();

  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );

  // Load ODE stuff
  private: int LoadODE( WorldFile *file, WorldFileNode *node );

  // Load sonar stuff
  private: int LoadSonar( WorldFile *file, WorldFileNode *node );

  // Update the odometry
  private: void UpdateOdometry( double step );

  // Get commands from the external interface
  private: void GetPositionCmd();
  
  // Update the data in the external interface
  private: void PutPositionData();

  // Update the data in the power interface
  private: void PutPowerData();

  // Update the data in the sonar interface
  private: void PutSonarData();
  
  // Update parameters
  private: double updatePeriod, updateTime;

  // Robot parameters
  private: double wheelSep, wheelDiam;

  // Sonar configuration
  private: double sonarMaxRange;
  private: GzVector pos[16];
  private: double dir[16];
  
  // ODE components
  private: Body *chassis;
  private: Body *wheels[2];
  private: HingeJoint *wheelJoints[3];  
  private: Body *castorWheel;
  private: BallJoint *castorJoint;

  // Sensor for sonar ranging
//  private: RayProximity *sonarSensor; // COMENTADO PARA FACILITAR O REUSO DO MODELO

  // External interface
///////////  private: gz_position_t *position_iface; /////// Comentado para evitar erros de compilacao
///////////  private: gz_power_t *power_iface; ///////////// Comentado para evitar erros de compilacao
///////////  private: gz_sonar_t *sonar_iface; ///////////// Comentado para evitar erros de compilacao

  // Wheel speeds (left and right wheels are paired)
  private: int enableMotors;
  private: double wheelSpeed[2];

  // Odometric pose estimate
  private: double odomPose[3];

  // Odometric speed estimate
  private: double odomVel[3];

  // Battery level
  private: double batteryLevel;

  // Coefficients of battery discharge curve
  private: double batteryCurve[2];

  //////////////////////////////////////////////////////////////////
  // Metodos get/set para manipular propriedades com meta-dados
  // utilizadas pelo IRCE
  //////////////////////////////////////////////////////////////////
  public: void Start();
};

#endif
