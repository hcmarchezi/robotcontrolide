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
/* Desc: Road geometry
 * Author: Nate Keonig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: RoadGeom.cc,v 1.12 2004/12/07 00:15:06 natepak Exp $
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_TRIMESH

#include <assert.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Image.hh"
#include "RoadGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
RoadGeom::RoadGeom( Body *body, dSpaceID spaceId )
    : Geom( spaceId )
{

  this->scale = 0.50;
  this->stepsX = 20000;
  this->stepsY = 33;

  this->trackScale = 10.0;
  this->profileScale = 1.0;

  this->trackPointCount = 0;
  this->trackPointMaxCount = 0;
  this->trackPoints = NULL;

  this->signCount = 0;
  this->signMaxCount = 0;
  this->signs = NULL;

  this->vertexCount = 0;
  this->vertexMaxCount = 0;
  this->vertices = NULL;
  this->normals = NULL;

  this->indexCount = 0;
  this->indexMaxCount = 0;
  this->indices = NULL;

  this->patchSize = 10.0;

  // Initialize the mesh
  this->InitMesh();

  // Create an object for managing trimesh data
  this->tridata = dGeomTriMeshDataCreate();

  // Construct the trimesh data
  dGeomTriMeshDataBuildSimple(this->tridata,
                              (dReal*) this->vertices, this->vertexCount,
                              this->indices, this->indexCount);
  
  // Create and ODE trimesh geom
  this->trimesh = dCreateTriMesh(spaceId, this->tridata, NULL, NULL, NULL);

  // Initialize underlying geom
  this->SetGeom(body, this->trimesh, NULL, false);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
RoadGeom::~RoadGeom()
{
  int i;
  
  // Clean up
  dGeomDestroy(this->trimesh);
  dGeomTriMeshDataDestroy(this->tridata);

  free(this->indices);
  free(this->vertices);

  for (i = 0; i < this->trackPointCount; i++)
    free(this->trackPoints[i].profilePoints);
  free(this->trackPoints);  

  for (i = 0; i < this->signCount; i++)
    free(this->signs[i].text);
  free(this->signs);  

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Add a point to the track
int RoadGeom::AddTrackPoint(GzVector pos)
{
  size_t size;
  TrackPoint *point;
  
  if (this->trackPointCount >= this->trackPointMaxCount)
  {
    this->trackPointMaxCount += 1024;
    size = sizeof(this->trackPoints[0]) * this->trackPointMaxCount;
    this->trackPoints = (TrackPoint*) realloc(this->trackPoints, size);
  }  

  point = this->trackPoints + this->trackPointCount++;
  point->pos = pos;
  point->profilePointCount = 0;
  point->profilePointMaxCount = 0;
  point->profilePoints = NULL;

  return this->trackPointCount - 1;
}


//////////////////////////////////////////////////////////////////////////////
// Add a point to the profile
void RoadGeom::AddProfilePoint(int index, double pz)
{
  size_t size;
  TrackPoint *point;

  assert(index >= 0 && index < this->trackPointCount);
  point = this->trackPoints + index;
  
  if (point->profilePointCount >= point->profilePointMaxCount)
  {
    point->profilePointMaxCount += 8;
    size = sizeof(point->profilePoints[0]) * point->profilePointMaxCount;
    point->profilePoints = (GzVector*) realloc(point->profilePoints, size);
  }  
  point->profilePoints[point->profilePointCount++] = GzVectorSet(0, 0, pz);
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Add a sign.
void RoadGeom::AddSign(GzVector pos, const char *text)
{
  size_t size;
  SignData *sign;
  
  if (this->signCount >= this->signMaxCount)
  {
    this->signMaxCount += 254;
    size = sizeof(this->signs[0]) * this->signMaxCount;
    this->signs = (SignData*) realloc(this->signs, size);
  }  

  sign = this->signs + this->signCount++;
  sign->pos = pos;
  sign->text = strdup(text);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the mesh
void RoadGeom::InitMesh()
{
  int i, j;
  dReal *vertex;

  // Allocate vertex storage
  this->vertexCount = 0;
  this->vertexMaxCount = this->stepsX * this->stepsY;
  this->vertices = (dVector3*) realloc(this->vertices,
                                       sizeof(this->vertices[0]) * this->vertexMaxCount);
  this->normals = (GzVector*) realloc(this->normals,
                                      sizeof(this->normals[0]) * this->vertexMaxCount);
    
  // Initialize vertices
  for (i = 0; i < this->stepsX; i++)
  {
    for (j = 0; j < this->stepsY; j++)
    {
      vertex = this->vertices[this->vertexCount++];
      vertex[0] = i * this->scale;
      vertex[1] = j * this->scale;
      vertex[2] = i * 0.05;
    }
  }

  // Allocate index storage
  this->indexCount = 0;
  this->indexMaxCount = this->stepsX * this->stepsY * 6;
  this->indices = (int*) realloc(this->indices,
                                 sizeof(this->indices[0]) * this->indexMaxCount);

  // Construct triangle strips
  for (i = 0; i < this->stepsX - 1; i++)
  {
    for (j = 0; j < this->stepsY - 1; j++)
    { 
      assert(this->indexCount + 6 <= this->indexMaxCount);
      this->indices[this->indexCount++] = (i + 0) * this->stepsY + j;
      this->indices[this->indexCount++] = (i + 1) * this->stepsY + j;
      this->indices[this->indexCount++] = (i + 0) * this->stepsY + j + 1;     
      this->indices[this->indexCount++] = (i + 0) * this->stepsY + j + 1;
      this->indices[this->indexCount++] = (i + 1) * this->stepsY + j;
      this->indices[this->indexCount++] = (i + 1) * this->stepsY + j + 1;
    }
  }
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Regenerate the mesh
void RoadGeom::UpdateMesh(GzVector pos)
{
  int i, j;
  int si, ti;
  int n0, n1, n2, n3;
  double s, sm, t, tm;
  TrackPoint *point;
  GzVector o, ot, p;
  dReal *vertex;

  /*
  // HACK
  // Find the nearest point in the track
  trackMin = 0;
  distMin = DBL_MAX;
  
  for (i = 0; i < this->trackPointCount; i++)
  {
  p = this->trackPoints[i];
  p = GzVectorSub(pos, p);
  s = GzVectorDot(p, p);

  if (s < distMin)
  {
  distMin = s;
  trackMin = i;
  }
  }
  */
  
  // Consider points along the track
  for (i = 0; i < this->stepsX; i++)
  {
    //s = (i - this->stepsX / 2) * this->scale;
    s = i * this->scale;

    // Figure out where we are in the track
    si = (int) (floor(s / this->trackScale));
    sm = s / this->trackScale - si;

    si += 1;
        
    //printf("s si sm %f %d %f\n", s, si, sm);
    assert(si >= 1 && si + 2 < this->trackPointCount);
      
    // Use a spline to interpolate our position
    o = this->SplineCR(sm,
                       this->trackPoints[si - 1].pos,
                       this->trackPoints[si + 0].pos,
                       this->trackPoints[si + 1].pos,
                       this->trackPoints[si + 2].pos);
    ot = this->SplineCRtan(sm,
                           this->trackPoints[si - 1].pos,
                           this->trackPoints[si + 0].pos,
                           this->trackPoints[si + 1].pos,
                           this->trackPoints[si + 2].pos);
    //printf("%f %f %f\n", o.x, o.y, o.z);

    assert(si >= 0 && si < this->trackPointCount);
    point = this->trackPoints + si;      

    // TESTING HACK
    if (point->profilePointCount >= 4)
    {
      point->profilePoints[0].z = -o.z - 0.10;
      point->profilePoints[1].z = -o.z - 0.10;
      point->profilePoints[point->profilePointCount - 2].z = -o.z - 0.10;    
      point->profilePoints[point->profilePointCount - 1].z = -o.z - 0.10;
    }

    // Consider points along the profile
    for (j = 0; j < this->stepsY; j++)
    {
      vertex = this->vertices[i * this->stepsY + j];

      t = (j - this->stepsY / 2) * this->scale;

      // Figure our where we are in the profile
      ti = (int) (floor(t / this->profileScale));
      tm = t / this->profileScale - ti;
      ti += point->profilePointCount / 2;
      //printf("t ti tm %f %d %f\n", t, ti, tm);      

      // Spline control point indexes
      n0 = ti - 1;
      n1 = ti + 0;
      n2 = ti + 1;
      n3 = ti + 2;
      
      // Handle boundary conditions for spline fit
      n0 = Min(Max(0, n0), point->profilePointCount - 1);
      n1 = Min(Max(0, n1), point->profilePointCount - 1);
      n2 = Min(Max(0, n2), point->profilePointCount - 1);
      n3 = Min(Max(0, n3), point->profilePointCount - 1);

      assert(n0 >= 0 && n0 < point->profilePointCount);
      assert(n1 >= 0 && n1 < point->profilePointCount);
      assert(n2 >= 0 && n2 < point->profilePointCount);
      assert(n3 >= 0 && n3 < point->profilePointCount);
        
      // Use a spline to interpolate our altitude
      p = this->SplineCR(tm,
                         point->profilePoints[n0],
                         point->profilePoints[n1],
                         point->profilePoints[n2],
                         point->profilePoints[n3]);
      
      // Update the vertex position
      vertex[0] = o.x - ot.y * t;
      vertex[1] = o.y + ot.x * t;      
      vertex[2] = o.z + p.z;
    }
  }

  // Compute normal vectors
  this->UpdateNormals();

  // Construct the trimesh data
  dGeomTriMeshDataBuildSimple(this->tridata,
                              (dReal*) this->vertices, this->vertexCount,
                              this->indices, this->indexCount);
  dGeomTriMeshSetData(this->trimesh, this->tridata);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Compute normal vectors on vertices
