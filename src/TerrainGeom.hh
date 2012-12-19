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
/* Desc: Terrain geometry
 * Author: Nate Keonig
 * Date: 02 Sept 2003
 * CVS: $Id: TerrainGeom.hh,v 1.14 2004/11/10 08:21:44 inspectorg Exp $
 */

#ifndef TERRAINGEOM_HH
#define TERRAINGEOM_HH

#include <ode/ode.h>
#include "Global.hh"
#include "Geom.hh"

class TerrainGeom : public Geom
{
  // Constructor
  public: TerrainGeom( Body *body, dSpaceID spaceId );

  // Destructor
  public: virtual ~TerrainGeom();

  // Load the terrain from the height data
  public: void Load( GLfloat *packedVertices, unsigned int vertexCount, 
                       GLuint *indices, unsigned int indexCount,
                       GLuint *odeIndices, unsigned int odeIndexCount);

  // Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  public: void RenderODE();

  // OpenGL data of the terrain
  private: GLfloat *vertices;
  private: unsigned int vertexCount;
  private: GLuint *indices;
  private: unsigned int indexCount;

  // ODE Collision detection attributes
  private: dTriMeshDataID odeData;
  private: GLuint *odeIndices;
  private: unsigned int odeIndexCount;
};

#endif
