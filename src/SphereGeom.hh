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
/* Desc: Sphere geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: SphereGeom.hh,v 1.9 2004/10/29 22:40:01 inspectorg Exp $
 */

#ifndef SPHEREGEOM_HH
#define SPHEREGEOM_HH

#include "Geom.hh"

class SphereGeom : public Geom
{
  // Constructor
  public: SphereGeom( Body *body, dSpaceID spaceId, double radius );

  // Destructor
  public: virtual ~SphereGeom();
  
  // Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  /// Sphere dimensions
  private: double radius;

  public: void SetSize(double radius);
};

#endif
