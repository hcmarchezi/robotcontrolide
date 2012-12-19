
#include "StringField.h"

StringField::StringField(string name,string defaultValue):Field(name)
{   
   this->defaultValue = defaultValue;
   this->typeName = "string";
   this->SetValue(value);
}

StringField::~StringField(void)
{
}

string StringField::GetValue()
{
  if (this->typeName != "string")
  {
    throw new SystemException("StringField::GetValue","Conversão inválida de objeto Field");
  }
  return this->value;
}

void StringField::SetValue(string value)
{
  if (this->typeName != "string")
  {
    throw new SystemException("StringField::GetValue","Conversão inválida de objeto Field");
  }
  this->value = value;
  this->strValue = value;
}

string StringField::ToString()
{
  return this->value;
}

void StringField::SetStringValue(string value)
{
  this->value = value;
  this->strValue = value;
}

bool StringField::IsDefaultValue()
{
  return ( this->value == this->defaultValue );
}

Field * StringField::GetCopy()
{
  StringField * copy = new StringField(this->GetName(),this->defaultValue);
  copy->SetValue(this->GetValue());
  return copy;
}
