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
/* Desc: Model for the sky dome
 * Author: Nate Koenig
 * Date: 24 Nov 2004
 * CVS: $Id: SkyDome.cc,v 1.7 2005/05/06 17:09:31 natepak Exp $
 */

/// @addtogroup models
/// @{
/** @defgroup SkyDome Sky Dome


The SkyDome model simulates a sky.

@par libgazebo interfaces

This model has no libgazebo interfaces.

@par Player drivers

No Player drivers are available.

@par Attributes

The following attributes are supported.

@htmlinclude default_attr_include.html

- toneMap (string, filename)
  - Image map of sky tones. Left=12am, Right=11:59pm. Top=Zenith, Bottom=horizon
  - Default: empty

- timeLapse (bool)
  - True=use elapsed time to adjust sky.
  - Default: true

@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:SkyDome>
</model:SkyDome>
@endverbatim


@par Authors

Nate Koenig

*/
/// @}

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/time.h>

#include "gazebo.h"
#include "Error.hh"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "SkyDome.hh"
#include "Body.hh"
#include "DomeGeom.hh"


//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("SkyDome", SkyDome);


//////////////////////////////////////////////////////////////////////////////
// Constructor
SkyDome::SkyDome( World *world )
 : Model( world ), domeGeom(NULL)
{
  /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  /////////////////////////////////////////////
  this->SetField(new StringField("toneMap",""));
  this->SetField(new BoolField("timeLapse",true));
  /////////////////////////////////////////////

  this->body = NULL;
  this->domeGeom = NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
SkyDome::~SkyDome()
{
  if (this->body)
    delete this->body;
  if (this->domeGeom)
    delete this->domeGeom;

  this->body = NULL;
  this->domeGeom = NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Load the sky dome for the world file
int SkyDome::Load( WorldFile *file, WorldFileNode *node )
{
  const char *textureFile;

  int *aint = new int[1];

  // Create a dummy body to which to attach geoms
  this->body = new Body(this->world, NULL, true);
  this->AddBody(this->body, true);

  // Create a sky dome
  this->domeGeom = new DomeGeom( this->body, this->modelSpaceId, 10);

  // Set properties
  this->domeGeom->SetColor(node->GetColor("color", GzColor(0, 0, 0)));

  this->timeLapse = node->GetBool("timeLapse",true);

  // Apply a texture
  textureFile = node->SearchFilename("toneMap", GAZEBO_TEXTURES_PATH, NULL);
  if (textureFile)
  {
    PRINT_MSG1(2, "loading toneMap file [%s]", textureFile);  
    if (this->domeGeom->SetTexture2DFile(textureFile) != 0)
      PRINT_WARN("unable to load toneMap file");
  }

  // This a fixed model, and should not collide with other fixed models
  dGeomSetCategoryBits( (dGeomID) this->modelSpaceId, GZ_FIXED_COLLIDE );
  dGeomSetCollideBits( (dGeomID) this->modelSpaceId, ~GZ_FIXED_COLLIDE );
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the plane
int SkyDome::Init( WorldFile *file, WorldFileNode *node )
{
  this->domeGeom->SetDayTime( this->world->GetDayTime() );
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the plane
int SkyDome::Fini()
{
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update the sky dome
void SkyDome::Update( double /*step*/ )
{
  if (this->timeLapse)
    this->domeGeom->SetElapsedTime( this->world->GetSimTime() );
}

/////////////////////////////////////////////////////////////////////
// Metodo que inicia a geometria as propriades padroes do modelo
/////////////////////////////////////////////////////////////////////
void SkyDome::Start()
{
  const char *textureFile;

  int *aint = new int[1];

  // Create a dummy body to which to attach geoms
  this->body = new Body(this->world, NULL, true);
  this->AddBody(this->body, true);

  // Create a sky dome
  this->domeGeom = new DomeGeom( this->body, this->modelSpaceId, 10);

  // Set properties
  this->domeGeom->SetColor(GzColor(0, 0, 0));

  this->timeLapse = true;

  // Apply a texture
//   textureFile = GAZEBO_TEXTURES_PATH;
//   if (textureFile)
//   {
//     PRINT_MSG1(2, "loading toneMap file [%s]", textureFile);  
//     if (this->domeGeom->SetTexture2DFile(textureFile) != 0)
//       PRINT_WARN("unable to load toneMap file");
//   }

  // This a fixed model, and should not collide with other fixed models
  dGeomSetCategoryBits( (dGeomID) this->modelSpaceId, GZ_FIXED_COLLIDE );
  dGeomSetCollideBits( (dGeomID) this->modelSpaceId, ~GZ_FIXED_COLLIDE );
}

