
#ifndef APPLICATIONEXCEPTION_H
#define APPLICATIONEXCEPTION_H
//
//#include <exception>
#include "Exception.h"
#include <string>

using namespace std;

class ApplicationException : public Exception
{
//private: string strErrorMessage;
public: ApplicationException(const char * strMessage);
public: virtual ~ApplicationException(void);
public: virtual const char * GetErrorMessage();
};

#endif

