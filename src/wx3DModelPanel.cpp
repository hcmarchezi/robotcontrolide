
#include "wx3DModelPanel.h"

//////////////////////////////////////////////////////#include "ViewOptions.h"
#include "SystemException.h"

#include <map>
#include "ModelUtility.h"
#include "RenderOptionsSingleton.h"

using namespace std;

wx3DModelPanel::wx3DModelPanel(wxWindow* parent, wxWindowID id,IRCEProject* project):wxPanel(parent, id)
{
  // Verifica se um projeto foi informado
  if ( project == NULL)
  {
    throw new SystemException("Deve ser instanciado antes","wx3DModelPanel::wx3DModelPanel");
  }

  // Instancia o Widget responsável por mostrar os objetos 3D em OpenGL
  this->worldGLCanvas = new wxModelGLCanvas(    
    this,    
    this->GetId(),
    this->GetPosition(),    
    wxSize(500,500),
    wxSUNKEN_BORDER,
    wxT("GLCanvas"));  

  // Passa a a referencia de modelos 3D para o GLcanvas
  this->worldGLCanvas->SetVirtualWorld(project->GetVirtualWorld());

  // Instancia o label e o combobox de ViewType 
  wxPoint ptTabItemWorld = this->GetPosition();

  wxPoint ptViewType = ptTabItemWorld;
  ptViewType.y += this->worldGLCanvas->GetSize().GetHeight() + 5; //605;
  
  this->lbViewType = new wxStaticText(
    this,
    2,
    wxT("View Type"),
    ptViewType,
    wxSize(80,15));

  wxPoint ptCmbbxViewType = ptTabItemWorld;
  ptCmbbxViewType.x += 90;
  ptCmbbxViewType.y += this->worldGLCanvas->GetSize().GetHeight() + 5; //605;

  this->cmbbxViewType = new wxComboBox(
    this,
    3,
    wxEmptyString,
    ptCmbbxViewType,
    wxSize(200,15));
      
  // Instancia o RadioBox contendo as opcoes de renderizacao
  // SolidMesh e WireFrame
  wxPoint ptRadbRenderType = ptTabItemWorld;
  ptRadbRenderType.x += 320;
  ptRadbRenderType.y += this->worldGLCanvas->GetSize().GetHeight() + 5; // 600;

  const wxString arrStrOpcoes[] = {wxT("Solid"),wxT("Wireframe")};

  this->radbRenderType = new wxRadioBox(
    this,
    4,
    wxT(""),
    ptRadbRenderType,
    wxSize(150,35),
    2,
    arrStrOpcoes);

  // Insere os contextos de visoes (views) X/Y, X/Z, Y/Z numa lista
  // e insere cada nome de visao no combobox de visoes  
  this->cmbbxViewType->Append(this->worldGLCanvas->GetViewContext(0)->name);
  this->cmbbxViewType->Append(this->worldGLCanvas->GetViewContext(1)->name);
  this->cmbbxViewType->Append(this->worldGLCanvas->GetViewContext(2)->name);
  
  // combobox ja inicia apontando para a 1a opcao
  this->cmbbxViewType->SetWindowStyle(wxCB_READONLY);
  this->cmbbxViewType->SetSelection(0);  //Select(0);
  
  this->Connect(4,4,wxEVT_COMMAND_RADIOBOX_SELECTED,
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
    &wx3DModelPanel::OnRenderTypeRadioBoxSelected);

  this->Connect(3,3,wxEVT_COMMAND_COMBOBOX_SELECTED,
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
    &wx3DModelPanel::OnViewTypeComboboxSelected);
}

wx3DModelPanel::~wx3DModelPanel(void)
{
}


