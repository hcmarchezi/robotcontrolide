
#include "Field.h"

Field::Field(string name)
{
  this->name = name;
}

Field::~Field(void)
{
}

string Field::GetName()
{
  return this->name;
}

void Field::SetName(string name)
{
  this->name = name;
}

string Field::GetTypeName()
{
  return this->typeName;
}
