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
/* Desc: Frustrum geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: FrustrumGeom.cc,v 1.5 2004/12/06 06:36:43 inspectorg Exp $
 */

#include <GL/glu.h>
#include "Image.hh"
#include "FrustrumGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
FrustrumGeom::FrustrumGeom( Body *body, dSpaceID spaceId, 
                        double aspect, double nearClip, double farClip)
    : Geom( spaceId )
{
  this->aspect = aspect;
  this->hfov = M_PI / 6;
  this->nearClip = nearClip;
  this->farClip = farClip;

  // This geom is not placable
  this->SetGeom(body, NULL, NULL, false);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
FrustrumGeom::~FrustrumGeom()
{
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the horizontal field of view
void FrustrumGeom::SetFOV(double fov)
{
  this->hfov = fov;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Default render routine
void FrustrumGeom::Render(RenderOptions *opt)
{
  int i;
  double w, h;

  // Normalized vertex values
  GLfloat v[4][3] =
    {
      {0, -1, -1}, {0, -1, +1}, {0, +1, +1}, {0, +1, -1},
    };

  GLfloat n[4][3];

  if (!opt->displayFrustrums)
    return;
  
  if (this->textureImage)
  {
    // Set up textures
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Build the mipmaps
    // Assume textureImage data is *not* aligned
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
    gluBuild2DMipmaps(GL_TEXTURE_2D, this->textureImage->components,
                      this->textureImage->width, this->textureImage->height,
                      this->textureImage->format, this->textureImage->type,
                      this->textureImage->data);

    glEnable(GL_TEXTURE_2D);
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, GzColor(1, 1, 1, 1));
  glMaterialfv(GL_FRONT, GL_SPECULAR, GzColor(0, 0, 0, 0));

  w = tan(this->hfov / 2);
  h = tan(this->hfov / 2) / aspect;
  
  // Compute the vertices
  for (i = 0; i < 4; i++)
  {
    n[i][0] = 0.5;
    n[i][1] = v[i][1] * w * 0.5;
    n[i][2] = v[i][2] * h * 0.5;
  }

  // Draw the frustrum
  glBegin(GL_QUADS);

  glTexCoord2f(1, 0);
  glVertex3fv(n[0]);
  
  glTexCoord2f(1, 1);
  glVertex3fv(n[1]);
  
  glTexCoord2f(0, 1);
  glVertex3fv(n[2]);
  
  glTexCoord2f(0, 0);
  glVertex3fv(n[3]);
    
  glEnd();

  if (this->textureImage)
    glDisable(GL_TEXTURE_2D);

  // Compute the vertices
  for (i = 0; i < 4; i++)
  {
    n[i][0] = this->nearClip;
    n[i][1] = v[i][1] * w * this->nearClip;
    n[i][2] = v[i][2] * h * this->nearClip;
  }

  // Draw the near clip plane
  glBegin(GL_LINE_LOOP);
  glVertex3fv(n[0]);
  glVertex3fv(n[1]);
  glVertex3fv(n[2]);
  glVertex3fv(n[3]);    
  glEnd();
  
  // Compute the vertices
  for (i = 0; i < 4; i++)
  {
    n[i][0] = this->farClip;
    n[i][1] = v[i][1] * w * this->farClip;
    n[i][2] = v[i][2] * h * this->farClip;
  }

  // Draw the near clip plane
  glBegin(GL_LINE_LOOP);
  glVertex3fv(n[0]);
  glVertex3fv(n[1]);
  glVertex3fv(n[2]);
  glVertex3fv(n[3]);    
  glEnd();

  // Draw the frustrum
  for (i = 0; i < 4; i++)
  {
    glBegin(GL_LINES);
    n[i][0] = this->nearClip;
    n[i][1] = v[i][1] * w * this->nearClip;
    n[i][2] = v[i][2] * h * this->nearClip;
    glVertex3fv(n[i]);
      
    n[i][0] = this->farClip;
    n[i][1] = v[i][1] * w * this->farClip;
    n[i][2] = v[i][2] * h * this->farClip;
    glVertex3fv(n[i]);
    glEnd();
  }

  return;
}
