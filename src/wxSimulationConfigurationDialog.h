// -*- C++ -*- generated by wxGlade 0.4.1 on Sun Dec 03 23:00:07 2006
#ifndef WXSIMULATIONCONFIGURATIONDIALOG_H
#define WXSIMULATIONCONFIGURATIONDIALOG_H

#include <wx/wx.h>
#include <wx/image.h>
// begin wxGlade: ::dependencies
// end wxGlade

class wxSimulationConfigurationDialog: public wxDialog {
public:
    // begin wxGlade: wxSimulationConfigurationDialog::ids
    // end wxGlade

    wxSimulationConfigurationDialog(wxWindow* parent,wxWindowID id,const wxString& title);

private:
    // begin wxGlade: wxSimulationConfigurationDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

public:
    // begin wxGlade: wxSimulationConfigurationDialog::attributes
    wxStaticText* lbName;
    wxTextCtrl* txtName;
    wxStaticText* lbProvides;
    wxTextCtrl* txtProvides;
    wxStaticText* lbPlugIn;
    wxTextCtrl* txtPlugIn;
    wxStaticText* lbServerId;
    wxTextCtrl* txtServerId;
    wxButton* btOk;
    wxButton* btCancel;
    // end wxGlade

    void On_BtOk_Clicked(wxCommandEvent& e);
    void On_BtCancel_Clicked(wxCommandEvent& e);

}; // wxGlade: end class


#endif // WXSIMULATIONCONFIGURATIONDIALOG_H