// Ao mover o mouse sobre o GL canvas
void wx3DModelPanel::OnGLCanvasMouse(wxMouseEvent& e)
{
  /*
  if (e.Dragging() && e.LeftIsDown())
  {
    wxSize clientSize = this->worldGLCanvas->GetClientSize();
    //this->worldGLCanvas->velocityX = 3.0*(double)(e.GetPosition().x - mousePosition.x) / (double)clientSize.GetX();
    //this->worldGLCanvas->velocityY = 3.0*(double)(e.GetPosition().y - mousePosition.y) / (double)clientSize.GetY();
    this->velocityX = 6.0*(double)(e.GetPosition().x - mousePosition.x)/(double)clientSize.GetX();
    this->velocityY = 6.0*(double)(e.GetPosition().y - mousePosition.y)/(double)clientSize.GetY();

    GzVector currentEyePosition    = this->worldGLCanvas->GetEyePosition();
    GzVector currentCenterPosition = this->worldGLCanvas->GetCenterPosition();
    GzVector currentUpVector       = this->worldGLCanvas->GetUpvector();
    
    if (this->arrViewContexts[this->iCurrentViewContext].name == wxT("X/Y"))
    {
      currentEyePosition.x -=    this->velocityX;
      currentEyePosition.y +=    this->velocityY;
      currentCenterPosition.x -= this->velocityX;
      currentCenterPosition.y += this->velocityY;
    }
    else if (this->arrViewContexts[this->iCurrentViewContext].name == wxT("X/Z"))
    {
      currentEyePosition.x +=    this->velocityX;
      currentEyePosition.z +=    this->velocityY;
      currentCenterPosition.x += this->velocityX;
      currentCenterPosition.z += this->velocityY;
    }
    else if (this->arrViewContexts[this->iCurrentViewContext].name == wxT("Y/Z"))
    {
      currentEyePosition.y -=    this->velocityX;
      currentEyePosition.z +=    this->velocityY;
      currentCenterPosition.y -= this->velocityX;
      currentCenterPosition.z += this->velocityY;
    }
 
    this->worldGLCanvas->SetEyePosition(currentEyePosition);
    this->worldGLCanvas->SetCenterPosition(currentCenterPosition);

  }
  else if (e.RightIsDown())//(e.Dragging() && e.RightIsDown())
  {
    //wxSize clientSize = this->worldGLCanvas->this->worldGLCanvas->GetClientSize();
    //this->worldGLCanvas->velocityX = 3.0*(double)(e.GetPosition().x - mousePosition.x) / (double)clientSize.GetX();
    //this->worldGLCanvas->velocityY = 3.0*(double)(e.GetPosition().y - mousePosition.y) / (double)clientSize.GetY();
    //
    //this->worldGLCanvas->zoom += this->worldGLCanvas->velocityY;

    //this->worldGLCanvas->this->worldGLCanvas->SetEyePosition(
    //  GzVectorSet(
    //  this->worldGLCanvas->zoom * cos(this->worldGLCanvas->angleY)*cos(this->worldGLCanvas->angleX),
    //  this->worldGLCanvas->zoom * sin(this->worldGLCanvas->angleY)*cos(this->worldGLCanvas->angleX),
    //  this->worldGLCanvas->zoom * sin(this->worldGLCanvas->angleX)
    //  ));

    // Obtem o Model selecionado no Canvas
    Model * model = this->worldGLCanvas->SelectObjects(e.GetPosition().x,e.GetPosition().y);
    if (model == NULL)
    {
      return;
    }    

    // Ao selecionar um objeto pelo botao direito do mouse, o componente wx3DModelPanel 
    // dispara um evento proprio contendo o Model 3D selecionado
    this->glEvent.model = model;
    this->AddPendingEvent(this->glEvent);

    //// Obtem as propriedades do model
    //map<string,string> lstProperties;
    //this->worldGLCanvas->appWorldBuilder->GetPropertyList(model,&lstProperties);
    //// Limpa as propriedades da janela
    //for(int i=0;i < this->worldGLCanvas->arrProperties.size();i++)
    //{
    //  delete this->worldGLCanvas->arrProperties[i];
    //}
    //this->worldGLCanvas->arrProperties.clear();   

    //// Mostra as propriedades na janela principal    
    //int id = 20;
    //map<string,string>::iterator it; 
    //for(it = lstProperties.begin();it != lstProperties.end(); it++)
    //{
    //  wxString strPropertyName      = (*it).first.c_str();
    //  wxString strPropertyValue     = (*it).second.c_str();
    //  wxGridSizer * gridSizer       = this->worldGLCanvas->frmMainWindow->sizerProperties;
    //  
    //  wxStaticText * lbPropertyName  = new wxStaticText(this->worldGLCanvas->frmMainWindow->tabitemProperties,id++, strPropertyName);
    //  lbPropertyName->SetSize(80,20);
    //  wxTextCtrl * txtPropertyValue = new wxTextCtrl(this->worldGLCanvas->frmMainWindow->tabitemProperties,id++,strPropertyValue);
    //  txtPropertyValue->SetSize(180,20);
    //  gridSizer->Add(lbPropertyName,1); //,wxEXPAND,0);
    //  gridSizer->Add(txtPropertyValue,1); //,wxEXPAND,0);

    //  this->worldGLCanvas->arrProperties.push_back(lbPropertyName);
    //  this->worldGLCanvas->arrProperties.push_back(txtPropertyValue);
    //}
    ////this->worldGLCanvas->frmMainWindow->tabitemProperties->Show();
    ////this->worldGLCanvas->frmMainWindow->sizerProperties->ShowItems(true);
    ////this->worldGLCanvas->frmMainWindow->Refresh();    
    ////this->worldGLCanvas->frmMainWindow->tabitemProperties->Update();
    ////this->worldGLCanvas->frmMainWindow->Update();
    ////this->worldGLCanvas->frmMainWindow->tabitemProperties->SetSize( 
    ////  this->worldGLCanvas->frmMainWindow->tabitemProperties->GetSize() );
    ////this->worldGLCanvas->frmMainWindow->mainSplitter->SetSize( 
    ////  this->worldGLCanvas->frmMainWindow->mainSplitter->GetSize()
    ////  );
    /////////////////////////////////COMO É Q EU ATUALIZO A TELA !!!
  } 
  this->mousePosition.x = e.GetPosition().x;
  this->mousePosition.y = e.GetPosition().y;
  this->worldGLCanvas->Draw();  
  */
}

