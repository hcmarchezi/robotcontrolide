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
/* Desc: Base class for all geometries
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: BaseGeom.cc,v 1.15 2004/12/13 22:58:26 inspectorg Exp $
 */

#include <assert.h>

#include "Body.hh"
#include "BaseGeom.hh"
#include "ContactParams.hh"

// Forward declaration for ugly BaseGeom/Geom interdependency
class Geom;


//////////////////////////////////////////////////////////////////////////////
// Constructor
BaseGeom::BaseGeom( dSpaceID spaceId )
{
  this->body = NULL;
  this->spaceId = spaceId;
  this->transId = NULL;
  this->geomId = NULL;

  // Most geoms are placable
  this->placeableGeom = true;
  this->dummyPose = GzPoseSet(GzVectorZero(), GzQuaternIdent());

  // Most geoms dont need extra rotations
  this->extraRotation = GzQuaternIdent();
  
  // Out default mass is zero
  dMassSetZero( &this->geomMass );
  dMassSetZero( &this->bodyMass );

  // Create the contact parameters
  this->contact = new ContactParams();
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
BaseGeom::~BaseGeom()
{
  if (this->geomId)
    dGeomDestroy(this->geomId);
  
  if (this->transId)
    dGeomDestroy(this->transId);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the encapsulated geometry object
void BaseGeom::SetGeom( Body *body, dGeomID geomId, dMass *mass, bool placeable )
{
  assert(!this->geomId);

  this->geomId = geomId;
  this->transId = NULL;

  this->placeableGeom = placeable;

  if (this->placeableGeom)
  {
    this->transId = dCreateGeomTransform( this->spaceId );
    dGeomTransformSetGeom( this->transId, this->geomId );
    dGeomTransformSetInfo( this->transId, 1 );
  }

  if (mass)
    this->geomMass = *mass;

  if (this->placeableGeom)
  {
    // Placeable; the encapsulated geom must not belong to any space
    assert(dGeomGetSpace(this->geomId) == 0);
    dGeomSetData(this->geomId, this);
  }
  else if (this->geomId)
  {
    // Non-placeable; the encapsulated geom must belong to some space
    assert(dGeomGetSpace(this->geomId) != 0);
    dGeomSetData(this->geomId, this);
  }

  this->body = body;
  this->body->AttachGeom( (Geom*) this );
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the extra rotation of the encapsulated object (called in constructor)
void BaseGeom::SetExtraRotation( GzQuatern rot )
{
  this->extraRotation = rot;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the transform geometry
dGeomID BaseGeom::GetTransId() const
{
  return this->transId;
}


//////////////////////////////////////////////////////////////////////////////
// Get the geometry
dGeomID BaseGeom::GetGeomId() const
{
  return this->geomId;
}


//////////////////////////////////////////////////////////////////////////////
// Set the geometry category bitfield
void BaseGeom::SetCategoryBits( uint bits )
{
  if (this->transId)
    dGeomSetCategoryBits( this->transId, bits );
  dGeomSetCategoryBits( this->geomId, bits );
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the geometry collision bitfield
void BaseGeom::SetCollideBits( uint bits )
{
  if (this->transId)
    dGeomSetCollideBits( this->transId, bits );
  dGeomSetCollideBits( this->geomId, bits );
  return;
}


//////////////////////////////////////////////////////////////////////////////
/// @brief Set spring-damper parameters for contact joints
void BaseGeom::SetHardness( double kp, double kd )
{
  this->contact->kp = kp;
  this->contact->kd = kd;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set friction coefficients
void BaseGeom::SetFriction( double mu1, double mu2 )
{
  this->contact->mu1 = mu1;
  this->contact->mu2 = mu2;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set bounciness
void BaseGeom::SetBounce( double bounce )
{
  this->contact->bounce = bounce;
}


//////////////////////////////////////////////////////////////////////////////
// Set direction 1 slip
void BaseGeom::SetSlip1( double slip )
{
  this->contact->slip1 = slip;
}


//////////////////////////////////////////////////////////////////////////////
// Set direction 2 slip
void BaseGeom::SetSlip2( double slip )
{
  this->contact->slip2 = slip;
}


//////////////////////////////////////////////////////////////////////////////
// Get bounce parameter
double BaseGeom::GetBounce() const
{
  return this->contact->bounce;
}

//////////////////////////////////////////////////////////////////////////////
// Get direction 1 slip
double BaseGeom::GetSlip1() const
{
  return this->contact->slip1;
}


//////////////////////////////////////////////////////////////////////////////
// Get direction 2 slip
double BaseGeom::GetSlip2() const
{
  return this->contact->slip2;
}


//////////////////////////////////////////////////////////////////////////////
// Set the pose of the geom, relative to the body
void BaseGeom::SetRelativePose(GzPose pose, bool updateCoM)
{
  dQuaternion q;

  // Must be attached before we can set the pose
  assert(this->body);
  
  if (!this->placeableGeom)
  {
    this->dummyPose = pose;
    return;
  }
  
  // Transform into CoM relative pose
  pose = GzCoordPoseSub(pose, this->body->comPose);

  // Hack: we want cylinders aligned along the z-axis
  pose.rot = GzQuaternMul(pose.rot, this->extraRotation);

  q[0] = pose.rot.u;
  q[1] = pose.rot.x;
  q[2] = pose.rot.y;
  q[3] = pose.rot.z;

  // Set the pose of the encapsulated geom; this is always relative
  // to the CoM
  dGeomSetPosition( this->geomId, pose.pos.x, pose.pos.y, pose.pos.z );
  dGeomSetQuaternion( this->geomId, q);

  // Update the bodies CoM
  if (updateCoM)
    this->body->UpdateCoM();
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the pose relative to the body
GzPose BaseGeom::GetRelativePose() const
{
  const dReal *p;
  dQuaternion q;
  GzPose pose;

  // Must be attached before we can get the pose
  assert(this->body);

  if (!this->placeableGeom)
    return this->dummyPose;

  // Get the pose of the encapsulated geom; this is always relative to
  // the CoM
  p = dGeomGetPosition(this->geomId);
  dGeomGetQuaternion(this->geomId, q);
  pose = GzPoseSet(GzVectorSet(p[0], p[1], p[2]),
                   GzQuaternSet(q[0], q[1], q[2], q[3]));

  // Hack: we want cylinders aligned along the z-axis
  pose.rot = GzQuaternMul(pose.rot, GzQuaternInverse(this->extraRotation));

  // Transform into body relative pose
  pose = GzCoordPoseAdd(pose, this->body->comPose);

  return pose;
}


//////////////////////////////////////////////////////////////////////////////
// Get the pose (global cs)
GzPose BaseGeom::GetPose() const
{
  const dReal *p;
  dQuaternion q;
  GzPose a, b, pose;

  // For non-placeable geoms, just offset from parent body
  if (!this->placeableGeom)
  {
    a = this->GetRelativePose();
    assert(this->body);
    return GzCoordPoseAdd(a, this->body->GetPose());
  }
  
  // Get the relative pose and subtract out the body com offset.  Note
  // that we use GetRelativePose(), rather than calling ODE directly,
  // since the former may be overloaded in subclasses (e.g. RayGeom).
  a = this->GetRelativePose();

  // Offset with CoM
  a = GzCoordPoseSub(a, this->body->comPose);
    
  // Get the pose of the transform geom (global cs)
  assert(this->transId);
  p = dGeomGetPosition(this->transId);
  dGeomGetQuaternion(this->transId, q);  
  b.pos = GzVectorSet(p[0], p[1], p[2]);
  b.rot = GzQuaternSet(q[0], q[1], q[2], q[3]);

  // Compute global pose
  pose = GzCoordPoseAdd(a, b);
  
  return pose;
}


//////////////////////////////////////////////////////////////////////////////
// Set the position (body-centered cs)
void BaseGeom::SetRelativePosition( GzVector pos )
{
  GzPose pose;

  pose = this->GetRelativePose();
  pose.pos = pos;  
  this->SetRelativePose(pose);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the rotation (body-centered cs)
void BaseGeom::SetRelativeRotation( GzQuatern rot )
{
  GzPose pose;

  pose = this->GetRelativePose();
  pose.rot = rot;  
  this->SetRelativePose(pose);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the mass of the object.  
void BaseGeom::SetMass( double m )
{
  if (m < 1e-16)
    return;

  // Update the ODE mass
  dMassAdjust( &this->geomMass, m );

  // Update the bodies CoM
  this->body->UpdateCoM();
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the mass matrix (body-centered coords)
const dMass *BaseGeom::GetBodyMassMatrix()
{
  GzPose pose;
  dQuaternion q;
  dMatrix3 r;

  if (!this->placeableGeom)
    return NULL;

  // Get the pose relative to the body
  pose = this->GetRelativePose();

  // Construct the rotation matrix
  q[0] = pose.rot.u;
  q[1] = pose.rot.x;
  q[2] = pose.rot.y;
  q[3] = pose.rot.z;    
  dQtoR(q, r);

  // Compute the transformed mass matrix
  this->bodyMass = this->geomMass;
  dMassRotate( &this->bodyMass, r );
  dMassTranslate( &this->bodyMass, pose.pos.x, pose.pos.y, pose.pos.z );

  return &this->bodyMass;
}


