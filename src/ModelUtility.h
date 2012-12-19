
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
#ifndef MODELUTILITY_H
#define MODELUTILITY_H

#include "Model.hh"
#include "VirtualWorld.h"
#include "RenderOptions.hh"
#include "Vector.hh"
#include "Field.h"
#include <map>

using namespace std;

/**
@author Humberto Cardoso Marchezi
*/
class ModelUtility
{
private: static char strModelTypeName[100];
public: static const char * GetModelTypeName(Model * model);
public: static bool IsModelCamera(Model* model);

private: static vector<string> arrStrModelTypeNames;
public: static void Initialize();
public: static vector<string> GetModelTypeNames();

public: static vector<Model*> * GetChildModels(Model * parent);
public: static void RenderModel(Model * model,int pass,RenderOptions * renderOpt);

private: static GzVector eyePosition;
private: static GzVector centerPosition;
private: static GzVector upVector;
public: static GzVector GetEyePosition(Model * model);
public: static GzVector GetCenterPosition(Model * model);
public: static GzVector GetUpVector(Model * model);

// Extrai os campos de um modelo
public: static map<string,Field*> * GetFieldsFromModel(Model * model);
// Extrai um mapa com valores dos campos no formato string
public: static map<string,string> * GetStringFieldsFromModel(Model * model);
// Atribui os campos a um modelo
// Pre-Condicao: Os campos informados no mapa devem existir no modelo
public: static void SetFieldsToModel(Model * model,map<string, Field*> * fieldMap);
// Atribui uma lista de nomes de campos e valores a um modelo
// Pre-Condicao: Os nomes de campos informadoas no mapa devem corresponder aos
// nomes de campos no modelo
public: static void SetFieldsToModel(Model * model,map<string,string> * fieldMap);

public: static Model * NewModel(World * world,const char * className);
;

};

#endif
