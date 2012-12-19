
#include "wxModelSelectedEvent.h"

wxModelSelectedEvent::wxModelSelectedEvent(int id,wxEventType commandType):wxEvent(id,commandType)
{
  this->model = NULL;
//  this->bMouseDragging = false;
}

wxModelSelectedEvent::~wxModelSelectedEvent(void)
{
}

wxEvent * wxModelSelectedEvent::Clone(void) const
{
  wxModelSelectedEvent * clone = new wxModelSelectedEvent();
  clone->model = this->model;
//  clone->bMouseDragging = this->bMouseDragging;
  clone->mouseMessageType = this->mouseMessageType;
  return clone;
}
