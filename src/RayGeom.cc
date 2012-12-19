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
/* Desc: A ray
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: RayGeom.cc,v 1.26 2006/02/03 02:53:03 natepak Exp $
 */

#include <assert.h>
#include <float.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Error.hh"
#include "RayGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
RayGeom::RayGeom( Body *body, const dSpaceID space )
  : Geom( space )
{
  // Create default ray with unit length
  this->SetGeom( body, dCreateRay( space, 1.0 ), NULL, false );

  this->dirty = true;
  this->contactDepth = DBL_MAX;
  this->contactRetro = 0.0;
  this->contactFiducial = -1;

  this->SetEmissionColor( GzColor( 0, 0, 1.0, 1.0 ) );
    
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
RayGeom::~RayGeom()
{
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set object pose (relative to body)
void RayGeom::SetRelativePose( GzPose pose, bool updateCoM )
{
  PRINT_WARN("function has no effect; use Ray::Set() instead");
  assert(false);
    
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get object pose (relative to body)
GzPose RayGeom::GetRelativePose() const
{
  GzPose pose;
  pose = GzPoseSet(GzVectorZero(), GzQuaternIdent());
  return pose;
}


//////////////////////////////////////////////////////////////////////////////
// Set the starting point and direction
void RayGeom::Set(GzVector pos, GzVector dir)
{
  dGeomRaySet(this->geomId, pos.x, pos.y, pos.z, dir.x, dir.y, dir.z);
  this->dirty = true;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the starting point and direction
void RayGeom::Get(GzVector *pos, GzVector *dir)
{
  dVector3 p, d;  
  dGeomRayGet(this->geomId, p, d);

  *pos = GzVectorSet(p[0], p[1], p[2]);
  *dir = GzVectorSet(d[0], d[1], d[2]);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the length of the ray
void RayGeom::SetLength( const double len )
{
  dGeomRaySetLength( this->geomId, len );
  this->dirty = true;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the length of the ray
double RayGeom::GetLength() const
{
  return dGeomRayGetLength( this->geomId );
}



//////////////////////////////////////////////////////////////////////////////
// Render the geom (GL)
void RayGeom::Render(RenderOptions *opt)
{
  bool dirty;
  GLuint listId;
  RenderOptions listOpt;

  // Recover stored display list for this camera
  this->GetList(opt->cameraIndex, &listId, &listOpt);

  // See if the current display list is dirty
  dirty = this->dirty;
  dirty |= (listId == 0);
  dirty |= (opt->displayMaterials != listOpt.displayMaterials);
  dirty |= (opt->displayRays != listOpt.displayRays);

  this->dirty = false;

  // Generate the display list
//  if (dirty)
//  {
 /*   if (listId == 0)
      listId = glGenLists(1);    
    glNewList(listId, GL_COMPILE);
    */

    // Set material properties
    if (opt->displayMaterials)
    {
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, this->colorAmbient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, this->colorDiffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, this->colorSpecular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, this->colorEmission);      
    }
  
    GzVector pos, dir;
    GzVector a, b;
    double length;

    if (opt->displayRays)
    {

      // Use the contact length to display the ray
      length = Min(this->GetLength(), this->contactDepth);
      pos = this->pos_a;
      dir = GzVectorUnit(GzVectorSub(this->pos_b, this->pos_a));

      a = pos;
      b = GzVectorMul(length, dir);
      b = GzVectorAdd(pos, b);
    
      glBegin(GL_LINES);
      glVertex3f(a.x, a.y, a.z);
      glVertex3f(b.x, b.y, b.z);
      glEnd();

      // Draw retro responses
      if (this->contactRetro > 0.0 && false)  // SLOW
      {
        GLUquadricObj *quad;

        // Pick a spot right on the surface
        b = GzVectorMul(length, dir);
        b = GzVectorAdd(pos, b);

        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
                
        // Create the sphere
        quad = gluNewQuadric();
        gluQuadricOrientation(quad, GLU_OUTSIDE);
        gluSphere(quad, 0.02, 8, 8);
        gluDeleteQuadric(quad);

        glPopMatrix();
      }

      // Draw fiducial responses
      if (this->contactFiducial >= 0 && false) // SLOW
      {
        int i;
        char text[128];

        snprintf(text, sizeof(text), "%d", this->contactFiducial);
                
        // Pick a spot slightly away from the surface
        b = GzVectorMul((length - 0.10), dir);
        b = GzVectorAdd(pos, b);

        // Print the text
        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
        glRasterPos3f(0, 0, 0);
        for (i = 0; i < (int) strlen(text); i++)
          glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
        glPopMatrix();
      }
    }

    // Unset material properties (so other geoms dont get emissive)
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, GzColor(0, 0, 0, 1));      

    // Store list options
    //this->SetList(opt->cameraIndex, listId, *opt);      
   // glEndList();
  //}

  // Call the display list
  /*if (listId)
    glCallList(listId);
    */

  return;
}

