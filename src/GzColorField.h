
#ifndef GZCOLORFIELD_H
#define GZCOLORFIELD_H

#include "Field.h"
#include "Color.hh"

class GzColorField : public Field
{
public: GzColorField(string name,GzColor defaultValue);
public: virtual ~GzColorField(void);

private: GzColor value;
public:  GzColor GetValue();
public:  void SetValue(GzColor value);

public:  string ToString();
public: void SetStringValue(string value);

private: GzColor defaultValue;
public: bool IsDefaultValue();

public: Field * GetCopy();
};

#endif

