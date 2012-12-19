
#include "DoubleTupleField.h"

DoubleTupleField::DoubleTupleField(string name,DoubleTuple defaultValue):Field(name)
{  
  this->defaultValue = defaultValue;
  this->typeName = "DoubleTuple";
  this->SetValue(defaultValue);
}

DoubleTupleField::~DoubleTupleField(void)
{
}

DoubleTuple & DoubleTupleField::GetValue()
{
  if (this->typeName != "DoubleTuple")
  {
    throw new SystemException("DoubleTupleField::GetValue","Conversão inválida de objeto Field");
  }
  return this->value;
}

void DoubleTupleField::SetValue(DoubleTuple value)
{
  if (this->typeName != "DoubleTuple")
  {
    throw new SystemException("DoubleTupleField::SetValue","Conversão inválida de objeto Field");
  }
  this->value = value;
  this->strValue = Conversion::DoubleTupleToString(this->value);
}

string DoubleTupleField::ToString()
{
  return this->strValue;
}

void DoubleTupleField::SetStringValue(string value)
{
  this->value = Conversion::StringToDoubleTuple(value);
  this->strValue = value;
}

bool DoubleTupleField::IsDefaultValue()
{
  return (( this->value.x == this->defaultValue.x ) &&
          ( this->value.y == this->defaultValue.y ));
}

DoubleTupleField * DoubleTupleField::GetCopy()
{
  DoubleTupleField * copy = new DoubleTupleField(this->GetName(),this->defaultValue);
  copy->SetValue(this->GetValue());
  return copy;
}