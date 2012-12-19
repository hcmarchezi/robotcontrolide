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
 * CVS: $Id: Pioneer2Gripper.hh,v 1.9 2006/11/02 16:54:09 natepak Exp $
 */

#ifndef GAZEBO_P2GRIPPER_HH
#define GAZEBO_P2GRIPPER_HH

#include "Model.hh"

// Forward declarations
class Body;
class SliderJoint;
class RayProximity;

typedef struct gz_gripper gz_gripper_t;

class Pioneer2Gripper : public Model
{
  private: enum GripperState {OPEN,CLOSED,UP,DOWN,MOVING,STOPPED};

  // Constructor, destructor
  public: Pioneer2Gripper( World *world );
  public: virtual ~Pioneer2Gripper();

  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );

  // Grips-intersection callback
  private: static void UpdateGripCallback( void *data, dGeomID o1, dGeomID o2 );

  // Load ODE stuff
  private: int OdeLoad( WorldFile *file, WorldFileNode *node );

  // Initialize ODE
  private: int OdeInit( WorldFile *file, WorldFileNode *node );

  // Finalize ODE
  private: int OdeFini();

  // Initialize the external interface
  private: int IfaceInit();

  // Finalize the external interface
  private: int IfaceFini();

  // Get commands from the external interface
  private: void IfaceGetCmd();

  // Update the data in the external interface
  private: void IfacePutData();  
  
  private: void GripOpen();
  private: void GripClose();
  private: void GripStop();
  private: void LiftUp();
  private: void LiftDown();
  private: void LiftStop();


  // ODE objects
  private: Body *base;
  private: Body *horizBar;
  private: Body *leftPaddle;
  private: Body *rightPaddle;

  private: SliderJoint *horizBarSlider;
  private: SliderJoint *rightSlider;
  private: SliderJoint *leftSlider;

  private: float liftVel;
  private: float rightVel;
  private: float leftVel;

  private: GripperState leftPaddleState;
  private: GripperState rightPaddleState;
  private: GripperState liftState;

  private: RayProximity *frontBreakBeam;
  private: RayProximity *rearBreakBeam;

  // External interface
  private: gz_gripper_t *iface;

  ////////////////////////////////////////////
  // Metodo utilizado para iniciar o modelo 
  // com os valores padroes das propriedades
  ////////////////////////////////////////////
  public: void Start();
};

#endif
