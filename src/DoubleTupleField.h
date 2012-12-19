
#ifndef DOUBLETUPLEFIELD_H
#define DOUBLETUPLEFIELD_H

#include "Field.h"

#include "DoubleTuple.h"

class DoubleTupleField : public Field
{
public: DoubleTupleField(string name,DoubleTuple defaultValue);
public: virtual ~DoubleTupleField(void);

private: DoubleTuple value;
public:  DoubleTuple & GetValue();
public:  void SetValue(DoubleTuple value);

public:  string ToString();
public: void SetStringValue(string value);

private: DoubleTuple defaultValue;
public: bool IsDefaultValue();

public: DoubleTupleField * GetCopy();
};

#endif

