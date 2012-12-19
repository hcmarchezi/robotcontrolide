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
 * CVS: $Id: TerrainGeom.cc,v 1.26 2004/12/07 00:15:06 natepak Exp $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_TRIMESH

#include <GL/glut.h>
#include <assert.h>

#include "Error.hh"
#include "TerrainGeom.hh"
#include "Image.hh"

//////////////////////////////////////////////////////////////////////////////
// Constructor
TerrainGeom::TerrainGeom( Body *body, dSpaceID spaceId )
  : Geom(spaceId)
{
  // Store the body so that we can create the TriMesh later on
  this->body = body;

  // This will hold the vertex data of the triangle mesh
  this->odeData = dGeomTriMeshDataCreate();

  // This geom is not placeable
  this->placeableGeom = false;

  // the arrays are initially empty
  this->vertices = NULL;
  this->vertexCount = 0;
  this->indices = NULL;
  this->indexCount = 0;
  this->odeIndices = NULL;
  this->odeIndexCount = 0;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
TerrainGeom::~TerrainGeom()
{
}


//////////////////////////////////////////////////////////////////////////////
// Load the terrain from the given data
void TerrainGeom::Load(GLfloat *packedVertices, unsigned int vertexCount, 
                       GLuint *indices, unsigned int indexCount,
                       GLuint *odeIndices, unsigned int odeIndexCount)
{
  this->vertices = packedVertices;
  this->vertexCount = vertexCount;
  this->indices = indices;
  this->indexCount = indexCount;

  this->odeIndices = odeIndices;
  this->odeIndexCount = odeIndexCount;

  // Build the ODE triangle mesh for collision detection
  dGeomTriMeshDataBuildSingle(this->odeData,
      this->vertices+9, 12*sizeof(GLfloat), this->vertexCount,
      this->odeIndices,this->odeIndexCount, 3*sizeof(int));
      

  this->SetGeom(this->body, dCreateTriMesh(0, this->odeData,0,0,0),NULL,1);
}

//////////////////////////////////////////////////////////////////////////////
// Render the whole terrain geom
void TerrainGeom::Render(RenderOptions *opt)
{
  GLuint listId;
  RenderOptions listOpt;

  // Recover stored display list for this camera
  this->GetList(opt->cameraIndex, &listId, &listOpt);

  // See if the current display list is dirty
  this->dirty |= (listId == 0);
  this->dirty |= (opt->displayMaterials != listOpt.displayMaterials);
  this->dirty |= (opt->displayTextures != listOpt.displayTextures);

  // Generate the display list
  if (this->dirty)
  {
    if (listId == 0)
      listId = glGenLists(1);    
    glNewList(listId, GL_COMPILE);

    // Set material properties
    if (opt->displayMaterials)
    {
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, this->colorAmbient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, this->colorDiffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, this->colorSpecular);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, this->shininess);
    }

    if (opt->displayTextures && this->textureImage)
    {
      // Set up textures
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

      // Build the mipmaps
      // Assume image data is *not* aligned
      glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
      gluBuild2DMipmaps(GL_TEXTURE_2D, this->textureImage->components,
                        this->textureImage->width, this->textureImage->height,
                        this->textureImage->format, this->textureImage->type,
                        this->textureImage->data);

      glEnable(GL_TEXTURE_2D);
    }

    // Enable a tightly packed interleaved vertex array
    glInterleavedArrays( GL_T2F_C4F_N3F_V3F, 0, this->vertices );

    glDrawElements( GL_TRIANGLE_STRIP, this->indexCount , 
                    GL_UNSIGNED_INT, this->indices );

    //this->RenderODE();

    if (opt->displayTextures && this->textureImage)
      glDisable(GL_TEXTURE_2D);

    // Store list options
    this->SetList(opt->cameraIndex, listId, *opt);      
    glEndList();
  }

  // Call the display list
  if (listId)
    glCallList(listId);


}

//////////////////////////////////////////////////////////////////////////////
// For debugging
void TerrainGeom::RenderODE()
{
  int i, index;

  glBegin(GL_TRIANGLES);
  for (i=0; i < (int) this->odeIndexCount; i++)
  {
    index = this->odeIndices[i]*12+9;
    glVertex3f( this->vertices[index], this->vertices[index+1], this->vertices[index+2]);
  }
  glEnd();
}

#endif
