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
/* Desc: Model for a LightSource
 * Author: Nathan Koenig
 * Date: Nov 2004
 * CVS: $Id: LightSource.cc,v 1.16 2005/05/06 17:09:27 natepak Exp $
 */

/// @addtogroup models
/// @{ 
/** @defgroup LightSource Light Source

The LightSource model simulates a single point light.

@par libgazebo interfaces

This model has no libgazebo interface.

@par Player drivers

There are no Player drivers available.

@par Attributes

The following attributes are supported.

@htmlinclude default_attr_include.html

- ambientColor (float tuple)
  - Ambient color.
  - Default: 0.2 0.2 0.2 1.0

- diffuseColor (float tuple)
  - Diffuse color.
  - Default: 0.8 0.8 0.8 1.0

- specularColor (float tuple)
  - Specular color.
  - Default: 0.2 0.2 0.2 1.0

- attenuation (float tuple)
  - Constant, Linear, Quadratic attenuation.
  - Default: 1.0 0.0 0.0


@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:LightSource>
  <xyz>0 0 0</xyz>
</model:LightSource>
@endverbatim

@par Authors

Nate Koenig, Andrew Howard

*/
/// @}


// General includes
#include <assert.h>
#include <math.h>

// Local includes
#include "gazebo.h"
#include "Error.hh"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "Body.hh"
#include "LightGeom.hh"
#include "LightSource.hh"



//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("LightSource", LightSource);


//////////////////////////////////////////////////////////////////////////////
// Constructor
LightSource::LightSource( World *world )
    : Model( world ), lightOn(false)
{
  /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  /////////////////////////////////////////////
  this->SetField(new GzColorField("ambientColor",GzColor(0.2,0.2,0.2,1.0)));
  this->SetField(new GzColorField("diffuseColor",GzColor(0.8,0.8,0.8,1.0)));
  this->SetField(new GzColorField("specularColor",GzColor(0.2,0.2,0.2,1.0)));
  this->SetField(new GzVectorField("attenuation",GzVectorSet(1.0,0.0,0.0)));
  /////////////////////////////////////////////

  this->body = NULL;
  this->lightGeom = NULL;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
LightSource::~LightSource()
{
  if (this->body)
    delete this->body;
  if (this->lightGeom)
    delete this->lightGeom;

  this->body = NULL;
  this->lightGeom = NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Load the model
int LightSource::Load( WorldFile *file, WorldFileNode *node )
{
  // Lights are a finite resource, so check
  if (!LightGeom::CheckAvailable())
  {
    PRINT_ERR("too many lights (check your GL driver limits)");
    return -1;
  }

  // Create a dummy body to attach the light geom to
  this->body = new Body(this->world, NULL, true);
  this->AddBody(this->body, true);

  // Create a light geom
  this->lightGeom = new LightGeom(this->body, this->modelSpaceId);

  // Set the light colors
  this->lightGeom->SetAmbientColor(node->GetColor("ambientColor", GzColor(0.2, 0.2, 0.2)));
  this->lightGeom->SetDiffuseColor(node->GetColor("diffuseColor", GzColor(0.8, 0.8, 0.8)));
  this->lightGeom->SetSpecularColor(node->GetColor("specularColor", GzColor(0.2, 0.2, 0.2)));

  // Set the attenuation factor
  this->lightGeom->SetAttenuation(node->GetPosition("attenuation", GzVectorSet(1.0, 0, 0)));

  /* FIX sometime
  WorldFileNode *spotNode = node->GetChild("spot");
  if (spotNode)
  {
    float exponent = spotNode->GetDouble("exponent",0);
    float cutoff = spotNode->GetDouble("cutoff",0);

    if (exponent != 0)
      this->light->setSpotExponent(exponent);

    if (exponent != 0)
      this->light->setSpotCutoff(cutoff);
  }
  */
  
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int LightSource::Init( WorldFile *file, WorldFileNode *node )
{
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the model
int LightSource::Fini()
{
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Update the model
void LightSource::Update( double /*step*/ )
{
  /* FIX
  //const GzVector pos = this->GetPosition();
  //const GzQuatern rot = this->parent->GetRotation();

  //printf("light %p %f %f %f\n", this->GetBody(), pos.x, pos.y, pos.z);

  //this->lightBody->SetPosition(pos);
  //this->lightBody->SetRotation(rot);

  //this->light->setPosition( osg::Vec4(pos.x, pos.y, pos.z,1.0) );
  //this->light->setDirection( osg::Vec3(rot.x, rot.y, rot.z) );
  */
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Returns whether the light is on or off
bool LightSource::IsOn() const
{
  return this->lightOn;
}

//////////////////////////////////////////////////////////////////////////////
// Turns the light source on
void LightSource::On()
{
  if (!this->lightOn)
  {
    // TODO
  }

  this->lightOn = true;
  return;
}

//////////////////////////////////////////////////////////////////////////////
// Turns the light source off
void LightSource::Off()
{
  if (this->lightOn)
  {
    // TODO
  }

  this->lightOn = false;
}

//////////////////////////////////////////////////////////////////////////////
// Toggles the source on or off
void LightSource::Toggle()
{
  // TODO
}



void LightSource::Start()
{
  // Lights are a finite resource, so check
  if (!LightGeom::CheckAvailable())
  {
    PRINT_ERR("too many lights (check your GL driver limits)");
    return;
  }

  // Create a dummy body to attach the light geom to
  this->body = new Body(this->world, NULL, true);
  this->AddBody(this->body, true);

  // Create a light geom
  this->lightGeom = new LightGeom(this->body, this->modelSpaceId);

  // Set the light colors
  this->lightGeom->SetAmbientColor(GzColor(0.2, 0.2, 0.2));
  this->lightGeom->SetDiffuseColor(GzColor(0.8, 0.8, 0.8));
  this->lightGeom->SetSpecularColor(GzColor(0.2, 0.2, 0.2));

  // Set the attenuation factor
  this->lightGeom->SetAttenuation(GzVectorSet(1.0, 0, 0)); 

  /////////////////////////////////////////////
  // Precisa ser true para a luz funcionar
  /////////////////////////////////////////////
  this->lightGeom->renderOrder = true;
}