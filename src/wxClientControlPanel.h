
#ifndef WXCLIENTCONTROLPANEL_H
#define WXCLIENTCONTROLPANEL_H

#include <wx/wx.h>
/////////////////////////////#include "wx/wxscintilla.h"

#include "PlayerControl.h"

class wxClientControlPanel : public wxPanel
{
public: wxTextCtrl* txtClientControl;
//public: wxButton* btCompile;
//public: wxButton* btRunSimulation;
//public: wxButton* btRunControl;
public: wxTextCtrl* txtOutput;
private: PlayerControl * playerControl;

public: wxClientControlPanel(wxWindow* parent,wxWindowID id,PlayerControl * playerControl);
public: virtual ~wxClientControlPanel(void);
private: void do_layout();
public: PlayerControl * GetPlayerControl();
private: void On_Text_Updated(wxCommandEvent& e);
};

#endif

