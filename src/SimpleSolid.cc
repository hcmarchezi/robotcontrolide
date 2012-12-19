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
/* Desc: Model for simple solids, such as boxes and cylinders
 * Author: Andrew Howard
 * Date: 3 Aug 2003
 * CVS: $Id: SimpleSolid.cc,v 1.37 2006/03/02 22:24:46 natepak Exp $
 */

/// @addtogroup models
/// @{ 
/** @defgroup SimpleSolid Simple Solid

@htmlinclude SimpleSolid_view.html

The SimpleSolid model creates simple solid objects, such as spheres
and boxes.  The solids have mass, and will interact with other
objects.


@par libgazebo interfaces

This model does not have any interfaces.

@par Player drivers

There are no player drivers for this model

@par Attributes

The following attributes are supported.

@htmlinclude default_attr_include.html

- shape (string)
  - The shape of solid; can be "box", "cylinder" or "sphere".
  - Default: box

- size (float tuple, meters)
  - The size of the solid; the exact meaning depends upon the shape:
    - box: x, y and z dimensions of the box.
    - cylinder: cylinder diameter and height (along z-axis).
    - sphere: diameter of the sphere
    - Default: 1.0 1.0 1.0

- mass (float, kg)
  - Object mass
  - Default: 1

- color (float tuple)
  - RGB color.
  - Default: 0 0 0

- surfaceHardness (float tuple)
  - Set spring-damper parameters for contact joints
  - Default: dInfinity 0

- surfaceFriction (float tuple)
  - Set friction coefficients
  - Default: 1 1

- transparent (bool)
  - Set to true to make the object transparent
  - Default: false

- textureFile (string, filename)
  - Texture image filename.
  - Default: empty

- skinFile (string, filename)
  - Specify an 3D geometry file to create an arbitrary appearance.
  - Default: empty

- skinXyz (float tuple, meters)
  - Skin pose relative to the model
  - Default: 0 0 0

- skinRpy (float tuple, degrees)
  - Skin rotation relative to the model in Euler angles: roll, pitch, yaw
  - Default: 0.0 0.0 0.0

- skinScale (float tuple, meters)
  - Scale factor for skin file.
  - Default: 1.0 1.0 1.0

- retro (float)
  - Retro-reflectivity in laser scan (in range 0.0 to 1.0).
  - Default: 0

- fiducial (integer)
  - Fiducial code in laser scan.
  - Default: -1


@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:SimpleSolid>
  <xyz>0 0 0</xyz>
  <shape>box</shape>
  <color>1 0 0</color>
</model:SimpleSolid>
@endverbatim

@par Views

@htmlinclude SimpleSolid_more_views.html

@par Authors

Andrew Howard

*/
/// @}

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <math.h>

#include "gazebo.h"
#include "Error.hh"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "SphereGeom.hh"
#include "BoxGeom.hh"
#include "CylinderGeom.hh"
#include "SimpleSolid.hh"



//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("SimpleSolid", SimpleSolid);


SimpleSolid::SimpleSolid( World *world,const char * shape):Model(world)
{
 /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  /////////////////////////////////////////////
  this->SetField(new StringField("shape",""));
  this->SetField(new DoubleField("mass",0.0));
  this->SetField(new GzColorField("color",GzColor(1.0,1.0,1.0,1.0)));
  this->SetField(new DoubleTupleField("surfaceHardness",DoubleTupleSet(0.0,0.0)));
  this->SetField(new DoubleTupleField("surfaceFriction",DoubleTupleSet(0.0,0.0)));
  this->SetField(new BoolField("transparent",true));
  this->SetField(new StringField("textureFile",""));
  this->SetField(new StringField("skinFile",""));
  this->SetField(new GzVectorField("skinXyz",GzVectorSet(0.0,0.0,0.0)));
  this->SetField(new GzVectorField("skinRpy",GzVectorSet(0.0,0.0,0.0)));
  this->SetField(new GzVectorField("skinScale",GzVectorSet(0.0,0.0,0.0)));
  this->SetField(new DoubleField("retro",0.0));
  this->SetField(new IntegerField("fiducial",0));
  /////////////////////////////////////////////

  this->body = NULL;
  this->geom = NULL;

  ///////////////////////////////////////////////////////
  // Tipo da propriedade size muda com o tipo de solido
  ///////////////////////////////////////////////////////
  if (strcmp(shape,"box") == 0)
  {
    this->SetField(new GzVectorField("size",GzVectorSet(0.5,0.5,0.5)));
    this->GetField("shape")->SetStringValue("box");
  }
  else if (strcmp(shape,"cylinder") == 0)
  { 
    this->SetField(new DoubleTupleField("size",DoubleTupleSet(0.1,0.5)));
    this->GetField("shape")->SetStringValue("cylinder");
  }
  else if (strcmp(shape,"sphere") == 0)
  {
    this->SetField(new DoubleField("size",0.5));
    this->GetField("shape")->SetStringValue("sphere");
  }
  else
  {
    throw new SystemException("Shape invalido.","SimpleSolid::SimpleSolid");
  }
  //////////////////////////////////////////////////////////

  return;
}



