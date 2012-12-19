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
 * CVS: $Id: Body.hh,v 1.31 2006/02/14 03:00:25 natepak Exp $
 */

#ifndef BODY_HH
#define BODY_HH

#include <ode/ode.h>
#include "Vector.hh"

#include "GL/gl.h"

// Forward declarations
class World;
class Geom;
class RenderOptions;


/** @brief Class encapsulating a physical body.
 */
class Body
{
  /// @brief Constructor
  public: Body( const World *world, const char *bodyName = NULL, bool dummy = false );

  /// @brief Destructor
  public: virtual ~Body();
  
  // Attach a geometry to this body
  private: void AttachGeom( Geom *geom );

  // Detach a geometry from this body
  private: void DetachGeom( Geom *geom );

  // Update the CoM and mass matrix
  public: void UpdateCoM();

  // Get the list of geometries
  public: int GetNumGeoms() {return this->geomCount;}
  public: Geom **GetGeoms() {return this->geoms;}

  // Enable/disable the body (so it doesnt fall, for example)
  public: void Enable(bool enable);

  /// @brief Get the body pose (global cs)
  public: GzPose GetPose( void ) const;

  /// @brief Get the body position (global cs)
  public: GzVector GetPosition( void ) const;

  /// @brief Get the body rotation (global cs)
  public: GzQuatern GetRotation( void ) const;
  
  /// @brief Set the body pose (global cs)
  public: void SetPose( GzPose pose );

  /// @brief Set the body position (global cs)  
  public: void SetPosition( GzVector pos );

  /// @brief Set the body rotation (global cs)
  public: void SetRotation( GzQuatern rot );

  // Get the CoM body pose (global cs)
  public: GzPose GetCoMPose( void ) const;

  // Get the body linear velocity (global cs)
  public: GzVector GetLinearVel( void ) const;

  // Get the body angular velocity (global cs)
  public: GzQuatern GetAngularVel( void ) const;

  /// @brief Set the linear velocity (global cs)
  public: void SetLinearVel( GzVector vel );

  /// @brief Set the angular velocity (global cs)
  public: void SetAngularVel( GzVector vel );

  // Get the body id
  public: dBodyID GetBodyId() const;
  
  // Get the default space id
  public: dSpaceID GetSpaceId() const;

  /// @brief Enable/disable effect of gravity on this body
  public: void SetGravityMode(bool enabled);
  
  public: int GetGravityMode();
  
  // To correct the wheel rotation issues in ODE
  public: void SetFiniteRotationAxis(double x,double y,double z);
  public: void SetFiniteRotationMode(int mode);

  // Set pick id of all the geoms in this body
  public: void SetPickId( GLuint id );

  // TODO? // Force/Torque Setting Functions
  public: GzVector GetTorque(void);

  /// @brief Get the tota force acting on a body
  public: GzVector GetForce(void);

  /// @brief Set the total force acting on a body
  public: void SetForce( GzVector f );
  
  public: void SetTorque( double x, double y, double z);

  public: void AddForce( double x, double y, double z);
  public: void AddTorque( double x, double y, double z);
  public: void AddRelForce( double x, double y, double z);
  public: void AddRelTorque( double x, double y, double z);
   
  public: void AddForceAtPos( double x, double y, double z, 
                              double pos_x,double pos_y,double pos_z); 
  public: void AddForceAtRelPos( double x, double y, double z,
                                 double pos_x,double pos_y,double pos_z);

  // Draw this body
  public: void Render( int pass, RenderOptions *renderOpt );

  // Render a set of axes on a body
  private: void RenderAxes(RenderOptions *renderOpt);

  // Draw an axis with the given pose and size
  private: void DrawAxis(GzPose pose, double size, const char *text);
 
  // A name to reference us by
  public: char *bodyName;
  
  // ODE world we belong to
  protected: dWorldID worldId;

  // ODE body handle
  protected: dBodyID bodyId;

  // Pose of CoM relative to body
  private: GzPose comPose;

  // Body pose (for dummy bodies)
  protected: GzPose dummyPose;

  // List of geometries attach to this body
  private: int geomCount, geomMaxCount;
  private: Geom **geoms;

  // Mass properties of the object
  protected: dMass mass;

  // Warts
  friend class Geom;
  friend class BaseGeom;
  friend class Camera;
};



#endif

