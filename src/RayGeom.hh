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
/* Desc: A ray
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: RayGeom.hh,v 1.17 2004/12/07 00:15:06 natepak Exp $
 */

#ifndef RAYGEOM_HH
#define RAYGEOM_HH

#include "Geom.hh"


class RayGeom : public Geom
{
  // Constructor
  public: RayGeom( Body *body, const dSpaceID space );

  // Destructor
  public: virtual ~RayGeom();

  // Set object pose (relative to body)
  // This is a NOP for rays (use Set() instead)
  public: virtual void SetRelativePose( GzPose pose, bool updateCoM = true );

  // Get object pose (relative to body)
  // This is returns an identity transform for Rays
  public: virtual GzPose GetRelativePose() const;

  // Set the starting point and direction (relative coordinates)
  public: void Set(GzVector pos, GzVector dir);

  // Get the starting point and direction (relative coordinates)
  public: void Get(GzVector *pos, GzVector *dir);

  // Set the length of the ray
  public: void SetLength( const double len );

  // Get the length of the ray
  public: double GetLength( void ) const;

  // Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  /// The RayProximity sensor stores initial values and results from
  /// collision tests in the RayGeom class; it's ugly but efficient.
  friend class RayProximity;

  /// Ray endpoints, relative to parent body
  private: GzVector pos_a, pos_b;

  /// Contact information; this is filled out during collision
  /// detection.  
  private: double contactDepth;
  private: double contactRetro;
  private: int contactFiducial;
};


#endif
