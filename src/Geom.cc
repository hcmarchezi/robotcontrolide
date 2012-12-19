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
/* Desc: Base class for all drawables
 * Author: Nate Keonig
 * Date: 04 May 2003
 * CVS: $Id: Geom.cc,v 1.45 2005/03/18 23:19:58 natepak Exp $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <GL/glu.h>

#include "Error.hh"
#include "Vector.hh"
#include "Image.hh"
#include "Body.hh"
#include "Geom.hh"

#if HAVE_LIB3DS
#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/mesh.h>
#include <lib3ds/vector.h>
#include <lib3ds/matrix.h>
#include <lib3ds/material.h>
#endif


//////////////////////////////////////////////////////////////////////////////
// Constructor
Geom::Geom(dSpaceID spaceId)
    : BaseGeom(spaceId)
{
  this->renderOrder = GZ_RENDER_OPAQUE;

  this->transparency = false;
  this->laser = 1;
  this->retro = 0.0;
  this->fiducial = -1;

  // Initial color
  this->SetAmbientColor( GzColor( 0.2, 0.2, 0.2, 1.0 ) );
  this->SetDiffuseColor( GzColor( 0.8, 0.8, 0.8, 1.0 ) );
  this->SetSpecularColor( GzColor( 0.2, 0.2, 0.2, 1.0 ) );
  this->SetEmissionColor( GzColor( 0, 0, 0, 1.0 ) );

  this->textureImage = NULL;
  this->textureSize = GzVectorSet(2.0, 2.0, 2.0);
  
  this->skinFile = NULL;
  this->skinNull = false;
  this->skinScale = GzVectorSet(1, 1, 1);
  this->skinPose = GzPoseSet(GzVectorZero(), GzQuaternIdent());

  memset(this->listList, 0, sizeof(this->listList));

  return;
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
Geom::~Geom()
{
  if (this->textureImage)
    delete this->textureImage;

#ifdef HAVE_LIB3DS
  if (this->skinFile)
    lib3ds_file_free(this->skinFile);
#endif

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set ambient, diffuse, and specular to the same color
void Geom::SetColor( const GzColor &color )
{
  //this->dirty = true;
  this->SetAmbientColor(GzColor(color.r*.2, color.g*.2, color.b*.2));
  this->SetDiffuseColor(color);
  this->SetSpecularColor(GzColor(color.r*1.2, color.g*1.2, color.b*1.2));
  this->dirty = true;
}


//////////////////////////////////////////////////////////////////////////////
// Set the ambient color
void Geom::SetAmbientColor( const GzColor &color )
{
  this->dirty = true;
  GZ_COLOR_COPY(this->colorAmbient, color);
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the diffuse color
void Geom::SetDiffuseColor( const GzColor &color )
{
  this->dirty = true;
  GZ_COLOR_COPY(this->colorDiffuse, color);
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the specular color
void Geom::SetSpecularColor( const GzColor &color )
{
  this->dirty = true;
  GZ_COLOR_COPY(this->colorSpecular, color);
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the emission color
void Geom::SetEmissionColor( const GzColor &color )
{
  this->dirty = true;
  GZ_COLOR_COPY(this->colorEmission, color);
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the 2D texture.
int Geom::SetTexture2D( int width, int height, const uint8_t *data )
{
  this->dirty = true;
  if (this->textureImage != NULL)
    delete this->textureImage;
  this->textureImage = new GzImage;

  if (this->textureImage->Set(width, height, data) != 0)
    return -1;

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Set the 2D texture (accepts NULL for no texture)
int Geom::SetTexture2DFile( const char *filename )
{
  this->dirty = true;
  if (this->textureImage)
  {
    delete this->textureImage;
    this->textureImage = NULL;
  }

  if (filename == NULL)
    return 0;
  
  this->textureImage = new GzImage;
  
  if (this->textureImage->Load(filename) != 0)
  {
    delete this->textureImage;
    this->textureImage = NULL;
    return -1;
  }
  
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Set size of the texture image when mapped on a geom.
void Geom::SetTexture2DSize( GzVector size )
{
  this->dirty = true;
  this->textureSize = size;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the visual skin (3DS format)
int Geom::SetSkinFile( const char *filename )
{
#if HAVE_LIB3DS
  this->dirty = true;
  // Load the skin file
  skinFile = lib3ds_file_load(filename);
  if (skinFile == NULL)
  {
    PRINT_WARN1("could not load [%s]", filename);
    return -1;
  }
  // Get the first frame
  lib3ds_file_eval(this->skinFile, 0);
  return 0;
#else
  PRINT_WARN("support for skins was disabled at compile time");
  return 0;
#endif  
}


//////////////////////////////////////////////////////////////////////////////
// Set the skin pose
void Geom::SetSkinPose( GzPose pose )
{
  this->dirty = true;
  this->skinPose = pose;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the skin scale
void Geom::SetSkinScale( GzVector scale )
{
  this->dirty = true;
  this->skinScale = scale;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the Shade Model
void Geom::SetShadeModel( const char *mode )
{
  this->dirty = true;
  // TODO
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the Polygon Mode
void Geom::SetPolygonMode( const char *mode )
{
  this->dirty = true;
  // TODO
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the transparency
void Geom::SetTransparency( bool enable )
{
  this->dirty = true;
  this->transparency = enable;

  if (this->transparency)
    this->renderOrder = GZ_RENDER_TRANSPARENT;
  else
    this->renderOrder = GZ_RENDER_OPAQUE;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the shininess
void Geom::SetShininess( float num )
{
  this->dirty = true;
  this->shininess = num;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the laser-reflectiveness
void Geom::SetLaser( float num )
{
  this->laser = num;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the retro-reflectiveness
void Geom::SetRetro( float num )
{
  this->retro = num;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Set the fiducial id
void Geom::SetFiducial( int id )
{
  this->fiducial = id;
  return;
}

//////////////////////////////////////////////////////////////////////////////
// Set the pick id. Allows a user to select an object with the mouse
void Geom::SetPickId( GLuint id )
{
  this->pick = id;
  return;
}

//////////////////////////////////////////////////////////////////////////////
// Get the ambient color
GzColor Geom::GetAmbientColor() const
{
  return GzColor(this->colorAmbient[0], this->colorAmbient[1],
                 this->colorAmbient[2], this->colorAmbient[3]);
}


//////////////////////////////////////////////////////////////////////////////
// Get the diffuse factor
GzColor Geom::GetDiffuseColor() const
{
  return GzColor(this->colorDiffuse[0], this->colorDiffuse[1],
                 this->colorDiffuse[2], this->colorDiffuse[3]);
}


//////////////////////////////////////////////////////////////////////////////
// Get the specular factor
GzColor Geom::GetSpecularColor() const
{
  return GzColor(this->colorSpecular[0], this->colorSpecular[1],
                 this->colorSpecular[2], this->colorSpecular[3]);
}


//////////////////////////////////////////////////////////////////////////////
// Get the transparency
float Geom::GetTransparency() const
{
  return this->transparency;
}


//////////////////////////////////////////////////////////////////////////////
// Get the shininess
float Geom::GetShininess() const
{
  return this->shininess;
}


//////////////////////////////////////////////////////////////////////////////
// Get the laser-reflectiveness
float Geom::GetLaser() const
{
  return this->laser;
}


//////////////////////////////////////////////////////////////////////////////
// Get the retor-reflectiveness
float Geom::GetRetro() const
{
  return this->retro;
}


//////////////////////////////////////////////////////////////////////////////
// Get the fiducial id
int Geom::GetFiducial() const
{
  return this->fiducial;
}

//////////////////////////////////////////////////////////////////////////////
// Get the pick id
GLuint Geom::GetPickId() const
{
  return this->pick;
}


//////////////////////////////////////////////////////////////////////////////
/// @brief Set stored list options for a particular camera
void Geom::SetList(int camera, GLuint listId, RenderOptions opt)
{
  assert(camera >= 0);
  assert(camera < (int) (sizeof(this->listList) / sizeof(this->listList[0])));

  this->listList[camera].listId = listId;
  this->listList[camera].opt = opt;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
/// @brief Get stored list options for a particular camera
void Geom::GetList(int camera, GLuint *listId, RenderOptions *opt)
{
  assert(camera >= 0);
  assert(camera < (int) (sizeof(this->listList) / sizeof(this->listList[0])));

  *listId = this->listList[camera].listId;
  *opt = this->listList[camera].opt;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Pre-renderer
void Geom::PreRender(RenderOptions *opt)
{
  GzPose pose;
  GzVector pos;
  GzQuatern rot;
  
  glPushMatrix();

//   // This tags the next drawn object the the "pick" id. 
//   // This call only doesn't something meaningful when
//   // glRenderMode(GL_SELECT)
//   glPushName(this->pick);

  // Get global object pose
  pose = this->GetPose();

  // Apply translation
  pos = pose.pos;
  glTranslatef(pos.x, pos.y, pos.z);

  // Apply rotation
  rot = GzQuaternToAxis(pose.rot);
  glRotatef(rot.u * 180 / M_PI, rot.x, rot.y, rot.z);
 
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Default render routine
void Geom::Render(RenderOptions *opt)
{  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Post-renderer
void Geom::PostRender(RenderOptions *opt)
{  

  this->dirty = false;
  //glPopName();
  glPopMatrix();
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Render the skin
void Geom::RenderSkin(RenderOptions *opt)
{  
#ifdef HAVE_LIB3DS
  GzVector pos;
  GzQuatern rot;
  Lib3dsNode *p;
  Lib3dsMesh *m;
  
  if (this->skinFile == NULL)
    return;

  // Need normalization for rescaled models
  glEnable(GL_NORMALIZE);

  // Apply translation
  pos = this->skinPose.pos;
  glTranslatef(pos.x, pos.y, pos.z);

  // Apply rotation
  rot = GzQuaternToAxis(this->skinPose.rot);
  glRotatef(rot.u * 180 / M_PI, rot.x, rot.y, rot.z);

  // Apply scaling
  glScalef(this->skinScale.x, this->skinScale.y, this->skinScale.z);
 
  if (this->skinFile->nodes != NULL)
    for (p = this->skinFile->nodes; p != NULL; p = p->next)
      this->RenderSkinNode(p);
  else if (this->skinFile->meshes != NULL)
  {
    //if (this->meshListId == 0)
    //{
      //this->meshListId = glGenLists(1); 
      //glNewList(this->meshListId, GL_COMPILE);
      for (m=this->skinFile->meshes; m!=NULL; m = m->next)
        this->RenderSkinMesh(m);
      //glEndList();
    //}

    /*if (this->meshListId)
    {
       Render the object
      glCallList(this->meshListId);
    }*/
  }

