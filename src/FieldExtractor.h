
#ifndef FIELDEXTRACTOR_H
#define FIELDEXTRACTOR_H

#include "Field.h"
#include "Model.hh"
#include <string>
#include <map>

using namespace std;

class FieldExtractor
{
// Extrai os campos de um modelo
private: static map<string,Field*> * GetFieldsFromModel(Model * model);
// Extrai um mapa com valores dos campos no formato string
private: static map<string,string> * GetStringFieldsFromModel(Model * model);
// Atribui os campos a um modelo
// Pre-Condicao: Os campos informados no mapa devem existir no modelo
private: static void SetFieldsToModel(Model * model,map<string, Field*> * fieldMap);
// Atribui uma lista de nomes de campos e valores a um modelo
// Pre-Condicao: Os nomes de campos informadoas no mapa devem corresponder aos
// nomes de campos no modelo
private: static void SetFieldsToModel(Model * model,map<string,string> * fieldMap);
};

#endif