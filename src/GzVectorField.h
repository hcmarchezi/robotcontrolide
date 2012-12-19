
#ifndef GZVECTORFIELD_H
#define GZVECTORFIELD_H

#include "Field.h"
#include "Vector.hh"

class GzVectorField : public Field
{ 
public: GzVectorField(string name,GzVector defaultValue);
public: virtual ~GzVectorField(void);

private: GzVector value;
public:  GzVector GetValue();
public:  void SetValue(GzVector value);

public:  string ToString();
public: void SetStringValue(string value);

private: GzVector defaultValue;
public: bool IsDefaultValue();

public: Field * GetCopy();
};

#endif