void RoadGeom::UpdateNormals()
{
  int i, a, b, c;
  dVector3 *v;
  GzVector v0, v1, v2, n;

  v = this->vertices;

  // Reset the normals
  for (i = 0; i < this->vertexCount; i++)
    this->normals[i] = GzVectorZero();

  // Consider each triangle
  for (i = 0; i < this->indexCount; i += 3)
  {
    a = this->indices[i + 0];
    b = this->indices[i + 1];
    c = this->indices[i + 2];

    // Compute the normal
    v0 = GzVectorSet(v[a][0], v[a][1], v[a][2]);
    v1 = GzVectorSet(v[b][0], v[b][1], v[b][2]);
    v2 = GzVectorSet(v[c][0], v[c][1], v[c][2]);
    n = GzVectorCross(GzVectorSub(v1, v0), GzVectorSub(v2, v1));
    n = GzVectorUnit(n);

    // Accumate normals across triangles
    // We assume normals are being normalized by OpenGL
    this->normals[a] = GzVectorAdd(this->normals[a], n);
    this->normals[b] = GzVectorAdd(this->normals[b], n);
    this->normals[c] = GzVectorAdd(this->normals[c], n);
  }

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Compute a Catmull-Rom spline
GzVector RoadGeom::SplineCR(double t, GzVector p0, GzVector p1,
                            GzVector p2, GzVector p3)
{
  GzVector q;

  q.x = 2*p1.x +
    (-p0.x + p2.x) * t +
    (2*p0.x - 5 * p1.x + 4*p2.x - p3.x) * t*t +
    (-p0.x + 3*p1.x - 3*p2.x + p3.x) * t*t*t;

  q.y = 2*p1.y +
    (-p0.y + p2.y) * t +
    (2*p0.y - 5 * p1.y + 4*p2.y - p3.y) * t*t +
    (-p0.y + 3*p1.y - 3*p2.y + p3.y) * t*t*t;

  q.z = 2*p1.z +
    (-p0.z + p2.z) * t +
    (2*p0.z - 5 * p1.z + 4*p2.z - p3.z) * t*t +
    (-p0.z + 3*p1.z - 3*p2.z + p3.z) * t*t*t;

  q = GzVectorMul(0.5, q);
  
  return q;
}


//////////////////////////////////////////////////////////////////////////////
// Compute a Catmull-Rom spline tangent
GzVector RoadGeom::SplineCRtan(double t, GzVector p0, GzVector p1,
                               GzVector p2, GzVector p3)
{
  GzVector v0, v1, v;
  
  v0 = GzVectorSub(p2, p0);
  v0 = GzVectorMul(0.5, v0);
  v1 = GzVectorSub(p3, p1);
  v1 = GzVectorMul(0.5, v1);

  v = GzVectorMul((1 - t), v0);
  v = GzVectorAdd(v, GzVectorMul(t, v1));

  v = GzVectorUnit(v);
  
  return v;
}


//////////////////////////////////////////////////////////////////////////////
// Default render routine
void RoadGeom::Render(RenderOptions *opt)
{
  int i, j;
  int ni, nj, mi, mj;
  GLuint listId;
  RenderOptions listOpt;
  
  // Recover stored display list for this camera
  this->GetList(opt->cameraIndex, &listId, &listOpt);

  // See if the current display list is dirty
  this->dirty |= (listId == 0);
  this->dirty |= (opt->displayMaterials != listOpt.displayMaterials);
  this->dirty |= (opt->displayTextures != listOpt.displayTextures);

  // Compute offset based on old camera pose
  mi = (int) floor(listOpt.cameraPose.pos.x / this->patchSize + 0.5);
  mj = (int) floor(listOpt.cameraPose.pos.y / this->patchSize + 0.5);

  // Compute offset based on new camera pose
  ni = (int) floor(opt->cameraPose.pos.x / this->patchSize + 0.5);
  nj = (int) floor(opt->cameraPose.pos.y / this->patchSize + 0.5);

  // If the offset has changed, we need to redraw
  this->dirty |= (ni != mi);
  this->dirty |= (nj != mj);

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

    dReal *v;
    GzVector n;
    double tx, ty;

    // Texture scale; each texture image is scaled such that it maps
    // to a road patch of size textureSize
    tx = this->scale / this->textureSize.x;
    ty = this->scale / this->textureSize.y;
    
    // Construct triangle strips
    for (i = 0; i < this->stepsX - 1; i++)
    {
      // Test vertex to see if it is near the current camera position
      v = this->vertices[this->stepsY * i + this->stepsY / 2];
      if (fabs(v[0] - opt->cameraPose.pos.x) > opt->farClip)
        continue;
      if (fabs(v[1] - opt->cameraPose.pos.y) > opt->farClip)
        continue;

      glBegin(GL_TRIANGLE_STRIP);
      
      for (j = 0; j < this->stepsY; j++)
      {
        glTexCoord2f(i * tx, j * tx);        
        n = this->normals[this->stepsY * (i + 0) + j];
        v = this->vertices[this->stepsY * (i + 0) + j];
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(v[0], v[1], v[2]);

        glTexCoord2f((i + 1) * tx, j * tx);
        n = this->normals[this->stepsY * (i + 1) + j];
        v = this->vertices[this->stepsY * (i + 1) + j];        
        glNormal3f(n.x, n.y, n.z);
        glVertex3f(v[0], v[1], v[2]);
      }

      glEnd();
    }

    if (opt->displayTextures && this->textureImage)
      glDisable(GL_TEXTURE_2D);

    // Display signs
    this->RenderSigns(opt, listOpt);
    
    // Store list options
    this->SetList(opt->cameraIndex, listId, *opt);      
    glEndList();
  }
  
  // Call the display list
  if (listId)
    glCallList(listId);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Render signs
void RoadGeom::RenderSigns(RenderOptions *opt, RenderOptions listOpt)
{
  int i, j;
  double size;
  GzVector p;
  SignData *sign;

  // Set material properties
  if (opt->displayMaterials)
  {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, GzColor(0, 0, 0));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, GzColor(0, 0, 0));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, GzColor(0, 0, 0));
  }

  for (i = 0; i < this->signCount; i++)
  {
    sign = this->signs + i;
    p = sign->pos;
        
    // Test sign to see if it is near the current camera position
    if (fabs(p.x - opt->cameraPose.pos.x) > opt->farClip)
      continue;
    if (fabs(p.y - opt->cameraPose.pos.y) > opt->farClip)
      continue;

    // Set sign hight
    size = 0.80;
    
    // Print the sign
    glPushMatrix();
    glTranslatef(p.x, p.y, p.z + 1.0); // HACK
    glRotatef(90, 1, 0, 0);
    glScalef(size / 150, size / 150, size / 150);
    for (j = 0; j < (int) strlen(sign->text); j++)
      glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, sign->text[j]);
    glPopMatrix();

    // Print a little stand
    glBegin(GL_LINES);
    glVertex3f(p.x, p.y, p.z - size / 2);
    glVertex3f(p.x, p.y, 0.0);
    glEnd();
  }

  return;
}

#endif

