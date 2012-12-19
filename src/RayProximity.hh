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
 * CVS: $Id: RayProximity.hh,v 1.11 2005/11/22 16:44:51 natepak Exp $
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

#ifndef RAYPROXIMITYSENSOR_HH
#define RAYPROXIMITYSENSOR_HH

#include "Sensor.hh"
#include "Body.hh"

class RayGeom;


/// @brief Proximity sensor with one or more rays.
///
/// This sensor cast rays into the world, tests for intersections, and
/// reports the range to the nearest object.  It is used by ranging
/// sensor models (e.g., sonars and scanning laser range finders).
class RayProximity: public Sensor
{
  /// @brief Constructor
  /// @param world The world in which to place the sensor
  /// @param body The underlying collision test uses an ODE geom, so
  /// ray sensors must be attached to a body.
  /// @param rayCount Maximum number of rays in sensor.
  public: RayProximity(World *world, Body *body, int rayCount);

  
  /// @brief Destructor
  public: virtual ~RayProximity();

  /// @brief Set ray parameters
  /// @param index Ray index (from 0 to rayCount - 1).
  /// @param a, b Ray endpoints (initial and final points).  These are
  /// in body-relative coordiantes.
  public: void SetRay(int index, GzVector a, GzVector b);

  /// @breif Get ray parameters
  /// @param index Ray index (from 0 to rayCount -1).
  /// @param pos, dir Ray position and direction.
  public: void GetRay(int index, GzVector *pos, GzVector *dir);

  /// @brief Get detected range for a ray.
  /// @returns Returns DBL_MAX for no detection.
  public: double GetRange(int index);   

  /// @brief Get detected retro (intensity) value for a ray.
  public: double GetRetro(int index);   

  /// @brief Get detected fiducial value for a ray.
  public: int GetFiducial(int index);   

  /// @brief Update sensed values
  public: void Update();
  
  /// @brief Ray-intersection callback
  private: static void UpdateCallback( void *data, dGeomID o1, dGeomID o2 );
  
  /// The body we are attached to
  private: Body *body;
  
  /// Ray space for collision detector
  private: dSpaceID superSpaceId;
  private: dSpaceID raySpaceId; 

  /// Ray data
  private: int rayCount;
  private: RayGeom **rays;
};

#endif
