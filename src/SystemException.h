
#ifndef SYSTEMEXCEPTION_H
#define SYSTEMEXCEPTION_H

//#include <exception> 
#include "Exception.h"
#include <string>

using namespace std;

class SystemException : public Exception
{
//private: string strErrorMessage;
public:  SystemException(const char * strMessage,const char * strMethod);
public:  virtual ~SystemException(void);
public:  virtual const char * GetErrorMessage();
};

#endif


