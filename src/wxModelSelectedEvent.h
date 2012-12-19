
#ifndef WORLDGLCANVASEVENT_H
#define WORLDGLCANVASEVENT_H

#include <wx/wx.h>
#include "Model.hh"

enum Mouse_Message_Type
{
  Mouse_Left_Down=0,
  Mouse_Left_Up,
  Mouse_Left_Dragging,
  Mouse_Right_Down,
  Mouse_Right_Up,
  Mouse_Right_Dragging  
};

class wxModelSelectedEvent : public wxEvent
{
public: Model * model;
//public: bool  bMouseDragging;
public: enum Mouse_Message_Type mouseMessageType;
public: double velocityX;
public: double velocityY;
public: wxString viewContextName; // X/Y, Y/Z, X/Z
public: wxModelSelectedEvent(int id=0,wxEventType commandType = wxEVT_NULL);
public: virtual ~wxModelSelectedEvent(void);
public: wxEvent * Clone(void) const;
};

#endif

