
#ifndef DOUBLEFIELD_H
#define DOUBLEFIELD_H

#include "Field.h"

class DoubleField : public Field
{
public: DoubleField(string name,double defaultValue);
public: virtual ~DoubleField(void);

private: double value;
public:  double GetValue();
public:  void SetValue(double value);

public:  string ToString();
public: void SetStringValue(string value);

private: double defaultValue;
public: bool IsDefaultValue();

public: Field * GetCopy();
};

#endif

