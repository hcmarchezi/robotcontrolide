
#include "IntegerTupleField.h"

IntegerTupleField::IntegerTupleField(string name,IntegerTuple defaultValue):Field(name)
{
  this->defaultValue = defaultValue;
  this->typeName = "IntegerTuple";
  this->SetValue(defaultValue);
}

IntegerTupleField::~IntegerTupleField(void)
{
}

IntegerTuple IntegerTupleField::GetValue()
{
  if (this->typeName != "IntegerTuple")
  {
    throw new SystemException("IntegerTuple::GetValue","Conversão inválida de objeto Field");
  }
  return this->value;
}

void IntegerTupleField::SetValue(IntegerTuple value)
{
  if (this->typeName != "IntegerTuple")
  {
    throw new SystemException("IntegerTuple::SetValue","Conversão inválida de objeto Field");
  }
  this->value = value;
  this->strValue = Conversion::IntegerTupleToString(this->value);
}

string IntegerTupleField::ToString()
{
  return this->strValue;
}

void IntegerTupleField::SetStringValue(string value)
{
  this->value = Conversion::StringToIntegerTuple(value);
  this->strValue = value;
}

bool IntegerTupleField::IsDefaultValue()
{
  return (( this->value.x == this->defaultValue.x )&&
          ( this->value.y == this->defaultValue.y ));
}

Field * IntegerTupleField::GetCopy()
{
  IntegerTupleField * copy = new IntegerTupleField(this->GetName(),this->defaultValue);
  copy->SetValue(this->GetValue());
  return copy;
}
