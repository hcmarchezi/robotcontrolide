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
/* Desc: Geometry for rendering point clouds
 * Author: Andrew Howard
 * Date: 9 Mar 2004
 * CVS: $Id: PointSetGeom.cc,v 1.6 2004/11/04 23:55:13 inspectorg Exp $
 */


#include "PointSetGeom.hh"

//////////////////////////////////////////////////////////////////////////////
// Constructor
PointSetGeom::PointSetGeom( Body *body, dSpaceID spaceId, int pointCount, GzVector *points )
    : Geom( spaceId )
{
  // Create a dummy, nearly massless ODE geom
  dMass mass;
  dMassSetBox( &mass, 0.001, 0.001, 0.001, 0.001);
  this->SetGeom(body, dCreateBox(0, 0, 0, 0), &mass, false);

  // TESTING
  this->SetAmbientColor(GzColor(0.5, 0.5, 0.5));

  // Copy the points
  this->pointCount = pointCount;
  this->points = new GzVector[this->pointCount];
  memcpy(this->points, points, pointCount * sizeof(this->points[0]));
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
PointSetGeom::~PointSetGeom()
{
  if (this->points)
    delete [] this->points;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Default render routine
void PointSetGeom::Render(RenderOptions *opt)
{
  int i, j, k;
  GzVector *point;
  int maxPoints;

  // Write points in batches, since the OS X GL drivers segfault if
  // there are too may points in one object
  
  maxPoints = 1000000;
  
  for (i = 0; i < this->pointCount / maxPoints + 1; i++)
  {
    glBegin(GL_POINTS);
    for (j = 0; j < maxPoints; j++)
    {
      k = i * maxPoints + j;
      if (k >= this->pointCount)
        continue;
      point = this->points + k;
      glVertex3f(point->x, point->y, point->z);
    }
    glEnd();
  }
  
  return;
}

