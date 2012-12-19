
#ifndef FIELD_H
#define FIELD_H

#include<string>
#include "SystemException.h"
#include "Conversion.h"

using namespace std;

// Esta classe representa um campo de um modelo 3D como um ID,
// position, rotation, color, etc...
// Ao utilizar esta classe ao invés de um campo comum, a aplicacao
// pode  inspecionar um modelo 3D e descobrir o nome e o valor do
// campo
class Field
{
public: Field(string name);
public: virtual ~Field(void);

// Field Name
private: string name;
public: string GetName();
public: void SetName(string name);

// Field Type Name
protected: string typeName;
public: string GetTypeName();

// Converte o valor em string
protected: string strValue;
public: virtual string ToString() = 0;
// Recebe string como valor e converte internamente conforme o tipo
public: virtual void SetStringValue(string value) = 0; 

// Verifica se o valor no campo é o valor default
public: virtual bool IsDefaultValue() = 0;
// Faz uma cópia do campo
public: virtual Field * GetCopy() = 0;
};

#endif


