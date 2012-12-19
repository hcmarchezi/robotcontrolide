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
/* Desc: Dome geometry
 * Author: Nate Keonig
 * Date: 24 Nov 2004
 * CVS: $Id: DomeGeom.hh,v 1.4 2005/05/06 17:18:55 natepak Exp $
 */

#ifndef DOMEGEOM_HH
#define DOMEGEOM_HH

#include <time.h>

#include "Geom.hh"

/// @brief Geom for a dome.
///
/// This geom is used primarily for sky dome.  Note that it assumed nothing
/// will ever collide with this geom
class DomeGeom : public Geom
{
  /// @brief Constructor
  /// @param body Body to which we are attached.
  /// @param spaceId Collision space to which we belong.
  /// @param resolution Resolution of the dome = number of triangles
  public: DomeGeom(Body *body, dSpaceID spaceId, int resolution);

  /// @brief Destructor
  public: virtual ~DomeGeom();

  public: void SetDayTime( double seconds );
  public: void SetElapsedTime( double seconds );

  /// @brief Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  /// @brief Creates the skydome mesh
  private: void CreateMesh( double radius );

  /// @brief Resolution of the sky dome. Higher number creates a denser mesh 
  private: int resolution;

  /// @brief Starting time, in seconds, from the start of a day
  private: double startTime;

  /// @brief Number of elapsed seconds
  private: double elapsedTime;
  private: double prevElapsedTime;

  /// @brief Origin of the skydome
  private: GzVector origin;

  /// @brief Vertices of the dome
  private: GLfloat *vertices;
  private: int vertexCount;

};

#endif

