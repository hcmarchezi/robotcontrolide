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
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: BoxGeom.hh,v 1.11 2004/10/29 22:40:00 inspectorg Exp $
 */

#ifndef BOXGEOM_HH
#define BOXGEOM_HH

#include "Geom.hh"


class BoxGeom : public Geom
{
  // Constructor
  public: BoxGeom(Body *body, dSpaceID spaceId, double xDim, double yDim, double zDim);

  // Destructor
  public: virtual ~BoxGeom();

  // Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  /// Dimensions of the box
  private: double sides[3];

  public: void SetSize(double xDim,double yDim,double zDim);
};

#endif
