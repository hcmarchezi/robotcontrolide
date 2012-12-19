
#include "SystemException.h"

SystemException::SystemException(const char * strMessage,const char * strMethod):Exception(strMessage)
{
  this->strMessage = "";
  this->strMessage += strMethod;
  this->strMessage += " : ";
  this->strMessage += strMessage;
}

SystemException::~SystemException(void)
{
}

char const * SystemException::GetErrorMessage()
{
  return this->strMessage.c_str();
}



