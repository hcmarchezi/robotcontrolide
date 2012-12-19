
#include "IntegerField.h"

IntegerField::IntegerField(string name,int defaultValue):Field(name)
{  
  this->defaultValue = defaultValue;
  this->typeName = "integer";
  this->SetValue(defaultValue);
}

IntegerField::~IntegerField(void)
{
}

int IntegerField::GetValue()
{
  if (this->typeName != "integer")
  {
    throw new SystemException("BoolField::GetValue","Conversão inválida de objeto Field");
  }
  return this->value;
}

void IntegerField::SetValue(int value)
{
  if (this->typeName != "integer")
  {
    throw new SystemException("BoolField::GetValue","Conversão inválida de objeto Field");
  }
  this->value = value;
  this->strValue = Conversion::IntegerToString(this->value);
}

string IntegerField::ToString()
{
  return this->strValue;
}

void IntegerField::SetStringValue(string value)
{
  this->value = Conversion::StringToInt(value);
  this->strValue = value;
}

bool IntegerField::IsDefaultValue()
{
  return ( this->value == this->defaultValue );
}

Field * IntegerField::GetCopy()
{
  IntegerField * copy = new IntegerField(this->GetName(),this->defaultValue);
  copy->SetValue(this->GetValue());
  return copy;
}