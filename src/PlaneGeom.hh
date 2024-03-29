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
/* Desc: Plane geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: PlaneGeom.hh,v 1.11 2004/11/02 07:41:45 inspectorg Exp $
 */

#ifndef PLANEGEOM_HH
#define PLANEGEOM_HH

#include "Geom.hh"


/// @brief Geom for an infinite plane.
///
/// This geom is used primarily for ground planes.  Note that while
/// the plane in infinite, only the part near the camera is drawn.
class PlaneGeom : public Geom
{
  /// @brief Constructor
  /// @param body Body to which we are attached.
  /// @param spaceId Collision space to which we belong.
  /// @param altitude Plane altiude (z value above global 0).
  /// @param normal Normal vector to plane (global cs).
  public: PlaneGeom(Body *body, dSpaceID spaceId, double altitude, GzVector normal);

  /// @brief Destructor
  public: virtual ~PlaneGeom();

  /// @brief Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  /// Plane altiude (z value above global 0).
  private: double planeAltitude;

  /// Plane orientation (x, y, z unit vectors).
  private: GzVector planeNx, planeNy, planeNz;
  
  /// Size of each rendered patch.
  private: double patchSize;
};

#endif

