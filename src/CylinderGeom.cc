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
/* Desc: Cylinder geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: CylinderGeom.cc,v 1.21 2006/11/02 16:54:09 natepak Exp $
 */

#include <GL/gl.h>
#include <GL/glu.h>

#include "Image.hh"
#include "CylinderGeom.hh"
#include "dCylinder.h" // For declarations missing from ODE


//////////////////////////////////////////////////////////////////////////////
// Constructor
CylinderGeom::CylinderGeom( Body *body, dSpaceID spaceId, double radius, double length )
    : Geom( spaceId )
{
  dMass mass;

  // Initialize mass matrix
  dMassSetCylinder( &mass, 1.0, 3, radius, length );
  
  this->SetGeom( body, dCreateCylinder( 0, length, radius ), &mass, true );

  // ODE cylinders are oriented along the y-axis; fix this
  // so the are along the z-axis
  this->SetExtraRotation(GzQuaternFromAxis(1, 0, 0, M_PI / 2));
  this->SetRelativeRotation(GzQuaternIdent());
  
  this->radius = radius;
  this->length = length;

  return;
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
CylinderGeom::~CylinderGeom()
{
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Default render routine
void CylinderGeom::Render(RenderOptions *opt)
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
  if (this->dirty)
  {
    if (listId == 0)
      listId = glGenLists(1);    
    glNewList(listId, GL_COMPILE);

    // Make sure cylinder center is in correct place
    glTranslatef(0, 0, -this->length / 2);

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

    // Create the cylinder
    quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);  
    gluQuadricOrientation(quad, GLU_OUTSIDE);
    gluCylinder(quad, this->radius, this->radius, this->length, 32, 1);
    gluDeleteQuadric(quad);

    if (opt->displayTextures && this->textureImage)
    {
      // Restore texture matrix
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glDisable(GL_TEXTURE_2D);
    }

    quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_FALSE);  
    gluQuadricOrientation(quad, GLU_INSIDE);
    gluDisk(quad, 0, this->radius, 32, 4);
    gluDeleteQuadric(quad);

    glTranslatef(0, 0, this->length);
  
    quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_FALSE);  
    gluQuadricOrientation(quad, GLU_OUTSIDE);
    gluDisk(quad, 0, this->radius, 32, 4);
    gluDeleteQuadric(quad);

    // Store list options
    this->SetList(opt->cameraIndex, listId, *opt);      
    glEndList();
  }
  
  // Call the display list
  if (listId)
    glCallList(listId);

  return;
}

void CylinderGeom::SetSize(double radius,double length)
{
  this->radius = radius;
  this->length = length;
  this->dirty = true;
}
