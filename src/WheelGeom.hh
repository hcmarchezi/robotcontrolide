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
/* Desc: A wheel geom - draws as a cylinder but behaives as a sphere (for
 * collsion with meshes)
 * Author: Nate Keonig, Andrew Howard
 * Date: 9 Feb 2004
 * CVS: $Id: WheelGeom.hh,v 1.4 2004/08/16 07:29:03 inspectorg Exp $
 */

#ifndef WHEELGEOM_HH
#define WHEELGEOM_HH

#include "Geom.hh"


class WheelGeom : public Geom
{
  public: WheelGeom(Body *body, dSpaceID spaceId, double totalRadius, double tireRadius);
  public: virtual ~WheelGeom();
  
  // Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  // Dimensions
  private: double totalRadius, tireRadius;
};


#endif
