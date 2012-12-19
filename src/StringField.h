
#ifndef STRINGFIELD_H
#define STRINGFIELD_H

#include "Field.h"

class StringField : public Field
{
public: StringField(string name,string defaultValue);
public: virtual ~StringField(void);

private: string value;
public:  string GetValue();
public:  void SetValue(string value);

public:  string ToString();
public: void SetStringValue(string value);

private: string defaultValue;
public: bool IsDefaultValue();

public: Field * GetCopy();
};

#endif


