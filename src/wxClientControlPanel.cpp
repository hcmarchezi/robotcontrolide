
#include "wxClientControlPanel.h"

#include "SystemException.h"
#include <vector>

wxClientControlPanel::wxClientControlPanel(wxWindow* parent, wxWindowID id,PlayerControl * playerControl):wxPanel(parent, id)
{
    txtClientControl = new wxTextCtrl(this, id+1, wxT(""), wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxTE_MULTILINE | wxWANTS_CHARS | wxTE_PROCESS_TAB ); //|wxTE_RICH|wxTE_RICH2|wxTE_AUTO_URL|wxVSCROLL|wxHSCROLL|wxWANTS_CHARS);    
    txtClientControl->SetSize(wxDefaultSize.GetWidth(),430);    
    // Poe o conteudo do arquivo do txtClientControl ( linha a linha )
    for(unsigned int i=0; i < playerControl->GetSourceCode().size(); i++)
    {
      txtClientControl->AppendText( wxString( playerControl->GetSourceCode()[i].c_str() ) + wxString("\n") );
    }    
    
    //btCompile = new wxButton(this, id+2, wxT("Compile"));
    //btRunSimulation = new wxButton(this, -1, wxT("Simulate"));
    //btRunControl = new wxButton(this, -1, wxT("Send Control"));
    
    txtOutput = new wxTextCtrl(this, id+3, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);    
    txtOutput->SetSize(wxDefaultSize.GetWidth(),150); 
    
    this->do_layout();
    if ( playerControl == NULL)
    {
      throw new SystemException("wxClientControlPanel::wxClientControlPanel","Um programa de controle deve ser informado.");
    }
    this->playerControl = playerControl;

    wxEvtHandler::Connect(id+1,wxEVT_COMMAND_TEXT_UPDATED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &wxClientControlPanel::On_Text_Updated ); 
}

wxClientControlPanel::~wxClientControlPanel(void)
{
}

void wxClientControlPanel::do_layout()
{
  wxSizer * sizerClientControl = new wxBoxSizer(wxVERTICAL);
  wxSizer * sizerClientControlButtons = new wxBoxSizer(wxHORIZONTAL);
  sizerClientControlButtons = new wxBoxSizer(wxHORIZONTAL);
  sizerClientControl->Add(txtClientControl, 0, wxEXPAND, 0);
  //sizerClientControlButtons->Add(btCompile, 0, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
  //sizerClientControlButtons->Add(btRunSimulation, 0, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
  //sizerClientControlButtons->Add(btRunControl, 0, wxALIGN_CENTER_HORIZONTAL|wxADJUST_MINSIZE, 0);
  sizerClientControl->Add(sizerClientControlButtons, 1, wxADJUST_MINSIZE, 0);
  sizerClientControl->Add(txtOutput, 0, wxEXPAND, 0);  
  this->SetAutoLayout(true);
  this->SetSizer(sizerClientControl);
  sizerClientControl->Fit(this);
  sizerClientControl->SetSizeHints(this);
}

PlayerControl * wxClientControlPanel::GetPlayerControl()
{
  return this->playerControl;
}

void wxClientControlPanel::On_Text_Updated(wxCommandEvent& e)
{
  vector<string> arrStrTextOutput;
  for (int i = 0;i < this->txtClientControl->GetNumberOfLines(); i++)
  {
    arrStrTextOutput.push_back( this->txtClientControl->GetLineText(i).c_str() );
  }
  this->playerControl->SetSourceCode( arrStrTextOutput );  
}