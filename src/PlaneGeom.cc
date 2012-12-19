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
/* Desc: Infinite plane geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: PlaneGeom.cc,v 1.28 2006/02/14 03:00:25 natepak Exp $
 *
 * Notes: This is not a placeable geometry, so position and
 * orientation are undefined.
 */

#include <math.h>
#include <GL/glu.h>

#include "Error.hh"
#include "Image.hh"
#include "PlaneGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
PlaneGeom::PlaneGeom(Body *body, dSpaceID spaceId, double altitude, GzVector normal)
    : Geom(spaceId)
{
  // Note the assumption that this is a z-plane (more-or-less)
  this->planeAltitude = altitude;

  // Set the z unit vector (plane normal
  this->planeNz = GzVectorUnit(normal);

  // Compute x unit vector (orthogonal to Nz and y)
  this->planeNx = GzVectorCross(GzVectorSet(0, 1.0, 0), this->planeNz);
  this->planeNx = GzVectorUnit(this->planeNx);

  // Compute y unit vector (orthogonal to other two vectors)
  this->planeNy = GzVectorCross(this->planeNz, this->planeNx);

  // Size of each renderable planar patch
  this->patchSize = 2.0;

  // Create an ODE plane geom
  // This geom is not placable
  this->SetGeom(body, dCreatePlane(spaceId, normal.x, normal.y, normal.z, altitude),
                NULL, false);
    
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
PlaneGeom::~PlaneGeom()
{
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Default render routine
void PlaneGeom::Render(RenderOptions *opt)
{
  int i, j, k;
  int ni, nj;
  double sx, sy;
  GzVector p, q, offset;
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
      PRINT_GL_ERR();
    }

    if (opt->displayTextures && this->textureImage)
    {
      // Set up textures
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      PRINT_GL_ERR();

      // Build the mipmaps
      // Assume image data is *not* aligned
      glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
      gluBuild2DMipmaps(GL_TEXTURE_2D, this->textureImage->components,
                        this->textureImage->width, this->textureImage->height,
                        this->textureImage->format, this->textureImage->type,
                        this->textureImage->data);
      PRINT_GL_ERR();
    }


    if (opt->displayTextures && this->textureImage)
      glEnable(GL_TEXTURE_2D);

    glNormal3f(this->planeNz.x, this->planeNz.y, this->planeNz.z);

    // Compute number of patches
    ni = (int) (ceil(opt->farClip / this->patchSize));
    nj = (int) (ceil(opt->farClip / this->patchSize));

    // Render strips
    for (j = -nj; j < nj; j += 1)
    {
      glBegin(GL_TRIANGLE_STRIP);
      
      for (i = -ni; i < ni; i += 2)
      {
        double vertex[4][2] = {{-1, +1}, {-1, -1}, {+1, +1}, {+1, -1}};
        offset = GzVectorSet(i * this->patchSize, j * this->patchSize, 0.0);
          
        for (k = 0; k < 4; k++)
        {
          q = GzVectorSet(vertex[k][0] * this->patchSize, vertex[k][1] * this->patchSize, 0.0);
          q = GzVectorMul(0.5, q);
          q = GzVectorAdd(q, offset);
                                
          p = GzVectorMul(q.x, this->planeNx);
          p = GzVectorAdd(p, GzVectorMul(q.y, this->planeNy));

          // Texture is scaled so that it covers the right amount of the plane
          glTexCoord2f(q.x  / this->textureSize.x, q.y / this->textureSize.y);
          
          glVertex3f(p.x, p.y, p.z);
        }
      }
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
  {
    // Compute offset based on camera pose; we only draw the plane around the camera
    sx = Max(this->patchSize, this->textureSize.x);
    sy = Max(this->patchSize, this->textureSize.y);
    ni = (int) floor(opt->cameraPose.pos.x / sx + 0.5);
    nj = (int) floor(opt->cameraPose.pos.y / sy + 0.5);
    offset = GzVectorSet((ni + 0.5) * sx, (nj + 0.5) * sy, 0);
    glTranslatef(offset.x, offset.y, offset.z);

    glCallList(listId);
    PRINT_GL_ERR();
  }

  return;
}
