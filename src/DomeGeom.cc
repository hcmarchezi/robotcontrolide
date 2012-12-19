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
/* Desc: Dome geometry
 * Author: Nate Keonig
 * Date: 24 Nov 2004
 * CVS: $Id: DomeGeom.cc,v 1.6 2004/12/07 00:15:06 natepak Exp $
 *
 * Notes: This is not a placeable geometry, so position and
 * orientation are undefined.
 */

#include <math.h>
#include <sys/time.h>
#include <GL/glu.h>

#include "Error.hh"
#include "Image.hh"
#include "DomeGeom.hh"

// TODO: Use vertex shaders to render sky. Can use half a sphere fixed in
// from of the camera, and each vertex's texture and color attributes change
// as the camera moves. Would reduce number of triangles, but not sure if
// this method is beneficial - especially since it will be incompatible with
// cards that don't support shaders.

//////////////////////////////////////////////////////////////////////////////
// Constructor
DomeGeom::DomeGeom( Body *body, dSpaceID spaceId, int resolution)
  : Geom(spaceId)
{

  this->origin = GzVectorSet(0,0,-.1);
  this->resolution = resolution;
  this->vertices = NULL;
  this->vertexCount = 0;

  this->elapsedTime = 0;
  this->prevElapsedTime = 0;

  // This geom is not placable
  this->SetGeom(body, NULL, NULL, false);

  this->renderOrder = GZ_RENDER_LIGHT;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
DomeGeom::~DomeGeom()
{
  return;
}

void DomeGeom::SetDayTime( double seconds )
{
  this->startTime = seconds;
}

void DomeGeom::SetElapsedTime( double seconds )
{
  this->elapsedTime = seconds;
}

//////////////////////////////////////////////////////////////////////////////
// Default render routine
void DomeGeom::Render(RenderOptions *opt)
{
  int i,j;
  int delta = this->resolution * 4;
  int start, end;
  //int currTime;
  double texS;
  //timeval tv;
  //tm *tmpTime = NULL;
  GLuint listId;
  RenderOptions listOpt;

  if (this->textureImage==NULL)
    return;

  // Recover stored display list for this camera
  this->GetList(opt->cameraIndex, &listId, &listOpt);

  texS = (this->startTime + this->elapsedTime) / 86400.0; 

  // See if the current display list is dirty
  this->dirty |= (listId == 0);
  this->dirty |= (opt->displayMaterials != listOpt.displayMaterials);
  this->dirty |= (opt->displayTextures != listOpt.displayTextures);
  this->dirty |= (this->elapsedTime-this->prevElapsedTime) >= (86400.0 / this->textureImage->width);
    
  // Generate the display list
  if (this->dirty)
  {

    this->prevElapsedTime = this->elapsedTime;

    if (listId == 0)
      listId = glGenLists(1);    
    glNewList(listId, GL_COMPILE);

    // Set material properties
    /*if (opt->displayMaterials)
      {
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, this->colorAmbient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, this->colorDiffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, this->colorSpecular);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, this->shininess);
      PRINT_GL_ERR();
      }*/

    if (opt->displayTextures && this->textureImage)
    {
      // Set up textures
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
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

    // Create the dome geometry, once only.
    // Make the dome slightly smaller than the far clip so that the 
    // dome doesn't get clipped.
    if (this->vertexCount == 0)
      this->CreateMesh(opt->farClip*0.8);

    // Disable writing to the depth buffer
    glDepthMask(GL_FALSE);

    // Draw top of dome as a triangle fan
    glBegin(GL_TRIANGLE_FAN);

    glTexCoord2f(texS,0);
    glVertex3fv(this->vertices);
    for (i = 1; i <= this->resolution * 4; i++)
    {

      glTexCoord2f(texS,1.0/this->resolution);
      glVertex3fv(this->vertices+i*3);
    }

    glTexCoord2f(texS,1.0/this->resolution);
    glVertex3fv(this->vertices+3);
    glEnd();

    // Draw rest of dome as triangle strip
    for (i=1; i<this->resolution; i++)
    {
      start = delta * i + 1; 
      end = start + delta;

      glBegin(GL_TRIANGLE_STRIP);
      for (j=start; j<end; j++)
      {

        glTexCoord2f(texS,i/(float)this->resolution);
        glVertex3fv(this->vertices+(j-delta)*3);

        glTexCoord2f(texS,(i+1)/(float)this->resolution);
        glVertex3fv(this->vertices+j*3);
      }

      glTexCoord2f(texS,(i)/(float)this->resolution);
      glVertex3fv(this->vertices+(start-delta)*3);

      glTexCoord2f(texS,(i+1)/(float)this->resolution);
      glVertex3fv(this->vertices+start*3);
      glEnd();
    }

    // Enable writing to the depth buffer
    glDepthMask(GL_TRUE);

    if (opt->displayTextures && this->textureImage)
      glDisable(GL_TEXTURE_2D);

    // Store list options
    this->SetList(opt->cameraIndex, listId, *opt);      
    glEndList();
  }

  // Call the display list
  if (listId)
  {
    // Keep the dome centered around the camera
    glTranslatef(opt->cameraPose.pos.x, opt->cameraPose.pos.y, 0);

    glCallList(listId);
    PRINT_GL_ERR();
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
// Create the triangle mesh for the sky dome
void DomeGeom::CreateMesh( double radius )
{
  GzVector point;
  GzVector vertex;
  GzHomo homo;
  double vertSweep;
  double horzSweep;
  double radiusAngle;
  double zAdj;
  double vscale = 1.0;
  int numVerts=1;
  int i,j;

  // Compute the number of vertices
  this->vertexCount = 1 + 4 * this->resolution * this->resolution;

  this->vertices = new GLfloat[this->vertexCount * 3];


  vertSweep = M_PI * 0.5; // 90 degree vertical sweep

  // From the resolution, compute the angular sweep of one section of the
  // dome
  horzSweep = (M_PI*0.5) / (float)this->resolution;

  // Adjust the radius based on the vertical sweep
  radiusAngle = (M_PI*0.5) - vertSweep;
  radius /= cos(radiusAngle);

  // Compute the z adjustment
  zAdj = radius * sin(radiusAngle);

  // Adjust the vertical resolution
  vertSweep /= (float)this->resolution;

  // Start the vertex list wit the very top of the dome
  this->vertices[0] = this->origin.x;
  this->vertices[1] = this->origin.y;
  this->vertices[2] = (radius - zAdj) * vscale + this->origin.z;

  // Starting from the top, populate with this->resolution number of rings
  for (i=0; i<this->resolution; i++)
  {
    // Compute the vertex that will be rotated around to make the ring
    homo = GzHomoFromQuatern( GzQuaternFromEuler(0, vertSweep * (i+1),0) );
    point = GzHomoMul( homo, GzVectorSet(0,0,radius));
    point.z = (point.z - zAdj) * vscale;

    for (j=0; j<this->resolution * 4; j++)
    {
      homo = GzHomoFromQuatern( GzQuaternFromEuler(0,0,horzSweep * j) );
      vertex = GzHomoMul(homo, point);
      vertex = GzVectorAdd(vertex, this->origin);

      this->vertices[numVerts*3] = vertex.x ;
      this->vertices[numVerts*3+1] = vertex.y;
      this->vertices[numVerts*3+2] = vertex.z;

      numVerts++;
    }
  }
}