//////////////////////////////////////////////////////////////////////////////
// Constructor
SimpleSolid::SimpleSolid( World *world )
    : Model( world )
{
  /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  /////////////////////////////////////////////
  this->SetField(new StringField("shape",""));
  this->SetField(new DoubleField("mass",0.0));
  this->SetField(new GzColorField("color",GzColor(1.0,1.0,1.0,1.0)));
  this->SetField(new DoubleTupleField("surfaceHardness",DoubleTupleSet(0.0,0.0)));
  this->SetField(new DoubleTupleField("surfaceFriction",DoubleTupleSet(0.0,0.0)));
  this->SetField(new BoolField("transparent",true));
  this->SetField(new StringField("textureFile",""));
  this->SetField(new StringField("skinFile",""));
  this->SetField(new GzVectorField("skinXyz",GzVectorSet(0.0,0.0,0.0)));
  this->SetField(new GzVectorField("skinRpy",GzVectorSet(0.0,0.0,0.0)));
  this->SetField(new GzVectorField("skinScale",GzVectorSet(0.0,0.0,0.0)));
  this->SetField(new DoubleField("retro",0.0));
  this->SetField(new IntegerField("fiducial",0));
  /////////////////////////////////////////////

  this->body = NULL;
  this->geom = NULL;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
SimpleSolid::~SimpleSolid()
{
  if (this->body)
    delete this->body;
  if (this->geom)
    delete this->geom;

  this->body = NULL;
  this->geom = NULL;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Load the model
int SimpleSolid::Load( WorldFile *file, WorldFileNode *node )
{
  // Create the ODE objects
  if (this->OdeLoad( file, node ) != 0)
    return -1;
  
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load ODE objects
int SimpleSolid::OdeLoad( WorldFile *file, WorldFileNode *node )
{
  const char *geomShape;
  const char *textureFile;
  const char *skinFile;
  
  // Create a body
  this->body = new Body( this->world );
  this->AddBody( this->body, true );

  geomShape = node->GetString("shape", "box", 0);

  this->body->Enable(node->GetBool("enable",true,0));
  this->body->SetGravityMode(node->GetBool("gravity",true,0));

  // Create a geom of the correct shape
  if (strcmp(geomShape, "box") == 0)
  {
    double sx, sy, sz;

    sx = node->GetTupleLength("size", 0, 0.5);
    sy = node->GetTupleLength("size", 1, 0.5);
    sz = node->GetTupleLength("size", 2, 0.5);

    this->geom = new BoxGeom( this->body, this->modelSpaceId, sx, sy, sz );
    this->geom->SetMass( node->GetDouble("mass", 1, 0) );
    this->geom->SetRelativePosition(GzVectorSet(0, 0, sz / 2));
  }
  else if (strcmp(geomShape, "cylinder") == 0)
  {
    double len, diam;

    diam = node->GetTupleLength("size", 0, 0.1);
    len = node->GetTupleLength("size", 1, 0.5);
    
    this->geom = new CylinderGeom( this->body, this->modelSpaceId, diam / 2, len );
    this->geom->SetRelativePosition(GzVectorSet(0, 0, len/2));
    this->geom->SetMass( node->GetDouble("mass", 1, 0) );
  }
  else if (strcmp(geomShape, "sphere") == 0)
  {
    double diam;

    diam = node->GetTupleLength("size", 0, 0.5);
    
    this->geom = new SphereGeom( this->body, this->modelSpaceId, diam / 2 );
    this->geom->SetMass( node->GetDouble("mass", 1, 0) );
    this->geom->SetRelativePosition(GzVectorSet(0, 0, diam / 2));    
  }
  else
  {
    PRINT_ERR1("unknown shape [%s]", geomShape);
    return -1;
  }

  this->geom->SetTransparency(node->GetBool("transparent", false));
  this->geom->SetShadeModel(node->GetString("shadeModel","flat",0));
  this->geom->SetPolygonMode(node->GetString("polygonMode","fill",0));

  // Set generic properties
  this->geom->SetColor( node->GetColor("color", GzColor(1.0, 1.0, 1.0)) );
  this->geom->SetRetro( node->GetDouble("retro", 0.0) );
  this->geom->SetFiducial( node->GetInt("fiducial", -1) );
  
  // Surface properties
  this->geom->SetHardness(node->GetTupleDouble("surfaceHardness", 0, 1e6),
                          node->GetTupleDouble("surfaceHardness", 1, 0));
  this->geom->SetFriction(node->GetTupleDouble("surfaceFriction", 0, 1.0),
                          node->GetTupleDouble("surfaceFriction", 1, 1.0));

  // Apply a texture
  textureFile = node->SearchFilename("textureFile", GAZEBO_TEXTURES_PATH, NULL);
  if (textureFile)
  {
    PRINT_MSG1(1, "loading texture file [%s]", textureFile);  
    if (this->geom->SetTexture2DFile(textureFile) != 0)
    {
      PRINT_ERR("unable to load texture file");
      return -1;
    }
  }

  // Apply a skin
  skinFile = node->SearchFilename("skinFile", GAZEBO_SKINS_PATH, NULL);
  if (skinFile)
  {
    PRINT_MSG1(1, "loading skin file [%s]", skinFile);  
    if (this->geom->SetSkinFile(skinFile) != 0)
    {
      PRINT_ERR("unable to load skin file");
      return -1;
    }
  }
    
  // Skin geometry
  GzVector pos;
  GzQuatern rot;
  pos = node->GetPosition("skinXyz", GzVectorZero());
  rot = node->GetRotation("skinRpy", GzQuaternIdent());  
  this->geom->SetSkinPose(GzPoseSet(pos, rot));
  this->geom->SetSkinScale(node->GetPosition("skinScale", GzVectorSet(1, 1, 1)));

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int SimpleSolid::Init( WorldFile *file, WorldFileNode *node )
{    
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the model
int SimpleSolid::Fini()
{
  return 0;
}



//////////////////////////////////////////////////////////////////////////////
// Update the model
void SimpleSolid::Update( double /*step*/ )
{
  return;
}

//////////////////////////////////////////////////////////////////
// Inicia a geometria a as propriedades do modelo
//////////////////////////////////////////////////////////////////
void SimpleSolid::Start()
{
  const char *geomShape;
  const char *textureFile;
  const char *skinFile;
  
  // Create a body
  this->body = new Body( this->world );
  this->AddBody( this->body, true );

  geomShape = this->GetField("shape")->ToString().c_str();

  this->body->Enable(true);
  this->body->SetGravityMode(true);

  // Create a geom of the correct shape
  if (strcmp(geomShape, "box") == 0)
  {
    double sx, sy, sz;
    //sx = 0.5;
    //sy = 0.5;
    //sz = 0.5;    
    GzVector size = ((GzVectorField*)this->GetField("size"))->GetValue();
    sx = size.x;
    sy = size.y;
    sz = size.z;

    this->geom = new BoxGeom( this->body, this->modelSpaceId, sx, sy, sz );
    this->geom->SetMass(1);
    this->geom->SetRelativePosition(GzVectorSet(0, 0, sz / 2));
  }
  else if (strcmp(geomShape, "cylinder") == 0)
  {
    double len, diam;
    //diam = 0.1;
    //len = 0.5;
    DoubleTuple size = ((DoubleTupleField*)this->GetField("size"))->GetValue();
    diam = size.x;
    len = size.y;
    
    this->geom = new CylinderGeom( this->body, this->modelSpaceId, diam / 2, len );
    this->geom->SetRelativePosition(GzVectorSet(0, 0, len/2));
    this->geom->SetMass(1);
  }
  else if (strcmp(geomShape, "sphere") == 0)
  {
    double diam;
    //diam = 0.5;
    diam = ((DoubleField*)this->GetField("size"))->GetValue();
    
    this->geom = new SphereGeom( this->body, this->modelSpaceId, diam / 2 );
    this->geom->SetMass( 1 );
    this->geom->SetRelativePosition(GzVectorSet(0, 0, diam / 2));
  }
  else
  {
    PRINT_ERR1("unknown shape [%s]", geomShape);
    return;
  }

  this->geom->SetTransparency(false);
  this->geom->SetShadeModel("flat");
  this->geom->SetPolygonMode("fill");

  // Set generic properties
  this->geom->SetColor( GzColor(1.0, 1.0, 1.0) );
  this->geom->SetRetro( 0.0 );
  this->geom->SetFiducial( -1 );
  
  // Surface properties
  this->geom->SetHardness(1e6,0);
  this->geom->SetFriction(1.0,1.0);

  // Apply a texture
//   textureFile = node->SearchFilename("textureFile", GAZEBO_TEXTURES_PATH, NULL);
//   if (textureFile)
//   {
//     PRINT_MSG1(1, "loading texture file [%s]", textureFile);  
//     if (this->geom->SetTexture2DFile(textureFile) != 0)
//     {
//       PRINT_ERR("unable to load texture file");
//       return -1;
//     }
//   }

//   // Apply a skin
//   skinFile = node->SearchFilename("skinFile", GAZEBO_SKINS_PATH, NULL);
//   if (skinFile)
//   {
//     PRINT_MSG1(1, "loading skin file [%s]", skinFile);  
//     if (this->geom->SetSkinFile(skinFile) != 0)
//     {
//       PRINT_ERR("unable to load skin file");
//       return -1;
//     }
//   }
    
  // Skin geometry
  GzVector pos;
  GzQuatern rot;
  pos = GzVectorZero();
  rot = GzQuaternIdent();
  this->geom->SetSkinPose(GzPoseSet(pos, rot));
  this->geom->SetSkinScale(GzVectorSet(1, 1, 1));
}

/////////////////////////////////////////////////////////////////////
// Implementacao de metodos get/set para propriedades com meta-dados
/////////////////////////////////////////////////////////////////////

void SimpleSolid::SetShape(string shape)
{
  // Só permite atribuir um shape apenas uma vez nas outras vezes
  // o shape deve ser igual
  bool bEmptyShape = this->GetShape().empty();
  bool bSameShape = this->GetShape().compare(shape);
  if ( ( !bEmptyShape ) || (!bSameShape) )
  {
    throw new SystemException("Só é permitido atribuir um shape apenas uma vez","SimpleSolid::SetShape");
  }

  // o shape deve ser um box, cylinder ou sphere apenas
  if ( (shape != "cylinder") && (shape != "box") && (shape != "sphere") )
  {
    throw new SystemException("shape inválido valores devem ser 'box', 'cylinder' ou 'sphere'","SimpleSolid::SetShape");
  }

  // Executa o código apenas na primeira vez quando o shape ainda esta vazio
  if ( this->GetShape().empty() )
  {
    // A propriedade size tem possui informação diferente dependendo do shape escolhido
    // Esta propriedade é adicionada ao modelo quando o shape é atribuído
    // pela primeira vez
    if ( shape == "box" )
    {
      this->SetField(new GzVectorField("size",GzVectorSet(0.5,0.5,0.5)));
    }
    else if ( shape == "cylinder" )
    {
      this->SetField(new DoubleTupleField("size",DoubleTupleSet(0.1,0.5)));
    }
    else // if sphere
    {
       this->SetField(new DoubleField("size",0.5));
    }

    // Por fim a informacao do shape é atribuida se ainda nao tiver
    ((StringField*)this->GetField("shape"))->SetValue(shape);
    this->shape = shape;
  }

}

string SimpleSolid::GetShape()
{
  this->shape = ((StringField*)this->GetField("shape"))->GetValue();
  return ((StringField*)this->GetField("shape"))->GetValue();
}


GzColor SimpleSolid::GetColor()
{
  return ((GzColorField*)this->GetField("color"))->GetValue();
}
void SimpleSolid::SetColor(GzColor color)
{
  ((GzColorField*)this->GetField("color"))->SetValue(color);
  this->geom->SetColor(color);
}





void SimpleSolid::SetSize(const void* size)
{
  if ( this->GetShape() == "box" )
  {
    GzVector * value = (GzVector*)size;
    ((GzVectorField*)this->GetField("size"))->SetValue(*value);
    ((BoxGeom*)this->geom)->SetSize(value->x,value->y,value->z);
  }
  else if ( this->GetShape() == "cylinder" )
  {
    DoubleTuple * value = (DoubleTuple*)size;
    ((DoubleTupleField*)this->GetField("size"))->SetValue(*value);
    ((CylinderGeom*)this->geom)->SetSize(value->x,value->y);
  }
  else if ( this->GetShape() == "sphere" )
  {
    double * value = (double*)size;
    ((DoubleField*)this->GetField("size"))->SetValue(*value);
    ((SphereGeom*)this->geom)->SetSize(*value / 2.0);
  }
  else
  {
    throw new SystemException("shape nao atribuido ou desconhecido","SimpleSolid::SetSize");
  }
}

const void* SimpleSolid::GetSize()
{
  if ( this->GetShape() == "box" )
  {
    this->boxSize = ((GzVectorField*)this->GetField("size"))->GetValue();
    return &boxSize;
  }
  else if ( this->GetShape() == "cylinder" )
  {
    this->cylindersize = ((DoubleTupleField*)this->GetField("size"))->GetValue();
    return &cylindersize;
  }
  else if ( this->GetShape() == "sphere" )
  {
     this->sphereSize = ((DoubleField*)this->GetField("size"))->GetValue();
     return &sphereSize;
  }
  else
  {
    throw new SystemException("shape nao atribuido ou desconhecido","SimpleSolid::SetSize");
  }
}

/////////////////////////////////////////////////////////////////////


