
#include "BoolField.h"

BoolField::BoolField(string name,bool defaultValue):Field(name)
{
   this->defaultValue = defaultValue;
   this->typeName = "bool";
   this->SetValue(defaultValue);
}

BoolField::~BoolField(void)
{
}

bool BoolField::GetValue()
{
  if (this->typeName != "bool")
  {
    throw new SystemException("BoolField::GetValue","Conversão inválida de objeto Field");
  }
  return this->value;
}

void BoolField::SetValue(bool value)
{
  if (this->typeName != "bool")
  {
    throw new SystemException("BoolField::GetValue","Conversão inválida de objeto Field");
  }
  this->value = value;
  this->strValue = Conversion::BoolToString(this->value);
}

string BoolField::ToString()
{  
  return this->strValue;
}

void BoolField::SetStringValue(string value)
{
  this->value = Conversion::StringToBool(value);
  this->strValue = value;
}

bool BoolField::IsDefaultValue()
{
  return ( this->value == this->defaultValue );
}

Field * BoolField::GetCopy()
{
  BoolField * copy = new BoolField(this->GetName(),this->defaultValue);
  copy->SetValue(this->GetValue());
  return copy;
}