
#include "wx3DChildModelDialog.h"

#include "Conversion.h"
//#include "ModelFactory.hh"
#include "ModelUtility.h"

wx3DChildModelDialog::wx3DChildModelDialog(VirtualWorld * virtualWorld, wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{ 
    //this->arrParentModels = arrParentModels;
    this->virtualWorld = virtualWorld;

    lb3DParentModelID = new wxStaticText(this,  -1, wxT("3D Parent Model"));
    cmbbx3DParentModelID = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN);

    lb3DChildModelType = new wxStaticText(this, -1, wxT("3D Child Model"));
    //const wxString cmbbx3DModelType_choices[] = { };
    cmbbx3DChildModelType = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN);

    lb3DChildModelID = new wxStaticText(this, -1, wxT("Child Model ID") );
    txt3DChildModelID = new wxTextCtrl(this, -1, wxT("") ); 

    lbPositionX = new wxStaticText(this, -1, wxT("Position X"));
    txtPositionX = new wxTextCtrl(this, -1, wxT(""));
    lbPositionY = new wxStaticText(this, -1, wxT("Position Y"));
    txtPositionY = new wxTextCtrl(this, -1, wxT(""));
    lbPositionZ = new wxStaticText(this, -1, wxT("Position Z"));
    txtPositionZ = new wxTextCtrl(this, -1, wxT(""));
    btAdd = new wxButton(this, id+1, wxT("Add"));
    btCancel = new wxButton(this, id+2, wxT("Cancel"));

    set_properties();
    do_layout();

    // Alimento o combobox de Ids de Modelos 3D ( incluindo os filhos destes )
    for (int index2 = 0; index2 < virtualWorld->GetNumModels(); index2++)
    {
      Model * model = virtualWorld->GetModels()[index2];
      this->cmbbx3DParentModelID->Append(wxString(model->GetId()));
    }

    // Alimenta o combobox de tipo de modelos filhos 3D
    //vector<string> arrStrModelType = ModelFactory::GetModelTypeNames();
    vector<string> arrStrModelType = ModelUtility::GetModelTypeNames();
    for (unsigned int index = 0; index < arrStrModelType.size(); index++)
    {
      this->cmbbx3DChildModelType->Append(wxString( arrStrModelType[index].c_str() ));
    }

    this->bButtonAddPressed = false;

    // Ligacao de eventos
    wxEvtHandler::Connect(id+1,wxEVT_COMMAND_BUTTON_CLICKED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &wx3DChildModelDialog::On_BtAdd_Clicked );
    wxEvtHandler::Connect(id+2,wxEVT_COMMAND_BUTTON_CLICKED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &wx3DChildModelDialog::On_BtCancel_Clicked );    
}


void wx3DChildModelDialog::set_properties()
{
    SetTitle(wxT("Select 3D Model and Position"));
    SetSize(wxSize(280, 160));

    lb3DParentModelID->SetSize(wxSize(64,13));
    cmbbx3DParentModelID->SetSize(wxSize(180,21));

    lb3DChildModelType->SetSize(wxSize(64, 13));
    cmbbx3DChildModelType->SetSize(wxSize(180, 21));

    lb3DChildModelID->SetSize(wxSize(64, 13));
    txt3DChildModelID->SetSize(wxSize(170, -1));

    lbPositionX->SetSize(wxSize(64, 13));
    txtPositionX->SetSize(wxSize(170, -1));
    lbPositionY->SetSize(wxSize(64, 13));
    txtPositionY->SetSize(wxSize(170, -1));
    lbPositionZ->SetSize(wxSize(64, 13));
    txtPositionZ->SetSize(wxSize(170, -1));
    btAdd->SetSize(wxSize(90, -1));
    btCancel->SetSize(wxSize(90, -1));
}


void wx3DChildModelDialog::do_layout()
{
    wxGridSizer* sizerLayout = new wxGridSizer(6, 2, 0, 0);
    sizerLayout->Add(lb3DParentModelID, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(cmbbx3DParentModelID, 0, wxADJUST_MINSIZE, 0);

    sizerLayout->Add(lb3DChildModelType, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(cmbbx3DChildModelType, 0, wxADJUST_MINSIZE, 0);

    sizerLayout->Add(lb3DChildModelID, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(txt3DChildModelID, 0, wxADJUST_MINSIZE, 0);

    sizerLayout->Add(lbPositionX, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(txtPositionX, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(lbPositionY, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(txtPositionY, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(lbPositionZ, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(txtPositionZ, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(btAdd, 0, wxADJUST_MINSIZE, 0);
    sizerLayout->Add(btCancel, 0, wxADJUST_MINSIZE, 0);
    SetAutoLayout(true);
    SetSizer(sizerLayout);
    Layout();
}

Model * wx3DChildModelDialog::Get3DParentModel()
{
  // Obtem o modelo selecionado no combobox
  const char * strSelectedParentModelID =  this->cmbbx3DParentModelID->GetStringSelection().c_str();
  
  for (int index=0; index < this->virtualWorld->GetNumModels(); index++)
  { 
    Model * model = this->virtualWorld->GetModels()[index];
    if ( strcmp( strSelectedParentModelID, model->GetId() ) == 0 )
    {
      return model;
    }
  }
  return NULL;

/*  int index = -1;
  for (index=0; index < this->arrParentModels->size(); index++)
  {
    if ( strSelectedParentModelID == (*this->arrParentModels)[index]->GetId() )
    {
      break;
    }
  }
  Model * model;
  if ((index == -1)||(index >= this->arrParentModels->size()))
  {
    model = NULL;
  }    
  else
  {
    model= (*this->arrParentModels)[index];
  }
  // retorna o modelo
  return model;*/
}

wxString wx3DChildModelDialog::Get3DChildModelType()
{
  return this->cmbbx3DChildModelType->GetStringSelection();
}

GzVector wx3DChildModelDialog::Get3DChildModelPosition()
{
  string strPositionX = string( this->txtPositionX->GetLineText(0).c_str() );
  string strPositionY = string( this->txtPositionY->GetLineText(0).c_str() );
  string strPositionZ = string( this->txtPositionZ->GetLineText(0).c_str() );

  double dPositionX = Conversion::StringToDouble(strPositionX);
  double dPositionY = Conversion::StringToDouble(strPositionY);
  double dPositionZ = Conversion::StringToDouble(strPositionZ);

  GzVector position;
  position.x = dPositionX;
  position.y = dPositionY;
  position.z = dPositionZ;
  return position;
}
 
wxString wx3DChildModelDialog::Get3DChildModelID()
{
  return this->txt3DChildModelID->GetLineText(0);
}

void wx3DChildModelDialog::On_BtAdd_Clicked(wxCommandEvent& e)
{
  this->bButtonAddPressed = true;
  this->Close();
}

void wx3DChildModelDialog::On_BtCancel_Clicked(wxCommandEvent& e)
{
  this->bButtonAddPressed = false;
  this->Close();
}

bool wx3DChildModelDialog::IsButtonAddPressed()
{
  this->bButtonAddPressed = true;
  return this->bButtonAddPressed;
}


