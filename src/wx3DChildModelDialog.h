#ifndef WX3DCHILDMODELDIALOG_H
#define WX3DCHILDMODELDIALOG_H

#include <wx/wx.h>
#include <wx/image.h>
#include "Vector.hh"
#include "Model.hh"

class wx3DChildModelDialog : public wxDialog
{
private:
  wxStaticText* lb3DParentModelID;
  wxComboBox* cmbbx3DParentModelID;
  wxStaticText* lb3DChildModelType;
  wxComboBox* cmbbx3DChildModelType;
  wxStaticText* lb3DChildModelID;
  wxTextCtrl* txt3DChildModelID;
  wxStaticText* lbPositionX;
  wxTextCtrl* txtPositionX;
  wxStaticText* lbPositionY;
  wxTextCtrl* txtPositionY;
  wxStaticText* lbPositionZ;
  wxTextCtrl* txtPositionZ;
  wxButton* btAdd;
  wxButton* btCancel;
  bool bButtonAddPressed;

public: 
  wx3DChildModelDialog(VirtualWorld * virtualWorld, wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_DIALOG_STYLE);
  Model * Get3DParentModel();
  wxString Get3DChildModelType();
  GzVector Get3DChildModelPosition();
  wxString Get3DChildModelID();
  bool IsButtonAddPressed();

private:
  VirtualWorld * virtualWorld;
  void set_properties();
  void do_layout();
  void On_BtAdd_Clicked(wxCommandEvent& e);
  void On_BtCancel_Clicked(wxCommandEvent& e);
};

#endif

