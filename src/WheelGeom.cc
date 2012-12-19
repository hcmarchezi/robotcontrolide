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

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "WheelGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
WheelGeom::WheelGeom( Body *body, dSpaceID dSpaceId, double totalRadius, double tireRadius )
  : Geom( dSpaceId )
{
  dMass mass;

  // Initialize mass matrix
  dMassSetSphere(&mass, 1, totalRadius);

  // Create the sphere geometry
  this->SetGeom( body, dCreateSphere( 0, totalRadius ), &mass, true );

  this->totalRadius = totalRadius;
  this->tireRadius = tireRadius;

  return;
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
WheelGeom::~WheelGeom()
{
}


//////////////////////////////////////////////////////////////////////////////
// Default render routine
void WheelGeom::Render(RenderOptions *opt)
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

    // Set material properties
    if (opt->displayMaterials)
    {
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, this->colorAmbient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, this->colorDiffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, this->colorSpecular);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, this->shininess);
    }

    // Create the tire
    glutSolidTorus(this->tireRadius, this->totalRadius - this->tireRadius, 16, 16);

    // Create the hub
    glTranslatef(0, 0, +this->tireRadius * 0.8);
    quad = gluNewQuadric();
    gluQuadricOrientation(quad, GLU_OUTSIDE);
    gluDisk(quad, 0, this->totalRadius * 0.8, 32, 4);
    gluDeleteQuadric(quad);

    glTranslatef(0, 0, -this->tireRadius * 1.6);
    quad = gluNewQuadric();
    gluQuadricOrientation(quad, GLU_INSIDE);
    gluDisk(quad, 0, this->totalRadius * 0.8, 32, 4);
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
