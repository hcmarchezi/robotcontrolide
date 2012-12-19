
#include "FieldDiccionary.h"
#include "SystemException.h"

Field * FieldDiccionary::GetField(string fieldName)
{
  Field * field = this->mapFields[fieldName]; 
  return field;
}

void FieldDiccionary::SetField(Field * field)
{
  this->mapFields[field->GetName()] = field;
}

map<string,Field*> FieldDiccionary::GetFields()
{
  return this->mapFields;
}

