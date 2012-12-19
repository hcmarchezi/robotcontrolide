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
/* Desc: Base class for all models
 * Author: Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: Model.hh,v 1.36 2006/02/22 14:58:57 natepak Exp $
 */

#ifndef MODEL_HH
#define MODEL_HH

#include <ode/ode.h>
#include "Vector.hh"

#include "GL/gl.h"

// Forward declaraions
class World;
class WorldFile;
class WorldFileNode;
class Body;
class Geom;
class Joint;
class HingeJoint;
class RenderOptions;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// class Model - Classe Model passa a herdar de FieldDiccionary para conter informaćão de Meta-dados
// necessarias ao IRCE
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include "FieldDiccionary.h"

#include "DoubleTuple.h"
#include "IntegerTuple.h"

#include "StringField.h"
#include "GzVectorField.h"
#include "BoolField.h"
#include "DoubleField.h"
#include "GzColorField.h"
#include "IntegerField.h"
#include "DoubleTupleField.h"
#include "IntegerTupleField.h"

class VirtualWorld;

///////// Define algumas macros utilizadas por alguns modelos
#define GAZEBO_TEXTURES_PATH  "."
#define GAZEBO_SKINS_PATH     "."


class Model : public FieldDiccionary /////// Para ser utilizado no IRCE, Model herda de FieldDiccionary
{
  //public: VirtualWorld * virtualWorld; //// Model passa a ter uma referencia para um Virtual World para que seja capaz de achar quem são os seus filhos ( Mais tarde substituir por World )

  // Default constructor
  public: Model( World *world );

  // Destructor
  public: virtual ~Model();

  // Load default values for the entire model. This calls Model::Load
  public: int MasterLoad( WorldFile *file, WorldFileNode *node );
 
  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node ) = 0;
 
  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node ) = 0;

  // Finalize the model
  public: virtual int Fini() = 0;

  // Update the model state
  public: virtual void Update( double step ) = 0;

  // Called for all models. This calls Update
  public: void MasterUpdate( double step );

  // Get the model pose (global frame)
  public: GzPose GetPose();

  // Set the model pose (global frame)
  public: void SetPose( GzPose pose );

  // Log the current pose of the canonical body
  public: void LogPose();

  // Set our parent
  public: void SetParent(Model *parent, Body *parentBody);

  // Attach this model to another. 
  public: void Attach();

  // Detach this model from its parent.
  public: void Detach();

  // Add a body to this model; exactly one body must be used as the
  // canonical body
  protected: void AddBody(Body *body, bool canonical = false);

  // Get the canonical body for this model (returns canonical body by
  // default)
  public: Body *GetBody(const char *bodyName = NULL);

  // Get the list of bodies
  public: int GetNumBodies() {return this->bodyCount;}
  public: Body **GetBodies() {return this->bodies;}

  // Set the id of the model
  public: void SetId(const char* id);

  // Get the id of the model
  public: const char* GetId() const {return this->id;}

  // Set the name of the model
  public: void SetName(const char *name);

  // Get the name of the model
  public: const char* GetName();

  // Set the Pick id of the model
  public: void SetPickId( GLuint id ); 

  // Get the Pick id of the model
  public: GLuint GetPickId() const;

  // Return the integer ID of this model
  public: int GetIntId() const;

  // Return the integer ID of this model's parent
  public: int GetParentIntId() const;


  // Draw this model
  public: virtual void Render( int pass, RenderOptions *renderOpt );

  // The world
  public: World *world;

  // The node this was loaded from
  public: WorldFileNode *node;

  // Our parent model
  public: Model *parent;

  // Our parent body
  public: Body *parentBody;

  /// Initial pose (for reseting)
  public: GzPose initPose;


  // The models id
  private: char *id;

  // The name of this model
  private: char *name;

  // Joint attaching us to parent body (for ODE bodies)
  public: HingeJoint *joint;

  // Pose relative to parent body (for dummy bodies)
  public: GzPose relPose;

  // Flag is true if this is a fixed object
  public: bool fixed;

  // The space for all the bodies in this model
  public: dSpaceID modelSpaceId;

  // A list of bodies making up this model
  protected: int bodyCount, bodyMaxCount;
  protected: Body **bodies;

  // The canonical body (reference for the model cs)
  private: Body *canonicalBody;

  // A list of joints making up this model
  private: int jointCount, jointMaxCount;
  private: Joint **joints;

  private: GLuint pickId;
  private: static GLuint pickIdMaster;

  private: bool gravityMode;
  private: bool enable;

  // Integer based ID 
  private: static int integerIdCounter;
  private: int integerId;

  // TESTING:
  public: bool logPoses;
  public: int poseCount, poseMaxCount;
  public: GzPose *poses;

  public: virtual void Start() = 0;
 
};

#endif

