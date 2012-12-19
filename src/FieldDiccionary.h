
#ifndef FIELDDICCIONARY
#define FIELDDICCIONARY

#include<map>
#include<string>
#include "Field.h"
#include <vector>

using namespace std;

class FieldDiccionary
{
private: map<string,Field*> mapFields; // Mapa de Fields

// Obtem o field pelo nome informado
protected: Field * GetField(string fieldName);
// Atribui o valor para um field, criando um se não houver
protected: void SetField(Field * field);

// Retorna um mapa de campos
public: map<string,Field*> GetFields();
};

#endif


