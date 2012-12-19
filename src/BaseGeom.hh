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
 * CVS: $Id: BaseGeom.hh,v 1.13 2005/05/06 17:18:55 natepak Exp $
 */

#ifndef BASEGEOM_HH
#define BASEGEOM_HH

#include <ode/ode.h>
#include "Vector.hh"

class Body;
class ContactParams;


/// @brief Base class for GL-renderable geoms
///
/// BaseGeom and Geom are the base classes for all physical geometric
/// objects; BaseGeom encapsulates the mass and collision
/// characteritics, while Geom encapsulated the visual appearance.
class BaseGeom
{
  // Constructor
  public: BaseGeom( dSpaceID spaceId );

  // Destructor
  public: virtual ~BaseGeom();

  /// @brief Set the encapsulated geometry object
  public: void SetGeom( Body *body, dGeomID geomId, dMass *mass, bool placeable);

  // Set the extra rotation of the encapsulated object (called in constructor)
  protected: void SetExtraRotation( GzQuatern rot );
  
  // Get the geometry id
  public: dGeomID GetTransId() const;
  public: dGeomID GetGeomId() const;

  // Set the geometry category bitfield
  public: void SetCategoryBits( uint bits );

  // Set the geometry collision bitfield
  public: void SetCollideBits( uint bits );
  
  /// @brief Set object pose (relative to body)
  public: virtual void SetRelativePose( GzPose pose, bool updateCoM = true );

  /// @brief Get object pose (relative to body)
  public: virtual GzPose GetRelativePose() const;
  
  /// @brief Get object pose (global coord)
  public: GzPose GetPose() const;

  /// @brief Set the relative position (without changing the rotation)
  public: void SetRelativePosition( GzVector pos );

  /// @brief Set the relative rotation (without changing the position)
  public: void SetRelativeRotation( GzQuatern rot );

  /// @brief Set surface hardness parameters for contact joints
  /// @param kp Elastic spring constant
  /// @param kd Damping constant
  public: void SetHardness( double kp, double kd );

  /// @brief Set friction coefficients
  /// @param mu1 Coefficient in direction 1
  /// @param mu2 Coefficient in direction 2
  public: void SetFriction( double mu1, double mu2 );
  
  // Set Contact params
  public: void SetBounce( double bounce );
  public: void SetSlip1( double slip );
  public: void SetSlip2( double slip );

  // Get Contact params
  public: double GetBounce() const;
  public: double GetSlip1() const;
  public: double GetSlip2() const;

  // Set the mass of the object
  public: void SetMass( double m );

  // Get the mass matrix (body-centered coords)
  public: const dMass *GetBodyMassMatrix();

  // The body we are attached to
  public: Body *body;

  // ODE space this geom belongs go
  protected: dSpaceID spaceId;

  // Id for the geometry and the transform
  protected: dGeomID transId, geomId;

  // Mass matrix relative to geometry and relative to body
  protected: dMass geomMass, bodyMass;

  // Is this geom placable?
  public: bool placeableGeom;

  // Global pose if the geom is not placeable
  private: GzPose dummyPose;

  // Hacky extra rotation to make ODE and Gazebo primitives have
  // matching orientations
  private: GzQuatern extraRotation;

  // Contact joint parameters
  protected: ContactParams *contact;

  // Wart
  friend class World;
};

#endif

