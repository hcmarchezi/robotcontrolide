
#ifndef INTEGERTUPLEFIELD_H
#define INTEGERTUPLEFIELD_H

#include "Field.h"

#include "IntegerTuple.h"

class IntegerTupleField : public Field
{
public: IntegerTupleField(string name,IntegerTuple defaultValue);
public: virtual ~IntegerTupleField(void);

private: IntegerTuple value;
public:  IntegerTuple GetValue();
public:  void SetValue(IntegerTuple value);

public:  string ToString();
public: void SetStringValue(string value);

private: IntegerTuple defaultValue;
public: bool IsDefaultValue();

public: Field * GetCopy();
};

#endif

