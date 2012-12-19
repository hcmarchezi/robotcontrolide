
#include "ExceptionHandler.h"

#include "SystemException.h"
#include "ApplicationException.h"

#include <wx/wx.h>

#include <typeinfo>

void ExceptionHandler::Handler(Exception * e)
{
  string strFullTypeName = typeid(*e).name();
  string strErrorMessage = "";

  if (strFullTypeName == "exception")
  {
    strErrorMessage = ((exception*)e)->what();
  }
  else
  {
    strErrorMessage = ((Exception*)e)->GetErrorMessage();
  }
  wxMessageDialog messageBox(NULL, wxT(strErrorMessage.c_str()), wxT("Mensagem de Erro"), wxOK );
  messageBox.ShowModal();
  delete e;
}