// Ao selecionar um tipo de renderizacao
void wx3DModelPanel::OnRenderTypeRadioBoxSelected(wxCommandEvent& e)
{
  wxString strRenderType = this->radbRenderType->GetStringSelection();
  if (strRenderType == wxT("Wireframe"))
  {
    RenderOptionsSingleton::GetInstance()->polygonMode = GL_LINE;
  }
  else if (strRenderType == wxT("Solid"))
  {
    RenderOptionsSingleton::GetInstance()->polygonMode = GL_FILL;
  } 
  this->worldGLCanvas->Draw();
}

// Ao selecionar um tipo de visao 3D
void wx3DModelPanel::OnViewTypeComboboxSelected(wxCommandEvent& e)
{
  /////////////////////////////////////////////////////////////////////////////////////
  // Guarda a posicao do observador, centro e upvector no contexto atual 
  // ( Para visao ortografica )
  /////////////////////////////////////////////////////////////////////////////////////
  if ( this->worldGLCanvas->GetCurrentViewContext()->orthographic == true )
  {
    this->worldGLCanvas->GetCurrentViewContext()->eyePosition    = this->worldGLCanvas->GetEyePosition();
    this->worldGLCanvas->GetCurrentViewContext()->centerPosition = this->worldGLCanvas->GetCenterPosition();
    this->worldGLCanvas->GetCurrentViewContext()->upVector       = this->worldGLCanvas->GetUpvector();
  }

  ////////////////////////////////////////////////////////
  // Obtem o contexto de visao selecionado no combobox
  ////////////////////////////////////////////////////////
  ViewContext* viewContext = this->worldGLCanvas->GetViewContext(this->cmbbxViewType->GetStringSelection());

  ///////////////////////////////////////////////////////////////////
  // Passa os dados do contexto de visao selecionado no combobox
  // para o componente de visualizacao worldGLCanvas
  ///////////////////////////////////////////////////////////////////
  this->worldGLCanvas->EnableOrthographicView(viewContext->orthographic);
  this->worldGLCanvas->SetViewContextName(viewContext->name);

  if ( viewContext->camera != NULL )
  {
    //this->worldGLCanvas->SetEyePosition(viewContext->camera->GetEyePosition());
    //this->worldGLCanvas->SetCenterPosition(viewContext->camera->GetCenterPosition());
    //this->worldGLCanvas->SetUpVector(viewContext->camera->GetUpVector());
    this->worldGLCanvas->SetEyePosition( ModelUtility::GetEyePosition(viewContext->camera) );
    this->worldGLCanvas->SetCenterPosition( ModelUtility::GetCenterPosition(viewContext->camera) );
    this->worldGLCanvas->SetUpVector( ModelUtility::GetUpVector(viewContext->camera) );
  }
  else
  {
    this->worldGLCanvas->SetEyePosition(viewContext->eyePosition);
    this->worldGLCanvas->SetCenterPosition(viewContext->centerPosition);
    this->worldGLCanvas->SetUpVector(viewContext->upVector);
  }

  ///////////////////////////////////////////////////////////////////////
  // Seta o contexto selecionado como atual no componente de visualizacao
  ///////////////////////////////////////////////////////////////////////
  this->worldGLCanvas->SetCurrentViewContext(viewContext->name);
  
  ///////////////////////////////////////////////////////////////////////
  // Atualiza o GL widget
  ///////////////////////////////////////////////////////////////////////
  this->worldGLCanvas->Draw();
  this->Layout();
}
