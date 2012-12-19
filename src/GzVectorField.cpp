
#include "GzVectorField.h"

GzVectorField::GzVectorField(string name,GzVector defaultValue):Field(name)
{   
   this->defaultValue = defaultValue;
   this->typeName = "GzVector";
   this->SetValue(defaultValue);
}

GzVectorField::~GzVectorField(void)
{
}

GzVector GzVectorField::GetValue()
{
  if (this->typeName != "GzVector")
  {
    throw new SystemException("GzVectorField::GetValue","Conversão inválida de objeto Field");
  }
  return this->value;
}

void GzVectorField::SetValue(GzVector value)
{
  if (this->typeName != "GzVector")
  {
    throw new SystemException("GzVectorField::GetValue","Conversão inválida de objeto Field");
  }
  this->value = value;
  this->strValue = Conversion::GzVectorToString(this->value);
}

string GzVectorField::ToString()
{
  return this->strValue;
}

void GzVectorField::SetStringValue(string value)
{
  this->value = Conversion::StringToGzVector(value);
  this->strValue = value;
}

bool GzVectorField::IsDefaultValue()
{
  return  (( this->value.x == this->defaultValue.x ) &&
           ( this->value.y == this->defaultValue.y ) &&
           ( this->value.z == this->defaultValue.z ));
}

Field * GzVectorField::GetCopy()
{
  GzVectorField * copy = new GzVectorField(this->GetName(),this->defaultValue);
  copy->SetValue(this->GetValue());
  return copy;
}