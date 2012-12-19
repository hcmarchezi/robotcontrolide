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
/* Desc: Base class for all bodies
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: Body.cc,v 1.37 2006/02/14 03:00:25 natepak Exp $
 */

#include <assert.h>
#include <ode/ode.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Geom.hh"
#include "Body.hh"
#include "World.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
Body::Body( const World *world, const char *bodyName, bool dummy )
{
  if (bodyName)
    this->bodyName = strdup(bodyName);
  else
    this->bodyName = NULL;
  this->worldId = world->worldId;

  this->comPose = GzPoseSet(GzVectorZero(), GzQuaternIdent());
  this->dummyPose = GzPoseSet(GzVectorZero(), GzQuaternIdent());
  
  if (!dummy)
  {    
    this->bodyId = dBodyCreate( world->worldId );
    dBodyEnable(this->bodyId);
  }
  else
  {
    this->bodyId = NULL;
  }

  // List geoms associated with this body
  this->geomCount = 0;
  this->geomMaxCount = 0;
  this->geoms = NULL;

  // Initialize the mass matrix
  dMassSetZero( &this->mass );

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
Body::~Body()
{
  free(this->geoms);
  if (this->bodyId)
    dBodyDestroy( this->bodyId );
  if (this->bodyName)
    free(this->bodyName);
 
  return;
}

//////////////////////////////////////////////////////////////////////////////
// Attach a geometry to this body
void Body::AttachGeom( Geom *geom )
{
  // Attach the geom to the ODE body
  if (this->bodyId)
  {
    if (geom->placeableGeom)
      dGeomSetBody( geom->GetTransId(), this->bodyId );
  }

  // Expand list if necessary
  if (this->geomCount >= this->geomMaxCount)
  {
    this->geomMaxCount += 10;
    this->geoms = (Geom**) realloc(this->geoms, this->geomMaxCount * sizeof(this->geoms[0]));
    assert( this->geoms );
  }
  
  // Add to the list
  assert(this->geomCount < this->geomMaxCount);
  this->geoms[this->geomCount++] = geom;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Detach a geometry from this body
void Body::DetachGeom( Geom *geom )
{
  // TODO
  assert(0);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Update the CoM and mass matrix
/*
  What's going on here?  In ODE the CoM of a body corresponds to the
  origin of the body-fixed coordinate system.  In Gazebo, however, we
  want to have arbitrary body coordinate systems (i.e., CoM may be
  displaced from the body-fixed cs).  To get around this limitation in
  ODE, we have an extra fudge-factor (comPose), describing the pose of
  the CoM relative to Gazebo's body-fixed cs.  When using low-level
  ODE functions, one must use apply this factor appropriately.

  The UpdateCoM() function is used to compute this offset, based on
  the mass distribution of attached geoms.  This function also shifts
  the ODE-pose of the geoms, to keep everything in the same place in the
  Gazebo cs.  Simple, neh?

  TODO: messes up if you call it twice; should fix.
*/
void Body::UpdateCoM()
{
  int i;
  BaseGeom *geom;
  const dMass *mass;
  GzPose oldPose, newPose, pose;

  // Dummy bodies dont have mass
  if (!this->bodyId)
    return;
  
  // Construct the mass matrix by combining all the geoms
  dMassSetZero( &this->mass );
  for (i = 0; i < this->geomCount; i++)
  {
    geom = this->geoms[i];
    mass = geom->GetBodyMassMatrix();
    if (geom->placeableGeom)
      dMassAdd( &this->mass, mass );
  }

  // Old pose for the CoM
  oldPose = this->comPose;

  // New pose for the CoM
  newPose.pos.x = this->mass.c[0];
  newPose.pos.y = this->mass.c[1];
  newPose.pos.z = this->mass.c[2];
  newPose.rot = GzQuaternIdent();

  // Fixup the poses of the geoms (they are attached to the CoM)
  for (i = 0; i < this->geomCount; i++)
  {
    geom = this->geoms[i];

    if (geom->placeableGeom)
    {
      this->comPose = oldPose;
      pose = geom->GetRelativePose();
      this->comPose = newPose;
      geom->SetRelativePose(pose, false);
    }
  }

  // Fixup the pose of the CoM (ODE body)
  this->comPose = oldPose;
  pose = this->GetPose();
    this->comPose = newPose;
  this->SetPose(pose);
  
  // Settle on the new CoM pose
  this->comPose = newPose;

  // TODO: remove offsets from mass matrix?

  // Set the mass matrix
  dBodySetMass( this->bodyId, &this->mass );

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Enable/disable the body (so it doesnt fall, for example)
void Body::Enable(bool enable)
{
  if (!this->bodyId)
    return;
    
  if (enable)
    dBodyEnable(this->bodyId);
  else
    dBodyDisable(this->bodyId);
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the CoM body pose (global cs)
GzPose Body::GetCoMPose( void ) const
{
  const dReal *p, *r;
  GzPose pose;

  if (!this->bodyId)
    return this->dummyPose;
  
  // Get the CoM pose (global cs)
  p = dBodyGetPosition( this->bodyId );
  r = dBodyGetQuaternion( this->bodyId );
  pose.pos = GzVectorSet(p[0], p[1], p[2]);
  pose.rot = GzQuaternSet(r[0], r[1], r[2], r[3]);

  return pose;
}

//////////////////////////////////////////////////////////////////////////////
// Get the body pose (global cs)
GzPose Body::GetPose( void ) const
{
  const dReal *p, *r;
  GzPose pose;

  if (!this->bodyId)
  {
    pose = this->dummyPose;
    pose.rot = GzQuaternNormal(pose.rot);
    return pose;
  }
  
  // Get the CoM pose (global cs)
  p = dBodyGetPosition( this->bodyId );
  r = dBodyGetQuaternion( this->bodyId );
  pose.pos = GzVectorSet(p[0], p[1], p[2]);
  pose.rot = GzQuaternSet(r[0], r[1], r[2], r[3]);

  // Compute the body pose
  pose = GzCoordPoseSolve(this->comPose, pose);
  
  return pose;
}


//////////////////////////////////////////////////////////////////////////////
// Set the body pose (global cs)
void Body::SetPose( GzPose pose )
{
  dQuaternion q;
      
  if (!this->bodyId)
  {
    pose.rot = GzQuaternNormal(pose.rot);
    this->dummyPose = pose;
    return;
  }
  
  // Compute pose of CoM
  pose = GzCoordPoseAdd(this->comPose, pose);

  // Set the CoM pose
  dBodySetPosition( this->bodyId, pose.pos.x, pose.pos.y, pose.pos.z );
  q[0] = pose.rot.u;
  q[1] = pose.rot.x;
  q[2] = pose.rot.y;
  q[3] = pose.rot.z;
  dBodySetQuaternion( this->bodyId, q);
  
  return;
}

//////////////////////////////////////////////////////////////////////////////
// Get the bodies global position
GzVector Body::GetPosition( void ) const
{
  return this->GetPose().pos;
}


//////////////////////////////////////////////////////////////////////////////
// Get the body rotation (global cs)
GzQuatern Body::GetRotation( void ) const
{
  return this->GetPose().rot;
}




//////////////////////////////////////////////////////////////////////////////
// Set the body position (global cs)
void Body::SetPosition( GzVector pos )
{
  GzPose pose;
  
  pose = this->GetPose();
  pose.pos = pos;
  this->SetPose(pose);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the body rotation (global cs)
void Body::SetRotation( GzQuatern rot )
{
  GzPose pose;
  
  pose = this->GetPose();
  pose.rot = rot;
  this->SetPose(pose);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the body linear velocity (global cs)
GzVector Body::GetLinearVel( void ) const
{
  if (!this->bodyId)
  {
    return GzVectorZero();
  }
  else
  {
    const dReal *v = dBodyGetLinearVel( this->bodyId );
    return GzVectorSet( (double)(v[0]), (double)(v[1]), (double)(v[2]) );
  }
}


//////////////////////////////////////////////////////////////////////////////
// Get the body angular velocity (global cs)
GzQuatern Body::GetAngularVel( void ) const
{
  if (!this->bodyId)
  {
    return GzQuaternIdent();
  }
  else
  {
    const dReal *v;
    GzQuatern vel;

    // Get angular velocity in global frame
    v = dBodyGetAngularVel( this->bodyId );

    //printf("v   %6.2f %6.2f %6.2f %6.2f\n", v[0], v[1], v[2], v[3]);
  
    // Compute angular velocity quaternion in the body frame
    vel = GzQuaternMul(GzQuaternSet(0, (double)(v[0])/2, (double)(v[1])/2, 
                                    (double)(v[2])/2), this->GetRotation());

    // TODO: confused
    // Compute angular velocity quaternion in the global frame
    //vel = GzQuaternMul(GzQuaternInverse(this->GetRotation()),
    //GzQuaternMul(vel, this->GetRotation()));

    return vel;
  }
}


//////////////////////////////////////////////////////////////////////////////
// Set the linear velocity (global cs)
void Body::SetLinearVel( GzVector vel ) 
{
  if (this->bodyId)
    dBodySetLinearVel( this->bodyId, vel.x, vel.y, vel.z );
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the angular velocity (global cs)
void Body::SetAngularVel( GzVector vel ) 
{
  if (this->bodyId)
    dBodySetAngularVel( this->bodyId, vel.x, vel.y, vel.z );
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the id of the body
dBodyID Body::GetBodyId() const
{
  return this->bodyId;
}


//////////////////////////////////////////////////////////////////////////////
// Apply a force to the body
void Body::SetForce( GzVector f )
{
  if (this->bodyId)
    dBodySetForce( this->bodyId, (dReal)f.x, (dReal)f.y ,(dReal)f.z );
}


//////////////////////////////////////////////////////////////////////////////
// Apply a torque to the body
void Body::SetTorque( double x, double y, double z )
{
  if (this->bodyId)
    dBodySetTorque( this->bodyId, (dReal)x, (dReal)y, (dReal)z);
}


//////////////////////////////////////////////////////////////////////////////
// Get body force
GzVector Body::GetForce(void)
{
  if (!this->bodyId)
  {
    return GzVectorZero();
  }
  else
  {
    GzVector gvec;
    const double* vec = (double*)dBodyGetForce( this->bodyId);
    gvec.x=vec[0];
    gvec.y=vec[1];
    gvec.z=vec[2];
    return gvec;
  }
}


//////////////////////////////////////////////////////////////////////////////
// Get body torque
GzVector Body::GetTorque(void)
{ 
  const dReal *vec = dBodyGetTorque( this->bodyId);

  return GzVectorSet((double)vec[0],(double)vec[1],(double)vec[2]);
}


//////////////////////////////////////////////////////////////////////////////
// Add a force to the body
void Body::AddForce( double x, double y, double z )
{
  if (this->bodyId)
    dBodyAddForce( this->bodyId, (dReal)x, (dReal)y , (dReal)z );
}


//////////////////////////////////////////////////////////////////////////////
// Add a torque to the body
void Body::AddTorque( double x, double y, double z )
{
  if (this->bodyId)
    dBodyAddTorque( this->bodyId, (dReal)x, (dReal)y, (dReal)z);
}


//////////////////////////////////////////////////////////////////////////////
// Add a relative force to the body
void Body::AddRelForce( double x, double y, double z )
{
  if (this->bodyId)
    dBodyAddRelForce( this->bodyId, (dReal)x, (dReal)y, (dReal)z );
}


//////////////////////////////////////////////////////////////////////////////
// Add a relative torque to the body
void Body::AddRelTorque( double x, double y, double z )
{
  if (this->bodyId)
    dBodyAddRelTorque( this->bodyId, (dReal)x, (dReal)y, (dReal)z);
}


//////////////////////////////////////////////////////////////////////////////
// Apply a force to the body at position
void Body::AddForceAtPos( double x, double y, double z,double pos_x,double pos_y,double pos_z )
{
  if (this->bodyId)
    dBodyAddForceAtPos( this->bodyId, (dReal)x, (dReal)y, (dReal)z,
                        (dReal)pos_x, (dReal)pos_y, (dReal)pos_z );
}


//////////////////////////////////////////////////////////////////////////////
// Apply a force to the body at relative position 
void Body::AddForceAtRelPos( double x, double y, double z,double pos_x,double pos_y,double pos_z )
{
  if (this->bodyId)
    dBodyAddForceAtRelPos( this->bodyId, (dReal)x, (dReal)y, (dReal)z,
                           (dReal)pos_x, (dReal)pos_y, (dReal)pos_z);
}


//////////////////////////////////////////////////////////////////////////////
// Non-zero mode == affected by gravity, zero mode == not affected by
// gravity
void Body::SetGravityMode(bool enable)
{
  if (this->bodyId)
    dBodySetGravityMode(this->bodyId,(int) enable);
}


//////////////////////////////////////////////////////////////////////////////
// Return the gravity mode. See SetGravityMode
int Body::GetGravityMode()
{
  if (!this->bodyId)
    return 0;
  else
    return dBodyGetGravityMode(this->bodyId);
}


//////////////////////////////////////////////////////////////////////////////
// Wrappers for the body Finite Rotation Axis functions
void Body::SetFiniteRotationAxis(double x, double y, double z)
{
  if (this->bodyId)
    dBodySetFiniteRotationAxis(this->bodyId, (dReal)x, (dReal)y, (dReal)z);
} 


//////////////////////////////////////////////////////////////////////////////
// Mode Description:
// 0: An ``infitesimal'' orientation update is used. This is fast to compute, 
//    but it can occasionally cause inaccuracies for bodies that are rotating 
//    at high speed, especially when those bodies are joined to other bodies. 
//    This is the default for every new body that is created.
//
// 1: A ``finite'' orientation update is used. This is more costly to compute, 
//    but will be more accurate for high speed rotations. Note however that 
//    high speed rotations can result in many types of error in a simulation, 
//    and this mode will only fix one of those sources of error. 
//
void Body::SetFiniteRotationMode(int mode)
{
  if (this->bodyId)
    dBodySetFiniteRotationMode(this->bodyId,mode);
}


//////////////////////////////////////////////////////////////////////////////
// Set the pick id.
void Body::SetPickId( GLuint id )
{
  for (int i=0; i<this->geomCount; i++)
  {
    this->geoms[i]->SetPickId( id );
  }

  return;
}

void Body::Render( int pass, RenderOptions *renderOpt )
{
  int i;
  Geom *geom;

  if (renderOpt->displayAxes || renderOpt->displayCoM)
    this->RenderAxes(renderOpt);

  for (i = 0; i < this->GetNumGeoms(); i++)
  {
    geom = this->geoms[i];

    if (geom->renderOrder == pass)
    {
      geom->PreRender(renderOpt);

      if (renderOpt->displaySkins)
      {
        if (geom->HasSkin())
          geom->RenderSkin(renderOpt);
        else if (!geom->HasSkinNull())
          geom->Render(renderOpt);
      }
      else
        geom->Render(renderOpt);

      geom->PostRender(renderOpt);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// Render a set of axes on a body (useful for debugging)
void Body::RenderAxes(RenderOptions *renderOpt)
{
  GzPose pose;
  
  // Dont display axes on dummy bodies
  if (this->GetBodyId() == NULL)
    return;

  // Dont display axes on un-named bodies
  if (this->bodyName == NULL)
    return;
  
  // Draw axis for the body cs
  if (renderOpt->displayAxes)
  {
    pose = this->GetPose();
    this->DrawAxis(pose, 1.0, this->bodyName);
  }

  // Draw axis for the CoM cs
  if (renderOpt->displayCoM)
  {
    pose = this->GetCoMPose();
    this->DrawAxis(pose, 0.5, "CoM");
  }
  
  return;
}

//////////////////////////////////////////////////////////////////////////////
// Draw an axis with the given pose and size
void Body::DrawAxis(GzPose pose, double size, const char *text)
{
  int i;
  GLfloat color[4];
  GzQuatern rot;
  
  glPushMatrix();

  glTranslatef(pose.pos.x, pose.pos.y, pose.pos.z);

  rot = GzQuaternToAxis(pose.rot);
  glRotatef(rot.u * 180 / M_PI, rot.x, rot.y, rot.z);

  // Set default material properties
  GZ_COLOR_COPY(color, GzColor(0, 0, 0, 1));  
  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, color);
  glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, color);
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, color);

  // Z axis
  GZ_COLOR_COPY(color, GzColor(0, 0, 1, 1));
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
  glBegin(GL_LINES);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, size);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, size);
  glVertex3f(-0.05, 0, 0.9 * size);
  glVertex3f(+0.05, 0, 0.9 * size);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, size);
  glVertex3f(0, -0.05, 0.9 * size);
  glVertex3f(0, +0.05, 0.9 * size);
  glEnd();

  glRotatef(90, 0, 1, 0);

  // X axis
  GZ_COLOR_COPY(color, GzColor(1, 0, 0, 1));
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
  glBegin(GL_LINES);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, size);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, size);
  glVertex3f(-0.05, 0, 0.9 * size);
  glVertex3f(+0.05, 0, 0.9 * size);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, size);
  glVertex3f(0, -0.05, 0.9 * size);
  glVertex3f(0, +0.05, 0.9 * size);
  glEnd();

  // Display the body name
  glPushMatrix();
  glRotatef(-90, 0, 1, 0);
  glTranslatef(0.8 * size, 0.1, 0.1);
  glRasterPos3f(0, 0, 0);
  for (i = 0; i < (int) strlen(text); i++)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
  glPopMatrix();

  glRotatef(-90, 1, 0, 0);

  // Y axis
  GZ_COLOR_COPY(color, GzColor(0, 1, 0, 1));
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
  glBegin(GL_LINES);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, size);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, size);
  glVertex3f(-0.05, 0, 0.9 * size);
  glVertex3f(+0.05, 0, 0.9 * size);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3f(0, 0, size);
  glVertex3f(0, -0.05, 0.9 * size);
  glVertex3f(0, +0.05, 0.9 * size);
  glEnd();

  // Reset the emission color
  GZ_COLOR_COPY(color, GzColor(0, 0, 0, 1));
  glMaterialfv(GL_FRONT, GL_EMISSION, color);

  glPopMatrix();

  return;
}
