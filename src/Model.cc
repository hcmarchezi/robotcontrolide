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
/* Desc: Base class for all models.
 * Author: Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: Model.cc,v 1.42 2006/03/12 17:38:56 natepak Exp $
 */
#include <GL/glu.h>
#include <assert.h>

#include "World.hh"
#include "Body.hh"
#include "Geom.hh"
#include "HingeJoint.hh"
#include "Model.hh"
#include "WorldFile.hh"


#include "VirtualWorld.h"

GLuint Model::pickIdMaster = 1;
int Model::integerIdCounter = 1;

#ifndef PI_NUMERIC
#define PI_NUMERIC 3.14159265
#endif



//////////////////////////////////////////////////////////////////////////////
// Default constructor
Model::Model( World *world )
{
  /////////////////////////////////////////////////
  // Inserindo informacao de Meta-Dados para o IRCE
  /////////////////////////////////////////////////
  this->SetField(new StringField("id",""));
  this->SetField(new GzVectorField("xyz",GzVectorSet(0.0,0.0,0.0)));
  this->SetField(new GzVectorField("rpy",GzVectorSet(0.0,0.0,0.0)));
  this->SetField(new BoolField("gravity",true));
  this->SetField(new BoolField("enable",true));
  /////////////////////////////////////////////////

  this->world = world;
  this->parent = NULL;
  this->parentBody = NULL;
  this->joint = NULL;

  this->fixed = true;
  this->modelSpaceId = dSimpleSpaceCreate( this->world->spaceId );
  
  this->bodyCount = 0;
  this->bodyMaxCount = 0;
  this->bodies = NULL;
  
  this->canonicalBody = NULL;

  this->jointCount = 0;
  this->jointMaxCount = 0;
  this->joints = NULL;

  this->logPoses = false;
  this->poseCount = 0;
  this->poseMaxCount = 0;
  this->poses = NULL;

  this->SetPickId(pickIdMaster++);

  this->integerId = integerIdCounter++;

  this->id = NULL;
  this->name = NULL;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
Model::~Model()
{
  if (this->joint)
    delete this->joint;

  if (this->joints)
    free(this->joints);

  if (this->bodies)
    free(this->bodies);

  if (this->poses)
    free(this->poses);

  if (this->id)
    delete [] id;

  return;
}

//////////////////////////////////////////////////////////////////////////////
// Load some values that are global to this model
int Model::MasterLoad( WorldFile *file, WorldFileNode *node )
{
  // Read some default values
  this->gravityMode = node->GetBool("gravity", true);
  this->enable = node->GetBool("enabled", true);

  // Load the model specific info
  return this->Load(file, node);
}

//////////////////////////////////////////////////////////////////////////////
// Generic Update. Called for all models. This calls Update
void Model::MasterUpdate( double step )
{
  if (this->logPoses)
    this->LogPose();

  this->Update(step);
}


//////////////////////////////////////////////////////////////////////////////
// Set our parent
void Model::SetParent(Model *parent, Body *parentBody)
{
  assert(this->parent == NULL);
  
  this->parent = parent;
  this->parentBody = parentBody;
  this->SetPickId( this->parent->GetPickId() );

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Attach this model to another.
void Model::Attach()
{
  GzVector pos;

  assert(this->parent != NULL);
  assert(this->parentBody != NULL);
  assert(this->GetBody());

  // If both we and our parent have real ODE bodies, create an ODE
  // joint
  if (this->GetBody()->GetBodyId() && this->parentBody->GetBodyId())
  {
    pos = this->GetBody()->GetPosition();
    
    this->joint = new HingeJoint( this->world );
    this->joint->Attach( this->GetBody(), this->parentBody );
    this->joint->SetAnchor( pos );
    this->joint->SetAxis( GzVectorSet(0, 0, 1) );
    this->joint->SetParam( dParamHiStop, 0 );
    this->joint->SetParam( dParamLoStop, 0 );
  }

  // If either we or our parent has a dummy body, just store the
  // relative pose
  else
  {
    this->joint = NULL;
    this->relPose = GzCoordPoseSub(this->GetBody()->GetPose(), this->parentBody->GetPose());
  }

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Detach this model from another.
void Model::Detach()
{
  this->joint->Detach();
  delete this->joint;
  this->joint = NULL;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Add a body to this model
void Model::AddBody(Body *body, bool canonical)
{
  // Re-size array as needed
  if (this->bodyCount >= this->bodyMaxCount)
  {
    this->bodyMaxCount += 10;
    this->bodies = (Body**) realloc(this->bodies, this->bodyMaxCount * 
                                    sizeof(this->bodies[0]));
    assert(this->bodies);
  }

  body->SetPickId(this->pickId);

  // Add body to array
  this->bodies[this->bodyCount++] = body;

  // Exactly one body should be used as the canonical body
  if (canonical)
  {
    assert(this->canonicalBody == NULL);
    this->canonicalBody = body;

    // This is a bit naughty: make sure the names are consistent
    body->bodyName = "canonical";
  }

  body->SetGravityMode(this->gravityMode);
  body->Enable(this->enable);
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the canonical body for this model
Body *Model::GetBody(const char *bodyName)
{
  int i;
  Body *body;
  
  if (bodyName == NULL)
    return this->canonicalBody;
  if (strcmp(bodyName, "canonical") == 0)
    return this->canonicalBody;

  for (i = 0; i < this->bodyCount; i++)
  {
    body = this->bodies[i];
    if (body->bodyName == NULL)
      continue;
    if (strcmp(bodyName, body->bodyName) == 0)
      return body;
  }

  return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Set the id of the model
void Model::SetId(const char *id)
{
  if (id != NULL)
  {
    if (this->id != NULL)
    {
      delete [] this->id;
      this->id = NULL;
    }

    this->id = new char[strlen(id)+1];
    strcpy(this->id,id);

    // Set the null character, just to be safe.
    this->id[strlen(id)] = '\0';

    ////////////////////////////////////////////////////////////
    //////////// Propriedada de meta-dados eh atualizada
    ((StringField*)this->GetField("id"))->SetValue( id ); 
    ////////////////////////////////////////////////////////////

  }
}


//////////////////////////////////////////////////////////////////////////////
// Set the name of the model
void Model::SetName(const char *name)
{
  if (name != NULL)
  {
    if (this->name != NULL)
    {
      delete [] this->name;
      this->name = NULL;
    }

    this->name = new char[strlen(name)+1];
    strcpy(this->name,name);

    // Set the null character, just to be safe.
    this->name[strlen(name)] = '\0';
  }
}


//////////////////////////////////////////////////////////////////////////////
// Get the name of the model
const char* Model::GetName()
{
  return this->name;
}


//////////////////////////////////////////////////////////////////////////////
// Get the model pose (global frame)
GzPose Model::GetPose()
{
  assert(this->GetBody());
  return this->GetBody()->GetPose();
}


//////////////////////////////////////////////////////////////////////////////
// Set the pose of the model (global cs)
void Model::SetPose( GzPose pose )
{
  int i;
  Body *body;
  GzPose opose, npose, mpose;
 
  // TODO: Only set pose of the new pose is different

  // Get the current pose of the model
  opose = this->GetPose();

  assert(GzPoseFinite(opose));

  // Compute the new pose of the model
  npose = pose;

  for (i = 0; i < this->bodyCount; i++)
  {
    body = this->bodies[i];

    // Get the current pose of this body
    mpose = body->GetPose();
          
    // Compute the pose relative to the model
    mpose = GzCoordPoseSub( mpose, opose );

    // Compute the new pose
    mpose = GzCoordPoseAdd( mpose, npose );
    assert(GzPoseFinite(mpose));
        
    // Set the new position and rotation
    body->SetPose( mpose );
   
    // Reset the object velocity
    body->SetLinearVel(GzVectorSet(0, 0, 0));
    body->SetAngularVel(GzVectorSet(0, 0, 0));
  }

  ///////////////////////////////////////////////////////////////////
  // Atribui o valor da propriedade com meta-dados de posicao/orientacao
  // usada pelo IRCE
  ///////////////////////////////////////////////////////////////////
  // A posicao é atribuida diretamente
  ((GzVectorField*)this->GetField("xyz"))->SetValue(pose.pos);
  // Quaternios é transformado em Roll, Pitch, Yaw em radianos
  GzVector euler = GzQuaternToEuler(pose.rot);
  // Roll, pitch e yaw em radianos sao transformados em angulos
  euler.x = 180.0 / PI_NUMERIC * euler.x;
  euler.y = 180.0 / PI_NUMERIC * euler.y;
  euler.z = 180.0 / PI_NUMERIC * euler.z;
  // Roll, pitch e yaw em angulos sao atribuidos a propriedade rpy
  ((GzVectorField*)this->GetField("rpy"))->SetValue(euler);
  ///////////////////////////////////////////////////////////////////
  

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Log the current pose of the canonical body
void Model::LogPose()
{
  GzPose pose;
  double dist = 1.0;
  assert(this->canonicalBody);

  // Re-size array as needed
  if (this->poseCount >= this->poseMaxCount)
  {
    this->poseMaxCount += 50;
    this->poses = (GzPose*) realloc( this->poses, this->poseMaxCount * sizeof(this->poses[0]));
    assert( this->poses );
  }

  if (this->poseCount>0)
  {
    pose = this->canonicalBody->GetCoMPose();
    dist = sqrt( pow(this->poses[this->poseCount-1].pos.x-pose.pos.x,2) +
        pow(this->poses[this->poseCount-1].pos.y-pose.pos.y,2) + 
        pow(this->poses[this->poseCount-1].pos.z-pose.pos.z,2));

  }

  if (dist >= 1.0)
  {
    this->poses[this->poseCount] = pose;
    this->poseCount++;
  }
}


//////////////////////////////////////////////////////////////////////////////
// Set the pick id of the model
void Model::SetPickId( GLuint id )
{
  this->pickId = id;

  for (int i=0; i<this->bodyCount; i++)
  {
    this->bodies[i]->SetPickId(id);
  }

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the Pick id of the model
GLuint Model::GetPickId() const
{
  return this->pickId;
}


//////////////////////////////////////////////////////////////////////////////
// Return the integer ID of this model
int Model::GetIntId() const
{
  return this->integerId;
}

//////////////////////////////////////////////////////////////////////////////
// Return the integer ID of this model's parent
int Model::GetParentIntId() const
{
  return this->parent == NULL ? 0: this->parent->GetIntId();
}


//////////////////////////////////////////////////////////////////////////////
// Render the model
void Model::Render( int pass, RenderOptions *renderOpt )
{
  int i;

  
  // Posiciona um nome 
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  GzPose pose = this->GetBody()->GetPose();
  GzVector pos = pose.pos;
  glTranslatef(pos.x, pos.y, pos.z);
  GzQuatern rot = GzQuaternToAxis(pose.rot);
  glRotatef(rot.u * 180 / M_PI, rot.x, rot.y, rot.z);
  /////////glPushName(this->pickId);
  glLoadName(this->pickId);
  glPopMatrix();


  for (i = 0; i < this->bodyCount; i++)
  {
   this->bodies[i]->Render(pass, renderOpt);
  }

  if (this->logPoses && this->poseCount > 0 && pass==GZ_RENDER_MAX_PASSES-1)
  {
    GLUquadricObj *quad = gluNewQuadric();
    gluQuadricOrientation(quad, GLU_OUTSIDE);
    gluQuadricDrawStyle(quad, GLU_LINE);


    glPushMatrix();
    glTranslatef(this->poses[0].pos.x,this->poses[0].pos.y,this->poses[0].pos.z);
    gluSphere(quad, 0.05, 3, 3);
    for (i=1; i< this->poseCount; i++)
    {
      glTranslatef(this->poses[i].pos.x-this->poses[i-1].pos.x, 
          this->poses[i].pos.y-this->poses[i-1].pos.y, 
          this->poses[i].pos.z-this->poses[i-1].pos.z);
      gluSphere(quad, 0.05, 5, 5);
    }

    glPopMatrix();
    
    glBegin(GL_LINE_STRIP);
    for (i=0; i< this->poseCount; i++)
    {
      glVertex3f(this->poses[i].pos.x, this->poses[i].pos.y, this->poses[i].pos.z);
    }
    glVertex3f(this->canonicalBody->GetPose().pos.x,this->canonicalBody->GetPose().pos.y,this->canonicalBody->GetPose().pos.z);

    glEnd();
    gluDeleteQuadric(quad);
    
  }
}

 