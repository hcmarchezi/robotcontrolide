
/***************************************************************************
 *   Copyright (C) 2007 by Humberto Cardoso Marchezi   *
 *   hcmarchezi@linux   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//////////////////// DEBUG////////////////
//#include <wx/string.h>
//#include <wx/msgdlg.h>
#include <wx/wx.h>
/////////////////////////////////////////


#include "ModelUtility.h"
#include <typeinfo>
#include <string>

/////////////////////// Includes de cada Modelo suportado pela aplicacao
#include "CanonVCC4.hh"
#include "GarminGPS.hh"
#include "GroundPlane.hh"
#include "LightSource.hh"
#include "MonoCam.hh"
#include "ObserverCam.hh"
#include "Pioneer2AT.hh"
#include "Pioneer2DX.hh"
#include "Pioneer2Gripper.hh"
#include "SickLMS200.hh"
#include "SimpleSolid.hh"
#include "SonyVID30.hh"
#include "SkyDome.hh" 
/////////////////////////////////////////////////////////////////

#include "WorldFile.hh"

#include "GzVectorField.h"






using namespace std;

char ModelUtility::strModelTypeName[100] = "";
vector<string> ModelUtility::arrStrModelTypeNames;

GzVector ModelUtility::eyePosition;
GzVector ModelUtility::centerPosition;
GzVector ModelUtility::upVector;

const char * ModelUtility::GetModelTypeName(Model * model)
{
  string strFullTypeName = typeid(*model).name();
  
  char cTypeName = strFullTypeName[0];
  int index = 0;
  while ((cTypeName>='0')&&(cTypeName<='9'))
  {
    string strTemp = strFullTypeName.substr(1);
    strFullTypeName = strTemp; 
    cTypeName = strFullTypeName[0];
  }
  string strType = strFullTypeName;
  
  // Codigo valido apenas no Windows
  //string strType = strFullTypeName.substr(6);

  char * strDestiny = ModelUtility::strModelTypeName;

  strcpy( strDestiny , strType.c_str() );
  
	return strDestiny;
}

bool ModelUtility::IsModelCamera(Model* model)
{
  const char * name = ModelUtility::GetModelTypeName(model);
  if (( strcmp(name,"ObserverCam") == 0 ) || 
      ( strcmp(name,"MonoCam")     == 0 ) ||
      ( strcmp(name,"CanonVCC4")   == 0 ) ||
      ( strcmp(name,"SonyVID30")   == 0 ))
  {
    return true;
  }
  return false;
}

void ModelUtility::Initialize()
{
    ModelUtility::arrStrModelTypeNames.clear();
    ModelUtility::arrStrModelTypeNames.push_back("CanonVCC4");
    ModelUtility::arrStrModelTypeNames.push_back("GarminGPS");
    ModelUtility::arrStrModelTypeNames.push_back("GroundPlane");
    ModelUtility::arrStrModelTypeNames.push_back("LightSource");
    ModelUtility::arrStrModelTypeNames.push_back("MonoCam");
    ModelUtility::arrStrModelTypeNames.push_back("ObserverCam");
    ModelUtility::arrStrModelTypeNames.push_back("Pioneer2AT");
    ModelUtility::arrStrModelTypeNames.push_back("Pioneer2DX");
    ModelUtility::arrStrModelTypeNames.push_back("Pioneer2Gripper");
    ModelUtility::arrStrModelTypeNames.push_back("SickLMS200");
    ModelUtility::arrStrModelTypeNames.push_back("Box");
    ModelUtility::arrStrModelTypeNames.push_back("Cylinder");
    ModelUtility::arrStrModelTypeNames.push_back("Sphere");
    //ModelUtility::arrStrModelTypeNames.push_back("SimpleSolid");
    ModelUtility::arrStrModelTypeNames.push_back("SonyVID30");
    ModelUtility::arrStrModelTypeNames.push_back("SkyDome");
}

vector<string> ModelUtility::GetModelTypeNames()
{
  return ModelUtility::arrStrModelTypeNames;  
}

vector<Model*> * ModelUtility::GetChildModels(Model * parent)
{
	// Obtem todos os modelos do mundo 	
        //Model** arrModels = parent->virtualWorld->GetModels();
        //int numModels     = parent->virtualWorld->GetNumModels();
        Model** arrModels = parent->world->GetModels();
        int numModels     = parent->world->GetNumModels();

	// Array de modelos filho
	vector<Model*> * childModels = new vector<Model*>();

	// Retorna apenas aqueles cujo pai é o 
	// modelo informado
	for (unsigned int i=0; i < numModels; i++)
	{
		//Model * model = (*arrModels)[i];
                Model * model = arrModels[i];
		if (model->parent == parent)
		{
			childModels->push_back(model);
		}
	}

	return childModels;
}

void ModelUtility::RenderModel(Model * model,int pass,RenderOptions * renderOpt)
{
  if (model->parent == NULL)
  {
    // Renderiza o modelo principal
    model->Render(pass,renderOpt);
    // Renderiza os modelos filhos
    vector<Model*> * childModels = ModelUtility::GetChildModels(model);

    glPushMatrix();

    // Obtem a posicao global do modelo
    GzPose pose = model->GetPose();

    // Apply translation
    GzVector pos = pose.pos;
    glTranslatef(pos.x, pos.y, pos.z);

    // Apply rotation
    GzQuatern rot = GzQuaternToAxis(pose.rot);
    glRotatef(rot.u * 180 / M_PI, rot.x, rot.y, rot.z);
	
    for (unsigned int i=0; i < childModels->size(); i++)
    {
      (*childModels)[i]->Render(pass,renderOpt);
    }

    glPopMatrix();

    delete childModels;	
  }
}

GzVector ModelUtility::GetEyePosition(Model * model)
{
  if (! ModelUtility::IsModelCamera(model) )
  {
    throw new SystemException("Modelo não representa uma camera","ModelUtility::GetEyePosition");
  }

  ModelUtility::eyePosition = model->GetPose().pos;
  return ModelUtility::eyePosition;   
}

GzVector ModelUtility::GetCenterPosition(Model * model)
{
  if (! ModelUtility::IsModelCamera(model) )
  {
    throw new SystemException("Modelo não representa uma camera","ModelUtility::GetEyePosition");
  }

  GzQuatern cam_rot = model->GetPose().rot;
  GzVector a = GzVectorSet(1,0,0);  
  a = GzCoordPositionAdd(a,model->GetPose().pos,cam_rot);

  ModelUtility::centerPosition = a;
  return ModelUtility::centerPosition;
}

GzVector ModelUtility::GetUpVector(Model * model)
{
  if (! ModelUtility::IsModelCamera(model) )
  {
    throw new SystemException("Modelo não representa uma camera","ModelUtility::GetEyePosition");
  }

  GzQuatern cam_rot = model->GetPose().rot;
  GzVector b = GzVectorSet(1,0,1);
  b = GzCoordPositionAdd(b,model->GetPose().pos,cam_rot);

  // Calcula center position
  GzVector centerPosition = GzVectorSet(1,0,0);  
  centerPosition = GzCoordPositionAdd(centerPosition,model->GetPose().pos,cam_rot);

  b = GzVectorSub(b, centerPosition);

  ModelUtility::upVector = b;
  return ModelUtility::upVector;
}

map<string,Field*> * ModelUtility::GetFieldsFromModel(Model * model)
{
  map<string,Field*> * fieldMap = new map<string,Field*>();
  map<string,Field*>::iterator iterator;
  // O MAPA DE CAMPOS DEVE ESTAR NUMA VARIAVEL SOB PENA DE TER UMA EXCEÇÃO DO COMPILADOR
  map<string,Field*> modelFields = model->GetFields();
  for (iterator = modelFields.begin(); iterator != modelFields.end(); iterator++)
  {
    string strFieldName = (*iterator).first;
    if ( strFieldName == "color" )
    {
      int c = 0;
    }
    Field *       field = (*iterator).second;
    Field *  copy_field = field->GetCopy();
    if ( copy_field->ToString() != field->ToString() )
    {
      int b = 0;
    }
    (*fieldMap)[ strFieldName ] = copy_field;

  }
  return fieldMap;
}

map<string,string> * ModelUtility::GetStringFieldsFromModel(Model * model)
{
  map<string,string> * stringMap = new map<string,string>();
  map<string,Field*>::iterator iterator;
  map<string,Field*> modelFields = model->GetFields();
  for (iterator = modelFields.begin(); iterator != modelFields.end(); iterator++)
  {
    (*stringMap)[ (*iterator).first ] = (*iterator).second->ToString();
  }
  return stringMap;
}

void ModelUtility::SetFieldsToModel(Model * model,map<string, Field*> * fieldMap)
{
  map<string,Field*>::iterator iterator;
  for (iterator = (*fieldMap).begin(); iterator != (*fieldMap).end(); iterator++)
  {
    Field * field_map = (*iterator).second;
    string strFieldName = (*iterator).first;
    model->GetFields()[ strFieldName ]->SetStringValue( field_map->ToString() );
  }
}

void ModelUtility::SetFieldsToModel(Model * model,map<string,string> * fieldMap)
{
  map<string,string>::iterator iterator;
  for (iterator = (*fieldMap).begin(); iterator != (*fieldMap).end(); iterator++)
  {
    model->GetFields()[ (*iterator).first ]->SetStringValue( (*iterator).second );
    string strFieldName = (*iterator).first;
    if (strFieldName == "id")
    {
      model->SetId( (*iterator).second.c_str() );
    }
    else if (strFieldName == "xyz")
    {
      GzPose pose = model->GetPose();
      pose.pos =  Conversion::StringToGzVector( (*iterator).second );
      model->SetPose(pose);
    }
    else if (strFieldName == "rpy")
    {
      GzVector euler = Conversion::StringToGzVector( (*iterator).second );
      GzPose pose = model->GetPose();
      euler.x = M_PI * euler.x / 180.0;
      euler.y = M_PI * euler.y / 180.0;
      euler.z = M_PI * euler.z / 180.0;

      pose.rot = GzQuaternFromEuler(euler.x,euler.y,euler.z);
      model->SetPose(pose);
    }
    else if (strFieldName == "shape")
    {
      string originalShape = ((SimpleSolid*)model)->GetShape();
      string newShape      = (*iterator).second;
      if ( originalShape != newShape )
      {
        throw new SystemException("Propriedade shape nao pode ser alterada","ModelUtility::SetFieldsToModel");
      }
    }
    else if (strFieldName == "color")
    {
      SimpleSolid * solid = (SimpleSolid*)model;
      solid->SetColor( Conversion::StringToGzColor( (*iterator).second ) );
    }
    else if (strFieldName == "size")
    {
      SimpleSolid * solid = (SimpleSolid*)model;
      const void * size = solid->GetSize();
      if (solid->GetShape() == "box")
      {
        GzVector * boxSize = (GzVector*)size;
        (*boxSize) = Conversion::StringToGzVector( (*iterator).second );
      }
      else if (solid->GetShape() == "cylinder")
      {
        DoubleTuple * cylinderSize = (DoubleTuple*)size;
        (*cylinderSize) = Conversion::StringToDoubleTuple( (*iterator).second );
      }
      else if (solid->GetShape() == "sphere")
      {
        double * sphereSize = (double*)size;
        (*sphereSize) = Conversion::StringToDouble( (*iterator).second );
      }
      else
      {
        throw new SystemException("Shape invalido","ModelUtility::SetFieldsToModel");
      }
      solid->SetSize(size);
    }
  }
}


Model * ModelUtility::NewModel(World * world,const char * className)
{
  Model * model = NULL;

  if (strcmp(className,"CanonVCC4")==0)
  {
    model = new CanonVCC4(world);
    ((CanonVCC4*)model)->Start();
  }
  else if (strcmp(className,"GarminGPS")==0)
  {
    model = new GarminGPS(world);
    ((GarminGPS*)model)->Start();
  }
  else if (strcmp(className,"GroundPlane")==0)
  {
    model = new GroundPlane(world);
    ((GroundPlane*)model)->Start();
  }
  else if (strcmp(className,"LightSource")==0)
  {
    model = new LightSource(world);
    ((LightSource*)model)->Start();
  }
  else if (strcmp(className,"MonoCam")==0)
  {
    model = new MonoCam(world);
    ((MonoCam*)model)->Start();
  }
  else if (strcmp(className,"ObserverCam")==0)
  {
    model = new ObserverCam(world);
    ((ObserverCam*)model)->Start();
  }
  else if (strcmp(className,"Pioneer2AT")==0)
  {
    model = new Pioneer2AT(world);
   ((Pioneer2AT*)model)->Start();
  }
  else if (strcmp(className,"Pioneer2DX")==0)
  {
    model = new Pioneer2DX(world);
    ((Pioneer2DX*)model)->Start();
  }
  else if (strcmp(className,"Pioneer2Gripper")==0)
  {
    model = new Pioneer2Gripper(world);
    ((Pioneer2Gripper*)model)->Start();
  }
  else if (strcmp(className,"SickLMS200")==0)
  {
    model = new SickLMS200(world);
    ((SickLMS200*)model)->Start();
  }
//   // Ainda deve permanecer esta opcao para o caso de carregar o modelo pelo arquivo
//   else if (strcmp(className,"SimpleSolid")==0)
//   {
//     model = new SimpleSolid(world);
//     ((SimpleSolid*)model)->Start();
//   }
  // As opcoes box, cylinder e sphere sao para quando o modelo eh criado no programa
  else if (strcmp(className,"Box")==0)
  {
    model = new SimpleSolid(world,"box");
    ((SimpleSolid*)model)->Start();
  }
  else if (strcmp(className,"Cylinder")==0)
  {
    model = new SimpleSolid(world,"cylinder");
    ((SimpleSolid*)model)->Start();
  }
  else if (strcmp(className,"Sphere")==0)
  {
    model = new SimpleSolid(world,"sphere");
    ((SimpleSolid*)model)->Start();
  }

  else if (strcmp(className,"SonyVID30")==0)
  {
    model = new SonyVID30(world);
    ((SonyVID30*)model)->Start();
  }
  else if (strcmp(className,"SkyDome")==0)
  {
    model = new SkyDome(world);
    ((SkyDome*)model)->Start();
  }
  else if (model == NULL)
  {
    throw SystemException("Modelo informado não existe ou não foi implementado.","ModelUtility::NewModel");
  }

  return model;
}



