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
 * CVS: $Id: BoxGeom.cc,v 1.20 2004/12/07 00:15:06 natepak Exp $
 */

#include <GL/glut.h>
#include "Image.hh"
#include "BoxGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
BoxGeom::BoxGeom( Body *body, dSpaceID spaceId, double xDim, double yDim, double zDim )
    : Geom( spaceId )
{
  dMass mass;

  this->sides[0] = xDim;
  this->sides[1] = yDim;
  this->sides[2] = zDim;

  // Initialize box mass matrix
  dMassSetBox( &mass, 1, xDim, yDim, zDim );
  
  // Create a box geometry with box mass matrix
  this->SetGeom( body, dCreateBox( 0, xDim, yDim, zDim ), &mass, true );
    
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
BoxGeom::~BoxGeom()
{
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Default render routine
void BoxGeom::Render(RenderOptions *opt)
{
  int i;
    
  // Normals
  GLfloat n[6][3] =
    {  
      {0, 0, -1}, {0, 0, +1},
      {0, -1, 0}, {0, +1, 0},
      {-1, 0, 0}, {+1, 0, 0}
    };


  // Vertices
  GLint faces[6][4] =
    {
      {3, 2, 1, 0}, {4, 5, 6, 7},
      {0, 1, 5, 4}, {2, 3, 7, 6},
      {3, 0, 4, 7}, {1, 2, 6, 5}
    };


  // Vertex values
  GLfloat v[8][3] =
    {
      {-1, -1, -1}, {+1, -1, -1}, {+1, +1, -1}, {-1, +1, -1},
      {-1, -1, +1}, {+1, -1, +1}, {+1, +1, +1}, {-1, +1, +1}
    };

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

    // Compute the vertices
    for (i = 0; i < 8; i++)
    {
      v[i][0] *= this->sides[0] / 2;
      v[i][1] *= this->sides[1] / 2;
      v[i][2] *= this->sides[2] / 2;
    }

    // Draw the box
    for (i = 0; i < 6; i++)
    {
      glBegin(GL_QUADS);
      glNormal3fv(n[i]);

      glTexCoord2f(0, +1);
      glVertex3fv(&v[faces[i][0]][0]);
    
      glTexCoord2f(+1, +1);
      glVertex3fv(&v[faces[i][1]][0]);

      glTexCoord2f(+1, 0);
      glVertex3fv(&v[faces[i][2]][0]);

      glTexCoord2f(0, 0);
      glVertex3fv(&v[faces[i][3]][0]);

      glEnd();
    }

    if (opt->displayTextures && this->textureImage)
      glDisable(GL_TEXTURE_2D);

    // Store list options
    this->SetList(opt->cameraIndex, listId, *opt);      
    glEndList();
  }

  // Call the display list
  if (listId)
    glCallList(listId);

  return;
}

void BoxGeom::SetSize(double xDim,double yDim,double zDim)
{
  this->sides[0] = xDim;
  this->sides[1] = yDim;
  this->sides[2] = zDim;
  this->dirty = true;
}
