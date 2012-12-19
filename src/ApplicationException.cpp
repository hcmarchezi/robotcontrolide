#include "ApplicationException.h"

ApplicationException::ApplicationException(const char * strMessage):Exception(strMessage)
{
  //this->strErrorMessage = strMessage;
}

ApplicationException::~ApplicationException(void)
{
}

const char * ApplicationException::GetErrorMessage()
{
  return this->strMessage.c_str();
}

