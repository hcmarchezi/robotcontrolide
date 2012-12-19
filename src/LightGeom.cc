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
/* Desc: Light geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: LightGeom.cc,v 1.12 2006/04/05 14:51:57 natepak Exp $
 */

#include <GL/glut.h>
#include "LightGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Static vars
int LightGeom::lightNum = 0;


//////////////////////////////////////////////////////////////////////////////
// See if there are any lights available
bool LightGeom::CheckAvailable()
{
  // TODO: fix
  /*
  GLint m;
  glGetIntegerv(GL_MAX_LIGHTS, &m);
  printf("max lights %d\n", m);
  return (lightNum < max);
  */
  return true;
}


//////////////////////////////////////////////////////////////////////////////
// Constructor
LightGeom::LightGeom( Body *body, dSpaceID spaceId )
    : Geom( spaceId )
{
  // Get a light index
  this->light = GL_LIGHT0 + lightNum++;

  // This geom needs to be rendered before normal geoms
  this->renderOrder = GZ_RENDER_LIGHT;

  this->atten[0] = 1.0;
  this->atten[1] = 0.0;
  this->atten[2] = 0.0;

  // This geom is not placable
  this->SetGeom(body, NULL, NULL, false);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
LightGeom::~LightGeom()
{
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the attenuation factors
void LightGeom::SetAttenuation(GzVector atten)
{
  this->atten[0] = atten.x;
  this->atten[1] = atten.y;
  this->atten[2] = atten.z;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Default pre-render routine
void LightGeom::PreRender(RenderOptions *opt)
{
  GzVector pos;
  GLfloat npos[4];
  
  pos = this->GetPose().pos;
  
  // Make a point light source
  npos[0] = pos.x;
  npos[1] = pos.y;
  npos[2] = pos.z;
  npos[3] = 1.0;
  glLightfv(this->light, GL_POSITION, npos);
  
//   // Set attenuation
  glLightfv(this->light, GL_CONSTANT_ATTENUATION, this->atten + 0);
  glLightfv(this->light, GL_LINEAR_ATTENUATION, this->atten + 1);
  glLightfv(this->light, GL_QUADRATIC_ATTENUATION, this->atten + 2);
//   
//   // Set colors
  glLightfv(this->light, GL_AMBIENT, this->colorAmbient);
  glLightfv(this->light, GL_DIFFUSE, this->colorDiffuse);
  glLightfv(this->light, GL_SPECULAR, this->colorSpecular);

  glEnable(this->light);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Default post-render routine
void LightGeom::PostRender(RenderOptions *opt)
{
  // Dont delete; we need to override the default PostRender() so that
  // it does nothing
  return;
}
