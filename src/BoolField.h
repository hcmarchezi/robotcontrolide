
#ifndef BOOLFIELD_H
#define BOOLFIELD_H

#include "Field.h"

class BoolField : public Field
{
public: BoolField(string name,bool defaultValue);
public: virtual ~BoolField(void);

private: bool value;
public:  bool GetValue();
public:  void SetValue(bool value);

public: string ToString();
public: void SetStringValue(string value);

private: bool defaultValue;
public: bool IsDefaultValue();

public: Field * GetCopy();
};

#endif

