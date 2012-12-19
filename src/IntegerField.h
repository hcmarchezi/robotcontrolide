
#ifndef INTEGERFIELD_H
#define INTEGERFIELD_H

#include "Field.h"

class IntegerField : public Field
{
public: IntegerField(string name,int defaultValue);
public: virtual ~IntegerField(void);

private: int value;
public:  int GetValue();
public: void SetValue(int value);

public:  string ToString();
public: void SetStringValue(string value);

private: int defaultValue;
public: bool IsDefaultValue();

public: Field * GetCopy();
};

#endif