#endif
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Render a node (recursive)
void Geom::RenderSkinNode(Lib3dsNode *node)
{
#ifdef HAVE_LIB3DS
  Lib3dsNode *p;
  Lib3dsMesh *mesh;

  // Render the children first
  for (p = node->childs; p != 0; p = p->next)
    this->RenderSkinNode(p);

  if (node->type != LIB3DS_OBJECT_NODE)
    return; 
  if (strcmp(node->name,"$$$DUMMY") == 0)
    return;

  // If we dont have a display list yet, create one
  /*if (node->user.d == 0)
  {
    // TODO: this may clash with our list id assignment; FIX
    node->user.d = glGenLists(1); 
    glNewList(node->user.d, GL_COMPILE);
    
    mesh = lib3ds_file_mesh_by_name(this->skinFile, node->name);
    assert(mesh);    

    this->RenderSkinMesh( mesh );

    glEndList();
  }*/ 

  // If we do have a display list, render it
  //if (node->user.d)
  //{
    Lib3dsObjectData *d;

    glPushMatrix();

    // Apply translations from file
    d = &node->data.object;
    glMultMatrixf(&node->matrix[0][0]);
    glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);

    mesh = lib3ds_file_mesh_by_name(this->skinFile, node->name);
    assert(mesh);    

    this->RenderSkinMesh( mesh );

    // Render the object
    //glCallList(node->user.d);
    
    glPopMatrix();
  //}
