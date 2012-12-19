
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

#include <exception>

using namespace std;

class Exception 
{
protected: string strMessage;

public: Exception(const char * strMessage);
public: virtual ~Exception(void);
public: virtual const char * GetErrorMessage()=0;
};

#endif

