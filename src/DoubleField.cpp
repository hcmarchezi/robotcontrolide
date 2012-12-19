
#include "DoubleField.h"

DoubleField::DoubleField(string name,double defaultValue):Field(name)
{

  this->defaultValue = defaultValue;
  this->typeName = "double";
  this->SetValue(defaultValue);
}

DoubleField::~DoubleField(void)
{
}

double DoubleField::GetValue()
{
  if (this->typeName != "double")
  {
    throw new SystemException("DoubleField::GetValue","Conversão inválida de objeto Field");
  }
  return this->value;
}

void DoubleField::SetValue(double value)
{
  if (this->typeName != "double")
  {
    throw new SystemException("DoubleField::GetValue","Conversão inválida de objeto Field");
  }
  this->value = value;
  this->strValue = Conversion::DoubleToString(this->value);
}

string DoubleField::ToString()
{  
  return this->strValue;
}

void DoubleField::SetStringValue(string value)
{
  this->value = Conversion::StringToDouble(value);
  this->strValue = value;
}

bool DoubleField::IsDefaultValue()
{
  return ( this->value == this->defaultValue );
}

Field * DoubleField::GetCopy()
{
  DoubleField * copy = new DoubleField(this->GetName(),this->defaultValue);
  copy->SetValue(this->GetValue());
  return copy;
}