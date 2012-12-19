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
/* Desc: Ray proximity sensor
 * Author: Carle Cote (Laborius - Universite de Sherbrooke - Sherbrooke, Quebec, Canada)
 * Date: 23 february 2004
 * CVS: $Id: RayProximity.cc,v 1.15 2005/11/22 16:44:51 natepak Exp $
 *
 * Modification description :
 *
 * Author: Carle Cote 
 *           (Laborius - Universite de Sherbrooke - Sherbrooke, Quebec, Canada)
 * Date: 26 may 2004 
 * Desc: Fix model with refactored core
 *          - Change collision detection mechanism
 *          - Change rays to have their own spaceID 
 */

#include <assert.h>
#include <float.h>

#include "RayGeom.hh"
#include "World.hh"
#include "WorldFile.hh"

#include "RayProximity.hh"

#include "Vector.hh"
#include "Error.hh"

//////////////////////////////////////////////////////////////////////////////
// Constructor
RayProximity::RayProximity( World *world, Body *body, int rayCount)
    : Sensor( world )
{
  int i;
  
  this->world = world;
  this->body = body;
  
  // Create a space to contain the ray space
  this->superSpaceId = dSimpleSpaceCreate( 0 );
    
  // Create a space to contain all the rays
  this->raySpaceId = dSimpleSpaceCreate( this->superSpaceId );
  
  // Set collision bits
  dGeomSetCategoryBits((dGeomID) this->raySpaceId, GZ_LASER_COLLIDE);
  dGeomSetCollideBits((dGeomID) this->raySpaceId, ~GZ_LASER_COLLIDE);

  // Create and array of ray geoms
  this->rayCount = rayCount;
  this->rays = new RayGeom*[this->rayCount];
  for (i = 0; i < this->rayCount; i++)
    this->rays[i] = new RayGeom(this->body, this->raySpaceId);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
RayProximity::~RayProximity()
{
}


//////////////////////////////////////////////////////////////////////////////
// Set ray endpoints
void RayProximity::SetRay(int index, GzVector a, GzVector b)
{
  RayGeom *ray;

  assert(index >= 0 && index < this->rayCount);
  ray = this->rays[index];
  
  ray->SetColor( GzColor(0, 0, 1) );
  ray->SetCategoryBits( GZ_LASER_COLLIDE );
  ray->SetCollideBits( ~GZ_LASER_COLLIDE );
  ray->SetLength(GzVectorMag(GzVectorSub(b, a)));
  ray->pos_a = a;
  ray->pos_b = b;

  return;
}

//////////////////////////////////////////////////////////////////////////////
// Return a pointer to the ray geom
void RayProximity::GetRay(int index, GzVector *pos, GzVector *dir)
{
  RayGeom *ray;

  assert(index >= 0 && index < this->rayCount);
  ray = this->rays[index];

  ray->Get(pos, dir);
}


//////////////////////////////////////////////////////////////////////////////
// Get detected range for a ray
double RayProximity::GetRange(int index)
{
  RayGeom *ray;

  assert(index >= 0 && index < this->rayCount);
  ray = this->rays[index];

  return ray->contactDepth;
}


//////////////////////////////////////////////////////////////////////////////
// Get detected retro (intensity) value for a ray.
double RayProximity::GetRetro(int index)
{
  RayGeom *ray;

  assert(index >= 0 && index < this->rayCount);
  ray = this->rays[index];

  return ray->contactRetro;
}


//////////////////////////////////////////////////////////////////////////////
// Get detected fiducial value for a ray.
int RayProximity::GetFiducial(int index)
{
  RayGeom *ray;

  assert(index >= 0 && index < this->rayCount);
  ray = this->rays[index];

  return ray->contactFiducial;
}

//////////////////////////////////////////////////////////////////////////////
// Update the sensor information
void RayProximity::Update()
{
  int i;
  GzPose pose;
  RayGeom *ray;
  GzVector a, b;

  // Get the pose of the sensor body (global cs)
  pose = this->body->GetPose();
  
  // Reset the ray lengths and mark the geoms as dirty (so they get
  // redrawn)
  for (i = 0; i < this->rayCount; i++)
  {
    ray = this->rays[i];
    ray->dirty = true;
    ray->contactDepth = DBL_MAX;
    ray->contactRetro = 0.0;
    ray->contactFiducial = -1;

    // Update the ray endpoints (global cs)
    a = GzCoordPositionAdd(ray->pos_a, pose.pos, pose.rot);
    b = GzCoordPositionAdd(ray->pos_b, pose.pos, pose.rot);    
    ray->Set(a, GzVectorUnit(GzVectorSub(b, a)));
  }

  // Do collision detection
  dSpaceCollide2( ( dGeomID ) ( this->superSpaceId ),
                  ( dGeomID ) ( this->world->GetSpaceId() ),
                  this, &UpdateCallback );
  
  return;
}



/////////////////////////////////////////////////////////////////////////////
// Callback for ray intersection test
void RayProximity::UpdateCallback( void *data, dGeomID o1, dGeomID o2 )
{
  int n;
  dContactGeom contact;
  dxGeom *geom1, *geom2;
  RayGeom *rayGeom;
  Geom *hitGeom;
  RayProximity *self;

  self = (RayProximity*) data;
 

  // Check space
  if ( dGeomIsSpace( o1 ) || dGeomIsSpace( o2 ) )
  {
    if (dGeomGetSpace(o1) == self->superSpaceId || dGeomGetSpace(o2) == self->superSpaceId)
    {
      dSpaceCollide2( o1, o2, self, &UpdateCallback );
    }
    if (dGeomGetSpace(o1) == self->raySpaceId || dGeomGetSpace(o2) == self->raySpaceId)
    {
      dSpaceCollide2( o1, o2, self, &UpdateCallback );
    }
  }
  else
  {
    geom1 = NULL;
    geom2 = NULL;
    
    // Get pointers to the underlying geoms
    if (dGeomGetClass(o1) == dGeomTransformClass)
      geom1 = (dxGeom*) dGeomGetData(dGeomTransformGetGeom(o1));
    else
      geom1 = (dxGeom*) dGeomGetData(o1);
    
    if (dGeomGetClass(o2) == dGeomTransformClass)
      geom2 = (dxGeom*) dGeomGetData(dGeomTransformGetGeom(o2));
    else
      geom2 = (dxGeom*) dGeomGetData(o2);

    assert(geom1 && geom2);

    rayGeom = NULL;
    hitGeom = NULL;

    // Figure out which one is a ray; note that this assumes
    // that the ODE dRayClass is used *soley* by the RayGeom.    
    if (dGeomGetClass(o1) == dRayClass)
    {
      rayGeom = (RayGeom*) geom1;
      hitGeom = (Geom*) geom2;
      dGeomRaySetParams(o1, 0, 0);
      dGeomRaySetClosestHit(o1, 1);
    }    

    if (dGeomGetClass(o2) == dRayClass)
    {
      assert(rayGeom == NULL);
      rayGeom = (RayGeom*) geom2;
      hitGeom = (Geom* )geom1;
      dGeomRaySetParams(o2, 0, 0);
      dGeomRaySetClosestHit(o2, 1);
    }
        
    // Check for ray/geom intersections
    if ( rayGeom && hitGeom )
    {
      n = dCollide(o1, o2, 1, &contact, sizeof(contact));       

      if ( n > 0 )
      {       
        if (contact.depth < rayGeom->contactDepth)
        {
          GzVector start;
          GzVector dir;
          rayGeom->Get(&start, &dir);
          rayGeom->contactDepth = contact.depth;
          rayGeom->contactRetro = hitGeom->GetRetro();
          rayGeom->contactFiducial = hitGeom->GetFiducial();
        }
      }
    }
  }
  return;
}
