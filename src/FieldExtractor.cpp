
#include "FieldExtractor.h"

map<string,Field*> * FieldExtractor::GetFieldsFromModel(Model * model)
{
  map<string,Field*> * fieldMap = new map<string,Field*>();
  map<string,Field*>::iterator iterator;
  // O MAPA DE CAMPOS DEVE ESTAR NUMA VARIAVEL SOB PENA DE TER UMA EXCEÇÃO DO COMPILADOR
  map<string,Field*> modelFields = model->GetFieldsREMOVE();
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

map<string,string> * FieldExtractor::GetStringFieldsFromModel(Model * model)
{
  map<string,string> * stringMap = new map<string,string>();
  map<string,Field*>::iterator iterator;
  map<string,Field*> modelFields = model->GetFieldsREMOVE();
  for (iterator = modelFields.begin(); iterator != modelFields.end(); iterator++)
  {
    (*stringMap)[ (*iterator).first ] = (*iterator).second->ToString();
  }
  return stringMap;
}

void FieldExtractor::SetFieldsToModel(Model * model,map<string, Field*> * fieldMap)
{
  map<string,Field*>::iterator iterator;
  for (iterator = (*fieldMap).begin(); iterator != (*fieldMap).end(); iterator++)
  {
    Field * field_map = (*iterator).second;
    string strFieldName = (*iterator).first;
    model->GetFieldsREMOVE()[ strFieldName ]->SetStringValue( field_map->ToString() );
    if ( model->GetFieldsREMOVE()[ strFieldName ]->ToString() != field_map->ToString() )
    {
      int a = 0;
    }

  }
}

void FieldExtractor::SetFieldsToModel(Model * model,map<string,string> * fieldMap)
{
  map<string,string>::iterator iterator;
  for (iterator = (*fieldMap).begin(); iterator != (*fieldMap).end(); iterator++)
  {
    model->GetFieldsREMOVE()[ (*iterator).first ]->SetStringValue( (*iterator).second );
  }
}