#endif
  
  return;
}

//////////////////////////////////////////////////////////////////////////////
// Render a mesh 
void Geom::RenderSkinMesh( Lib3dsMesh *mesh )
{
#ifdef HAVE_LIB3DS
  unsigned p;
  Lib3dsVector *normalL=(Lib3dsVector *) malloc(3*sizeof(Lib3dsVector)*mesh->faces);

  {
    Lib3dsMatrix M;
    lib3ds_matrix_copy(M, mesh->matrix);
    lib3ds_matrix_inv(M);
    glMultMatrixf(&M[0][0]);
  }
  lib3ds_mesh_calculate_normals(mesh, normalL);

  for (p=0; p<mesh->faces; ++p) {
    Lib3dsFace *f=&mesh->faceL[p];
    Lib3dsMaterial *mat=0;
    if (f->material[0]) {
      mat=lib3ds_file_material_by_name(this->skinFile, f->material);
    }

    if (mat) {
      static GLfloat a[4]={0,0,0,1};
      float s;
      glMaterialfv(GL_FRONT, GL_AMBIENT, a);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
      s = pow(2, 10.0*mat->shininess);
      if (s>128.0) {
        s=128.0;
      }
      glMaterialf(GL_FRONT, GL_SHININESS, s);
    }
    else {
      Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
      Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
      Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
      glMaterialfv(GL_FRONT, GL_AMBIENT, a);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
      glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    }
    {
      int i;
      glBegin(GL_TRIANGLES);
      glNormal3fv(f->normal);
      for (i=0; i<3; ++i) {
        glNormal3fv(normalL[3*p+i]);
        glVertex3fv(mesh->pointL[f->points[i]].pos);
      }
      glEnd();
    }
  }

  free(normalL);
#endif
  return;
}
