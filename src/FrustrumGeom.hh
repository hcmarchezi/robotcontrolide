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
/* Desc: Frustrum geometry
 * Author: Andrew Howard
 * Date: 22 Oct 2004
 * CVS: $Id: FrustrumGeom.hh,v 1.3 2004/11/02 07:41:45 inspectorg Exp $
 */

#ifndef FRUSTRUMGEOM_HH
#define FRUSTRUMGEOM_HH

#include "Geom.hh"


/// @brief Geom for camera frustrums.
///
/// The frustrum geom is used mainly for debugging purposes, to show a
/// camera field of view.  It will also project the camera image back
/// into the world, so that, for example, a GUI camera can see what is
/// visible to a regular camera sensor.
class FrustrumGeom : public Geom
{
  /// @brief Constructor
  /// @param body Body to which we are attached.
  /// @param spaceId Collision space to which we belong.
  /// @param aspect Image aspect ratio (w/h).
  /// @param nearClip, farClip Distance to the near and far clipping planes (m).
  public: FrustrumGeom(Body *body, dSpaceID spaceId,
                       double aspect, double nearClip, double farClip);

  /// @brief Destructor
  public: virtual ~FrustrumGeom();

  /// @brief Set the horizontal field of view
  public: void SetFOV(double fov);

  /// @brief Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  /// Dimensions of the frustrum
  private: double hfov, aspect, nearClip, farClip;

  /// Dimensions of the image
  private: double width, height;
};

#endif
