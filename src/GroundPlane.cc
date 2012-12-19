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
/* Desc: Model for the ground plane
 * Author: Nate Koenig
 * Date: 07 July 2003
 * CVS: $Id: GroundPlane.cc,v 1.39 2005/05/06 17:09:27 natepak Exp $
 */

/// @addtogroup models
/// @{ 
/** @defgroup GroundPlane Ground Plane


The GroundPlane model simulates a flat infinite plane.

@par libgazebo interfaces

This model has no libgazebo interfaces.

@par Player drivers

No Player drivers are available.

@par Attributes

The following attributes are supported.

@htmlinclude default_attr_include.html

- normal (float tuple, unit vector)
  - Direction of the normal vector.
  - Default: 0 0 1

- height (float, meters)
  - Distance along the z-axis to place the plane.
  - Default: 0

- size (float tuple, meters)
  - Size of the rendered plane (the plane is infinite, but only part of
    it is rendered in cameras).
  - Default: 100 100

- color (float tuple)
  - RGB color.
  - Default: 0 0 0

- textureFile (string, filename)
  - Texture image filename.
  - Default: empty

- textureSize (float tuple, meters)
  - Size of the texture image when projected onto the ground plane (m).
  - Default: 2.0 2.0
  
- surfaceHardness (float tuple)
  - Set spring-damper parameters for contact joints.
  - Default: dInfinity 0

- surfaceFriction (float tuple)
  - Set friction coefficients
  - Default: 1 1


@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:GroundPlane>
  <xyz>0 0 0</xyz>
</model:GroundPlane>
@endverbatim


@par Authors

Andrew Howard, Nate Koenig

*/
/// @} 

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "gazebo.h"
#include "Error.hh"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "GroundPlane.hh"
#include "Body.hh"
#include "PlaneGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("GroundPlane", GroundPlane);


//////////////////////////////////////////////////////////////////////////////
// Constructor
GroundPlane::GroundPlane( World *world )
  : Model( world ), planeGeom(NULL)
{
  /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  /////////////////////////////////////////////
  this->SetField(new GzVectorField("normal",GzVectorSet(0.0,0.0,1.0)));
  this->SetField(new DoubleField("height",0.0));
  this->SetField(new DoubleField("sizeX",0.0));
  this->SetField(new DoubleField("sizeY",0.0));
  this->SetField(new GzColorField("color",GzColor(0.0,0.5,0.0)));
  this->SetField(new StringField("textureFile",""));
  this->SetField(new DoubleTupleField("textureSize",DoubleTupleSet(0.0,0.0)));
  this->SetField(new DoubleTupleField("surfaceHardness",DoubleTupleSet(0.0,0.0)));
  this->SetField(new DoubleTupleField("surfaceFriction",DoubleTupleSet(0.0,0.0)));
  /////////////////////////////////////////////

  this->body = NULL;
  this->planeGeom = NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
GroundPlane::~GroundPlane()
{
  if (this->body)
    delete this->body;
  if (this->planeGeom)
    delete this->planeGeom;

  this->body = NULL;
  this->planeGeom = NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Load the ground plane for the world file
int GroundPlane::Load( WorldFile *file, WorldFileNode *node )
{
  GzVector normal;
  double altitude;
  const char *textureFile;

  // The plane parameters
  altitude = node->GetLength("height", 0.0);
  normal = node->GetPosition("normal", GzVectorSet(0, 0, 1));

  // Create a dummy body to which to attach geoms
  this->body = new Body(this->world, NULL, true);
  this->AddBody(this->body, true);

  // Create a ground plane
  this->planeGeom = new PlaneGeom( this->body, this->modelSpaceId, altitude, normal );

  // Set properties
  this->planeGeom->SetColor(node->GetColor("color", GzColor(0, 0.5, 0)));
  //this->planeGeom->SetSpecularColor(GzColor(0, 0, 0));  

  // Apply a texture
  textureFile = node->SearchFilename("textureFile", GAZEBO_TEXTURES_PATH, NULL);
  if (textureFile)
  {
    PRINT_MSG1(2, "loading texture file [%s]", textureFile);  
    if (this->planeGeom->SetTexture2DFile(textureFile) != 0)
      PRINT_WARN("unable to load texture file");
  }
  this->planeGeom->SetTexture2DSize(node->GetPosition("textureSize", GzVectorSet(2.0, 2.0, 0.0)));

  // Surface hardness
  this->planeGeom->SetHardness(node->GetTupleDouble("surfaceHardness", 0, dInfinity),
                               node->GetTupleDouble("surfaceHardness", 1, 0));

  // Surface friction
  this->planeGeom->SetFriction(node->GetTupleDouble("surfaceFriction", 0, 1.0),
                               node->GetTupleDouble("surfaceFriction", 1, 1.0));

  // This a fixed model, and should not collide with other fixed models
  dGeomSetCategoryBits( (dGeomID) this->modelSpaceId, GZ_FIXED_COLLIDE );
  dGeomSetCollideBits( (dGeomID) this->modelSpaceId, ~GZ_FIXED_COLLIDE );

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the plane
int GroundPlane::Init( WorldFile *file, WorldFileNode *node )
{
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the plane
int GroundPlane::Fini()
{
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update the ground plane
void GroundPlane::Update( double /*step*/ )
{
}

//////////////////////////////////////////////////////////////////////////////
// Inicia o modelo com os valores padroes das propriedades
//////////////////////////////////////////////////////////////////////////////
void GroundPlane::Start()
{
  GzVector normal;
  double altitude;
  const char *textureFile;

  // The plane parameters
  altitude = 0.0;
  normal = GzVectorSet(0, 0, 1);

  // Create a dummy body to which to attach geoms
  this->body = new Body(this->world, NULL, true);
  this->AddBody(this->body, true);

  // Create a ground plane
  this->planeGeom = new PlaneGeom( this->body, this->modelSpaceId, altitude, normal );

  // Set properties
  this->planeGeom->SetColor(GzColor(0, 0.5, 0));
  
  // Apply a texture
  //textureFile = GAZEBO_TEXTURES_PATH;
  //if (textureFile)
  //{
  //  PRINT_MSG1(2, "loading texture file [%s]", textureFile);
  //  if (this->planeGeom->SetTexture2DFile(textureFile) != 0)
  //    PRINT_WARN("unable to load texture file");
  //}

  GzVector textureSize = GzVectorSet(2.0, 2.0, 0.0);
  this->planeGeom->SetTexture2DSize(textureSize);

  // Surface hardness
  this->planeGeom->SetHardness(dInfinity,0);

  // Surface friction
  this->planeGeom->SetFriction(1.0,1.0);

  // This a fixed model, and should not collide with other fixed models
  dGeomSetCategoryBits( (dGeomID) this->modelSpaceId, GZ_FIXED_COLLIDE );
  dGeomSetCollideBits( (dGeomID) this->modelSpaceId, ~GZ_FIXED_COLLIDE );
}

