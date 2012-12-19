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
 * CVS: $Id: SphereGeom.cc,v 1.15 2004/12/07 00:15:06 natepak Exp $
 */

#include "GL/glu.h"
#include "Image.hh"
#include "SphereGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
SphereGeom::SphereGeom( Body *body, dSpaceID spaceId, double radius )
    : Geom( spaceId )
{
  dMass mass;

  // Initialize box mass matrix
  dMassSetSphere(&mass, 1, radius);

  // Create the sphere geometry
  this->SetGeom( body, dCreateSphere( 0, radius ), &mass, true );
  this->radius = radius;
    
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
SphereGeom::~SphereGeom()
{
  return;
}



//////////////////////////////////////////////////////////////////////////////
// Default render routine
void SphereGeom::Render(RenderOptions *opt)
{
  GLUquadricObj *quad;
  GLuint listId;
  RenderOptions listOpt;

  // Recover stored display list for this camera
  this->GetList(opt->cameraIndex, &listId, &listOpt);

  // See if the current display list is dirty
  this->dirty |= (listId == 0);
  this->dirty |= (opt->displayMaterials != listOpt.displayMaterials);
  this->dirty |= (opt->displayTextures != listOpt.displayTextures);

  // Generate the display list
  if (dirty)
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

      // Flip so textures get draw the right way up
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glScalef(1, -1, 0);
    }
  
    // Create the sphere
    quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);  
    gluQuadricOrientation(quad, GLU_OUTSIDE);
    gluSphere(quad, this->radius, 32, 32);
    gluDeleteQuadric(quad);

    if (opt->displayTextures && this->textureImage)
    {
      // Restore texture matrix
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glDisable(GL_TEXTURE_2D);
    }

    // Store list options
    this->SetList(opt->cameraIndex, listId, *opt);      
    glEndList();
  }

  // Call the display list
  if (listId)
    glCallList(listId);

  return;
}

void SphereGeom::SetSize(double radius)
{
  this->radius = radius;
  this->dirty = true;
}