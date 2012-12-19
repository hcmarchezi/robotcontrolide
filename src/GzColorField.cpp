
#include "GzColorField.h"

GzColorField::GzColorField(string name,GzColor defaultValue):Field(name)
{  
  this->defaultValue = defaultValue;
  this->typeName = "GzColor";
  this->SetValue(defaultValue);
}

GzColorField::~GzColorField(void)
{
}

GzColor GzColorField::GetValue()
{
  if (this->typeName != "GzColor")
  {
    throw new SystemException("GzColorField::GetValue","Conversão inválida de objeto Field");
  }
  return this->value;
}

void GzColorField::SetValue(GzColor value)
{
  if (this->typeName != "GzColor")
  {
    throw new SystemException("GzColorField::GetValue","Conversão inválida de objeto Field");
  }
  this->value = value;
  this->strValue = Conversion::GzColorToString(this->value);
}

string GzColorField::ToString()
{
  return this->strValue;
}

void GzColorField::SetStringValue(string value)
{
  this->value = Conversion::StringToGzColor(value);
  this->strValue = value;
}

bool GzColorField::IsDefaultValue()
{
  return ( this->value == this->defaultValue );
}

Field * GzColorField::GetCopy()
{
  GzColorField * copy = new GzColorField(this->GetName(),this->defaultValue);
  copy->SetValue(this->GetValue());
  return copy;
}