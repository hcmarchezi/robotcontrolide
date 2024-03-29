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
/* Desc: Box geometry
 * Author: Andrew Howard
 * Date: 9 Mar 2003
 * CVS: $Id: PointSetGeom.hh,v 1.4 2004/09/13 04:26:16 inspectorg Exp $
 */

#ifndef POINTSETGEOM_HH
#define POINTSETGEOM_HH

#include "Geom.hh"
#include "Vector.hh"


class PointSetGeom : public Geom
{
  // Constructor
  public: PointSetGeom( Body *body, dSpaceID spaceId, int pointCount, GzVector *points );

  // Destructor
  public: virtual ~PointSetGeom();

  // Default render routine
  public: virtual void Render(RenderOptions *opt);

  // The point set
  private: int pointCount;
  private: GzVector *points;
};

#endif
