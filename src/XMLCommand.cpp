
#include "XMLCommand.h"
#include "ModelFactory.hh"
#include "ModelUtility.h"
#include "ApplicationException.h"
#include "FieldExtractor.h"

#include "SimpleSolid.hh" // Como este modelo é uma aberracao tenho que botar aqui
//#include "TestModelFields.h"

TiXmlElement *  XMLCommand::Serialize(Model * model)
{
  //string strModelType = ModelFactory::GetModelTypeName(model);  
  string strModelType = ModelUtility::GetModelTypeName(model);
  if ((strModelType == string("Box"))||(strModelType == string("Cylinder"))||(strModelType == string("Sphere")))
  {
    strModelType = "SimpleSolid";
  }
  string strModelTag = string("model:") + strModelType;

  TiXmlElement * xmlElement = new TiXmlElement(strModelTag.c_str());

  // Serializa cada propriedade do modelo  
  map<string,Field*>::iterator iterator;  
  
  // O MAPA DE CAMPOS DEVE ESTAR NUMA VARIAVEL SOB PENA DE TER UMA EXCEÇÃO DO COMPILADOR
  //map<string,Field*> * modelFields = FieldExtractor::GetFieldsFromModel(model);  
  map<string,Field*> * modelFields = ModelUtility::GetFieldsFromModel(model);

  for (iterator = (*modelFields).begin(); iterator != (*modelFields).end(); iterator++)
  {
    Field * field = (*iterator).second;
    if (! field->IsDefaultValue() )
    {
      TiXmlElement * xmlModelElement = new TiXmlElement( field->GetName().c_str() );
      TiXmlText * xmlModelValue = new TiXmlText( field->ToString().c_str() );
      xmlModelElement->LinkEndChild( xmlModelValue );
      xmlElement->LinkEndChild( xmlModelElement );
    }
  }

  for (iterator = (*modelFields).begin(); iterator != (*modelFields).end(); iterator++)
  {
    delete (*modelFields)[ (*iterator).first ];
  }

  delete modelFields;

  // Serializa os sub-modelos do modelo
  vector<Model *> * arrSubModels = ModelUtility::GetChildModels(model); //model->GetModels();
  for (unsigned int j=0; j < arrSubModels->size(); j++)
  {
    TiXmlElement * xmlSubModelElement = XMLCommand::Serialize((*arrSubModels)[j]);
    xmlElement->LinkEndChild(xmlSubModelElement);
  }
  delete arrSubModels;

  return xmlElement;
}


Model *  XMLCommand::Deserialize(TiXmlElement * modelElement,VirtualWorld * virtualWorld)
{
  // Instancia o tipo de modelo correspondente de acordo com o arquivo XML
  string strValue = modelElement->Value();
  string strModelType = strValue.substr(6,strValue.size()-6); 
  // Se o model for SimpleSolid
  Model * model = NULL;

  if (strModelType == "SimpleSolid")
  {
    // Obtem o valor do shape para atribuir diretamente ao modelo
    TiXmlElement * simpleSolidElement = modelElement->FirstChildElement("shape");
    const char * strValue = simpleSolidElement->GetText();
    string shape = "";
    if (strValue != NULL)
    {
      shape = strValue;
    }
    
    if (shape == "box") shape = "Box";
    else if (shape == "cylinder") shape = "Cylinder";
    else if (shape == "sphere") shape = "Sphere";
    
    model = ModelUtility::NewModel( virtualWorld, shape.c_str() );
  }
  else
  {
    model = ModelUtility::NewModel( virtualWorld, strModelType.c_str() );
  }
  
  //model->virtualWorld = virtualWorld;
 
  // Obtem a COPIA do mapa de propriedades do modelo
  map<string,string> * modelFields = ModelUtility::GetStringFieldsFromModel(model);

  // Percorre todos os campos do modelo
  TiXmlElement * modelFieldElement = modelElement->FirstChildElement();
  while ( modelFieldElement != NULL )
  {
    string strFieldName  = modelFieldElement->Value();
    const char * strValue = modelFieldElement->GetText();
    string strFieldValue = "";
    if (strValue != NULL)
    {
      strFieldValue = strValue;
    }

    int index = strFieldName.find("model:");

    // Se eh propriedade, poe o valor ( em string )
    if (index == -1)   //(*modelFields)[strFieldName] != NULL )
    {
      (*modelFields)[strFieldName] = strFieldValue;
    }   
    // Se o campo for um outro model, o metodo Deserialize deve ser 
    // usado no sub-modelo
    else if ( index > -1 )
    {
      Model * subModel = XMLCommand::Deserialize(modelFieldElement,virtualWorld);
      //model->AddModel(subModel);
      subModel->parent = model;
      virtualWorld->Add3DModel(subModel);
    }
    else
    {
//       map<string,string>::iterator ioterator;
//       for (iterator = (*modelFields).begin(); iterator != (*modelFields).end(); iterator++)
//       {
//         delete (*modelFields)[ (*iterator).first ];
//       }
      delete modelFields;
      throw new SystemException(string( string("O campo ") + strFieldName + string(" não existe ou não foi encontrado") ).c_str(),"XMLCommand::Deserialize");
    }     
    modelFieldElement = modelFieldElement->NextSiblingElement();
  }

  // Atribui os valores do mapa de propriedades de volta ao modelo
  ModelUtility::SetFieldsToModel(model,modelFields);

  // Deleta o mapa de propriedades da memoria
  map<string,string>::iterator iterator;
//   for (iterator = (*modelFields).begin(); iterator != (*modelFields).end(); iterator++)
//   {
//     delete (*modelFields)[ (*iterator).first ];
//   }
  delete modelFields;

  return model;
}