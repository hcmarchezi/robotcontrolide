
#include "worldbuilder.h"

#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/process.h>
#include <wx/regex.h>

/////////////////////////////////#include "ViewOptions.h"
#include "RenderOptionsSingleton.h"
//#include "RenderOptions.h"
/////////////////////////////////#include "WorldFile.h"
#include "ExceptionHandler.h"
#include "SystemException.h"
#include "ApplicationException.h"

#include "wxClientControlPanel.h"
#include "wx3DModelPanel.h"
#include "wxConfigurationPanel.h"
#include "ModelFactory.hh"
#include "ModelUtility.h"

#include "wx3DModelDialog.h"
#include "wx3DChildModelDialog.h"
#include "wxIRCEConfigurationDialog.h"

#include <map>
#include <vector>

#include <typeinfo>

#include "PlayerControl.h"
#include "IRCEConfiguration.h"

#include "ConfigurationFile.h"
#include "wxSimulationConfigurationDialog.h"
#include "wxPlayerConfigurationDialog.h"
#include <wx/wx.h>

//////////////////#include "Camera.h"
#include "wxModelGLCanvas.h"
#include <GL/glut.h>


#include "DoubleField.h"
#include "CanonVCC4.hh" 
#include "Conversion.h"

#include "SimulationConfiguration.h"
#include "PlayerControl.h"

using namespace std;

IMPLEMENT_APP(WorldBuilderApp)

///////////////////////////////////////////////////
// Construtor do Controller ( aplicacao )
///////////////////////////////////////////////////
WorldBuilderApp::WorldBuilderApp(void)
{
  // Iniciando a biblioteca glut ( OpenGL )
#ifndef WIN32
  char ** argv = NULL;
  int argc = 1;
  glutInit(&argc,argv);
#endif 

  // Iniciando a classe auxiliar ModelUtility
  ModelUtility::Initialize();

  // Render Options - SolidMesh
  RenderOptionsSingleton::GetInstance()->polygonMode = GL_FILL;

  // Variaveis para calculo de coordenadas esf�ricas
  // pelo arrasto do mouse para GLCanvas 
  this->velocityX = 0.0;
  this->velocityY = 0.0;
  this->angleX = 0.0;
  this->angleY = 0.0;
  this->zoom = 5.0;

  // Inicialmente nao existe modelo selecionado
  this->selected3DModel = NULL;

  ///////////////////////////////////////////////////////////////////////////
  // Instanciando a janela, menu e canvas
  // De acordo com os exemplos do wxWidgets esses objetos sao 
  // destru�dos automaticamente
  this->menuBar = new wxIRCEMenuBar();

  
  // janela principal (View) do sistema 
  this->frmMainWindow = new wxIRCEMainWindow(0,0,wxT("Gazebo WorldBuilder"));

  // Insere os nomes dos modelos 3D numa lista dentro da janela principal
  //this->frmMainWindow->lstbxModels->InsertColumn(0,wxT("Modelos"));
  //vector<string> arrStrModelType = ModelFactory::GetInstance()->GetModelTypeNames();
  //this->frmMainWindow->lstbxModels->InsertItem(0,wxT("----------"));
  //for (int i=0; i < arrStrModelType.size(); i++)
  //{    
  //  this->frmMainWindow->lstbxModels->InsertItem(i+1,wxT(arrStrModelType[i].c_str()));
  //}

  //////////////// Instancia as Aplicacoes do sistema ( Instancia Models do MVC )
  this->appIRCEProject = new AppIRCEProject();
  this->appVirtualWorld = new AppVirtualWorld();
  this->appControlProgram = new AppControlProgram();
  this->appConfiguration = new AppConfiguration();
  
  // Abre a janela principal
  this->frmMainWindow->Show(TRUE);
  SetTopWindow(this->frmMainWindow);  

  // Atribui a barra de menus principais
  this->frmMainWindow->SetMenuBar(this->menuBar);
  
  // Menu Bar assume o estado inicial
  this->menuBar->SetState(NO_PROJECT_CREATED);
  
  //////////////////////////////////////////////////////////////////////
  // Adiciona os tabs na janela principal
  //////////////////////////////////////////////////////////////////////
  
  //// Cria um 3D Model Panel conectando os modelos da Aplicacao no controle gr�fico
  //wx3DModelPanel * panel = new wx3DModelPanel(this->frmMainWindow->tabModel,-1,this->appIRCEProject->project);
  //// DEVE TER O MESMO ID DO wx3DModelPanel !!!!
  //wxEvtHandler:Connect(-1,-1, wxEVT_COMMAND_RIGHT_CLICK,
  //                (wxObjectEventFunction) (wxEventFunction) (wxModelSelectedEventFunction)
  //                &WorldBuilderApp::wx3DModelPanel_On_Model_Selected );
  //this->frmMainWindow->tabModel->AddPage(panel,wxT("Virtual World"));

  //this->frmMainWindow->tabModel->AddPage(new wxConfigurationPanel(this->frmMainWindow->tabModel),wxT("Configuration"));
  //this->frmMainWindow->tabModel->AddPage(new wxClientControlPanel(this->frmMainWindow->tabModel),wxT("Client Control 1"));
  //this->frmMainWindow->tabModel->AddPage(new wxClientControlPanel(this->frmMainWindow->tabModel),wxT("Client Control 2"));    

}

///////////////////////////////////////////////////
// Destrutor do Controller
///////////////////////////////////////////////////
WorldBuilderApp::~WorldBuilderApp(void)
{
  ///////////////////////////////delete ViewOptions::GetInstance();
  RenderOptionsSingleton::DeleteInstance();  
  delete this->appIRCEProject;
  delete this->appVirtualWorld;
  delete this->appControlProgram;
  delete this->appConfiguration;
}

void WorldBuilderApp::LoadConfigurations()
{
  wxString strIRCEConfigFile = "IRCE.cfg";

  // Verifica se o arquivo existe
  if (!wxFileExists( strIRCEConfigFile ))
  {
    throw new ApplicationException("O arquivo IRCE.cfg nao foi encontrado");
  }

  // Abre o arquivo como arquivo texto
  wxTextFile file( strIRCEConfigFile);
  if (!file.Open()) 
  {
    throw new SystemException("ConfigurationFile::LoadConfiguration","Ocorreu um problema ao abrir o arquivo IRCE.cfg");
  }

  wxRegEx regexPlayerPathParam;
  wxRegEx regexGazeboPathParam;
  wxRegEx regexCPPCompilerPathParam;
  wxRegEx regexPlayerHeaderPathParam;
  wxRegEx regexPlayerLibPathParam;
  wxRegEx regexPlayerLibsParam;

  // Caminho do Player, Gazebo e compilador C++ ( incluindo o programa executavel )
  if (!regexPlayerPathParam.Compile(wxT("PLAYER_PATH=[:alnum:]*")))
  {
    throw new SystemException("Erro ao compilar regex PLAYER_PATH","WorldBuilderApp::LoadConfigurations");
  }
  if (!regexGazeboPathParam.Compile(wxT("GAZEBO_PATH=[:alnum:]*")))
  {
    throw new SystemException("Erro ao compilar regex GAZEBO_PATH","WorldBuilderApp::LoadConfigurations");
  }
  if (!regexCPPCompilerPathParam.Compile(wxT("CPP_COMPILER_PATH=[:alnum:]*")))
  {
    throw new SystemException("Erro ao compilar regex CPP_COMPILER_PATH","WorldBuilderApp::LoadConfigurations");
  }
  // Caminho da biblioteca do player para controle de robos e o caminho
  // da biblioteca compilada (lib)
  if (!regexPlayerHeaderPathParam.Compile(wxT("PLAYER_HEADER_PATH=[:alnum:]*")))
  {
    throw new SystemException("Erro ao compilar regex PLAYER_HEADER_PATH","WorldBuilderApp::LoadConfigurations");
  }
  if (!regexPlayerLibPathParam.Compile(wxT("PLAYER_LIB_PATH=[:alnum:]*")))
  {
    throw new SystemException("Erro ao compilar regex PLAYER_LIB_PATH","WorldBuilderApp::LoadConfigurations");
  }
  if (!regexPlayerLibsParam.Compile(wxT("PLAYER_LIBS=[:alnum:]*")))
  {
    throw new SystemException("Erro ao compilar regex PLAYER_LIBS","WorldBuilderApp::LoadConfigurations");
  }

  // Le todo o conteudo do arquivo 
  wxString strFileContent;
  wxString strParamValue;
  for (unsigned int i=0; i < file.GetLineCount(); i++)
  {
    strFileContent = file.GetLine(i);
    int posicao = strFileContent.find_first_of("=")+1;
    int tamanho = strFileContent.Length();
    strParamValue = strFileContent.substr(posicao,tamanho-posicao);

    if (regexPlayerPathParam.Matches(strFileContent))
    {
      IRCEConfiguration::strPlayerPath = string( strParamValue.c_str() );
    }
    else if (regexGazeboPathParam.Matches(strFileContent))
    {
      IRCEConfiguration::strGazeboPath = string( strParamValue.c_str() );
    }
    else if (regexCPPCompilerPathParam.Matches(strFileContent))
    {
      IRCEConfiguration::strCPPCompilerPath = string( strParamValue.c_str() );
    }
    else if (regexPlayerHeaderPathParam.Matches(strFileContent))
    {
      IRCEConfiguration::strPlayerHeaderPath = string( strParamValue.c_str() );
    }
    else if (regexPlayerLibPathParam.Matches(strFileContent))
    {
      IRCEConfiguration::strPlayerLibPath = string( strParamValue.c_str() );
    }
    else if (regexPlayerLibsParam.Matches(strFileContent))
    {
      IRCEConfiguration::strPlayerLibs = string( strParamValue.c_str() );
    }
  }

  wxString strMessage = wxT("");
  strMessage += wxT("Player Path = ");
  strMessage += IRCEConfiguration::strPlayerPath.c_str();
  strMessage += '\n';
  strMessage += wxT("Gazebo Path = ");
  strMessage += IRCEConfiguration::strGazeboPath.c_str();
  strMessage += wxT('\n');
  strMessage += wxT("G++ Path = ");
  strMessage += IRCEConfiguration::strCPPCompilerPath.c_str();
  strMessage += wxT('\n');
  strMessage += wxT("Player Header Path = ");
  strMessage += IRCEConfiguration::strPlayerHeaderPath.c_str();
  strMessage += wxT('\n');
  strMessage += wxT("Player Lib Path = ");
  strMessage += IRCEConfiguration::strPlayerLibPath.c_str();
  strMessage += wxT('\n');
  strMessage += wxT("Player Libs = ");
  strMessage += IRCEConfiguration::strPlayerLibs.c_str();
  strMessage += wxT('\n');
  wxMessageDialog messageBox(NULL, strMessage, wxT("Informacao sobre Configuracoes"), wxOK );
  messageBox.ShowModal();

  // Fecha o arquivo
  file.Close();
}

////////////////////////////////////////////////////////////////////////////
// Instancia as aplicacoes e faz a conexao dos eventos entre View e Model 
////////////////////////////////////////////////////////////////////////////
bool WorldBuilderApp::OnInit()
{  
  try
  {
    /////////////////////////////////////////////////////////////
    // Carrega configuracoes do IRCE em IRCE.cfg ( se existir )
    this->LoadConfigurations();
  }
  catch(Exception * exception)
  {
    ExceptionHandler::Handler(exception);
    exit(0);
  }

  //////////////////////////////////////////////////////////////////////
  // Instancia as Aplicacoes ( models em MVC ) e conecta os eventos na 
  // Janela ( View ) para essas aplicacoes.

  // Conectando os eventos do item da arvores de itens
  this->frmMainWindow->treectrlItems->SetId( TREECONTROL_ITEMACTIVATED );
  wxEvtHandler::Connect( TREECONTROL_ITEMACTIVATED, wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
                  (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction)
                  &WorldBuilderApp::TreeControlItems_Item_Activated );

  // Conectando o evento de selecionar um tab item a atualiza�ao do menu
  this->frmMainWindow->tabModel->SetId( TABMODEL_TABITEM_CHANGED );
  wxEvtHandler::Connect( TABMODEL_TABITEM_CHANGED, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                  (wxObjectEventFunction) (wxEventFunction) (wxNotebookEventFunction)
                  &WorldBuilderApp::TabModel_Page_Changed );
 
  // Conectando os Slots da Aplicacao de Projeto IRCE   
  wxEvtHandler::Connect( NEW_PROJECT_MENU, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::wxIRCEMenuBar_NewProject_Click );
  wxEvtHandler::Connect( EDIT_PROJECT_MENU, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::wxIRCEMenuBar_EditProject_Click );
  wxEvtHandler::Connect( LOAD_PROJECT_MENU,wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::wxIRCEMenuBar_LoadProject_Click);
  wxEvtHandler::Connect( SAVE_PROJECT_MENU,wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::wxIRCEMenuBar_SaveProject_Click);
  wxEvtHandler::Connect( SIMULATE_PROJECT_MENU,wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::wxIRCEMenuBar_SimulateProject_Click);
  wxEvtHandler::Connect( TRANSFER_CONTROL_MENU,wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::wxIRCEMenuBar_TransferControl_Click);

  wxEvtHandler::Connect( EXIT_MENU,     wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::wxIRCEMenuBar_Exit_Click);
  wxEvtHandler::Connect( ABOUT_MENU,    wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::wxIRCEMenuBar_About_Click);

  // Conectando os eventos da Aplicacao de Montagem do Mundo Virtual
  wxEvtHandler::Connect( NEW_WORLD_FILE, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_NewWorldFile_Click );
  wxEvtHandler::Connect( CLEAN_VIRTUAL_WORLD, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_CleanVirtualWorld_Click );
  wxEvtHandler::Connect( LOAD_WORLD_FILE, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_LoadWorldFile_Click );
  wxEvtHandler::Connect( SAVE_WORLD_FILE, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_SaveWorldFile_Click );
  wxEvtHandler::Connect( ADD_3D_MODEL, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_Add3DModel_Click );
  wxEvtHandler::Connect( REMOVE_3D_MODEL, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_Remove3DModel_Click );
  wxEvtHandler::Connect( SELECT_3D_MODEL, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_Select3DModel_Click );
    wxEvtHandler::Connect( ADD_CHILD_MODEL, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_Add3DChildModel );
      wxEvtHandler::Connect( REMOVE_CHILD_MODEL, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_Remove3DChildModel );


  // Conectando os eventos da Aplicacao do Programa de Controle
  wxEvtHandler::Connect( NEW_CONTROL_PROGRAM, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_NewControlProgram_Click );
  wxEvtHandler::Connect( LOAD_CONTROL_PROGRAM, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_LoadControlProgram_Click );
  wxEvtHandler::Connect( SAVE_CONTROL_PROGRAM, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_SaveControlProgram_Click );
  wxEvtHandler::Connect( REMOVE_CONTROL_PROGRAM, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_RemoveControlProgram_Click );
  wxEvtHandler::Connect( COMPILE_CONTROL_PROGRAM, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_CompileControlProgram_Click );

  // Conectar os eventos da aplicacao de configuracao
  wxEvtHandler::Connect( NEW_CONFIGURATION_FILE, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_NewConfigurationFile_Click );
  wxEvtHandler::Connect( LOAD_CONFIGURATION_FILE, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_LoadConfigurationFile_Click );
  wxEvtHandler::Connect( SAVE_CONFIGURATION_FILE, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_SaveConfigurationFile_Click );
  wxEvtHandler::Connect( REMOVE_CONFIGURATION_FILE, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &WorldBuilderApp::wxIRCEMenuBar_RemoveConfigurationFile_Click );
    
	return TRUE;
} 

//////////////////////////////////////////////////////////////
// M�todos de Evento ( Slots )
//////////////////////////////////////////////////////////////

// Eventos de WorldBuilderMenuBar

//void WorldBuilderApp::WorldBuilderMenuBar_NewWorldMenu_Click(wxCommandEvent& e)
//{
//  try
//  {
//    this->appWorldBuilder->NewWorldFile("bb",NULL);
//  }
//  catch(exception * e)
//  {
//    ExceptionHandler::Handler(e);
//  }
//}
//
//void WorldBuilderApp::WorldBuilderMenuBar_LoadWorld_Click(wxCommandEvent& e)
//{
//  try
//  {
//    wxFileDialog * fileDialog = new wxFileDialog(
//        this->frmMainWindow,
//        wxT("Selecione o arqiuivo World"),
//        wxT("."),
//        wxEmptyString,
//        wxT("*.world"));
//    fileDialog->ShowModal();
//    wxString strPath = fileDialog->GetPath();
//
//    if (strPath.compare("") != 0)
//    {
//      // O WorldFile nao deveria estar dentro do model ( aplicacao ) ??
//      WorldFile * worldFile = new WorldFile();
//      this->appWorldBuilder->LoadWorldFile(strPath.c_str(),worldFile);
//      delete worldFile;
//    }
//  }
//  catch(exception * e)
//  {
//    ExceptionHandler::Handler(e);
//  }
//}
//
//void WorldBuilderApp::WorldBuilderMenuBar_SaveWorld_Click(wxCommandEvent& e)
//{
//  int i = 0;
//}
//
//void WorldBuilderApp::WorldBuilderMenuBar_Exit_Click(wxCommandEvent& e)
//{
//  wxWindow * topWindow = this->GetTopWindow();
//  topWindow->Close();
//}
//
//void WorldBuilderApp::WorldBuilderMenuBar_About_Click(wxCommandEvent& e)
//{
//  int i = 0;
//}
//
//
//

////////////////////////////////////////////////////////////////////////////////////////////
// Eventos de wxIRCEMenuBar

// AppIRCEProject

void WorldBuilderApp::wxIRCEMenuBar_NewProject_Click(wxCommandEvent& e)
{
  try
  {
    wxFileDialog * fileDialog = new wxFileDialog(
        this->frmMainWindow,
        wxT("Informe novo arquivo de projeto (*.irce)"),
        wxT("."),
        wxEmptyString,
        wxT("*.irce"));
    fileDialog->ShowModal();
    wxString strPath = fileDialog->GetPath();

    if ( strPath == wxT("") )
    {
      return;
    }

    this->appIRCEProject->CreateIRCEProject( string( strPath.c_str() ) );     

    // Passa o projeto criado para as outras aplicacoes
    this->appVirtualWorld->project = this->appIRCEProject->project;
    this->appControlProgram->project = this->appIRCEProject->project;
    this->appConfiguration->project = this->appIRCEProject->project;

    // Atualiza a arvore na interface gr�fica
    this->frmMainWindow->SetProject( this->appIRCEProject->project );
    this->frmMainWindow->treectrlItems->SetFocus();
    // Atualiza o menu
    this->menuBar->SetState(PROJECT_CREATED);
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_EditProject_Click(wxCommandEvent& e)
{
  try
  {
    throw new SystemException("Metodo nao foi implementado !!","WorldBuilderApp::wxIRCEMenuBar_EditProject_Click");
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_LoadProject_Click(wxCommandEvent& e)
{
  try
  {
    wxFileDialog * fileDialog = new wxFileDialog(
        this->frmMainWindow,
        wxT("Informe o arquivo de projeto (*.irce)"),
        wxT("."),
        wxEmptyString,
        wxT("*.irce"));
    fileDialog->ShowModal();
    wxString strPath = fileDialog->GetPath();

    if (strPath.compare("") != 0)
    {
      this->appIRCEProject->LoadIRCEProject( string( strPath.c_str() ) );

      // Passa o projeto criado para as outras aplicacoes
      this->appVirtualWorld->project = this->appIRCEProject->project;
      this->appControlProgram->project = this->appIRCEProject->project;  
      this->appConfiguration->project = this->appIRCEProject->project;

      // Atualiza a arvore na interface gr�fica 
      this->frmMainWindow->SetProject( this->appIRCEProject->project );

      // Cria um 3D Model Panel conectando os modelos da Aplicacao no controle gr�fico
      wx3DModelPanel * panel = new wx3DModelPanel(this->frmMainWindow->tabModel,10,this->appIRCEProject->project);
      // DEVE TER O MESMO ID DO wx3DModelPanel !!!!
      wxEvtHandler::Connect(-1,-1, wxEVT_COMMAND_RIGHT_CLICK,
                    (wxObjectEventFunction) (wxEventFunction) (wxModelSelectedEventFunction)
                    &WorldBuilderApp::wx3DModelPanel_On_Model_Selected );
      this->frmMainWindow->tabModel->AddPage(panel,wxT("Virtual World"));

      // Cria os paineis para cada programa de controle
      for( unsigned int i = 0; i < this->appIRCEProject->project->GetPlayerControlPrograms().size(); i++ )
      {
        PlayerControl * playerControl = this->appIRCEProject->project->GetPlayerControlPrograms()[i];

        wxClientControlPanel * clientPanel = new wxClientControlPanel(
          this->frmMainWindow->tabModel,
          -1,
          playerControl);
        this->frmMainWindow->tabModel->AddPage( clientPanel, wxFileNameFromPath( wxString( playerControl->GetClientControlFile().c_str() ) ) );
      }

      // Cria os paineis para cada configuracao
      for ( unsigned int i = 0; i < this->appIRCEProject->project->GetConfigurationFiles().size(); i++)
      {
        ConfigurationFile * configurationFile = this->appIRCEProject->project->GetConfigurationFiles()[i];

/*        wxConfigurationPanel * configurationPanel = new wxConfigurationPanel(
          this->frmMainWindow->tabModel,
          10,
          configurationFile,
          &(this->appIRCEProject->project->GetVirtualWorld()->GetModels())
          );*/
        wxConfigurationPanel * configurationPanel = new wxConfigurationPanel(
          this->frmMainWindow->tabModel,
          10,
          configurationFile
          );

        this->frmMainWindow->tabModel->AddPage( configurationPanel, wxFileNameFromPath( configurationFile->GetConfigurationFile().c_str() ) );
        // Conecta os eventos no Controller WorldBuilder
        // DEVE TER O MESMO ID DO wx3DModelPanel !!!!
        wxEvtHandler::Connect(11,11, wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                 &WorldBuilderApp::wxConfigurationPanel_On_SetSimulationConfiguration_Button_Click );
        wxEvtHandler::Connect(12,12, wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                 &WorldBuilderApp::wxConfigurationPanel_On_AddConfigurationPlayer_Button_Click );
        wxEvtHandler::Connect(13,13, wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                 &WorldBuilderApp::wxConfigurationPanel_On_UpdateConfigurationPlayer_Button_Click );
        wxEvtHandler::Connect(14,14, wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                 &WorldBuilderApp::wxConfigurationPanel_On_RemoveConfigurationPlayer_Button_Click );
      }

      // Adiciona as cameras no combobox de tipos de visoes
      Model ** arrModels = this->appIRCEProject->project->GetVirtualWorld()->GetModels();
      int modelCount = this->appIRCEProject->project->GetVirtualWorld()->GetNumModels();
      for (int i=0;i < modelCount; i++)
      {
        //this->wx3DModelPanel_UpdateViewTypeCombobox((*arrModels)[i]);
        this->wx3DModelPanel_UpdateViewTypeCombobox(arrModels[i]);
      }

      // Seleciona o tab item
      this->frmMainWindow->tabModel->SetSelection( 0 ); // this->frmMainWindow->tabModel->GetPageCount() -1 );
      // Atualiza o menu -> Virtual World Selecionado
      this->menuBar->SetState(VIRTUAL_WORLD_SELECTED);
    }
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_SaveProject_Click(wxCommandEvent& e)
{
  try
  {
    this->appIRCEProject->SaveIRCEProject();
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_SimulateProject_Click(wxCommandEvent& e)
{
  try
  {
    this->appIRCEProject->SimulateIRCEProject(this->frmMainWindow);
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_TransferControl_Click(wxCommandEvent& e)
{
  try
  {
    this->appIRCEProject->TransferControl();
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_Exit_Click(wxCommandEvent& e)
{
  wxWindow * topWindow = this->GetTopWindow();
  topWindow->Close();
}

void WorldBuilderApp::wxIRCEMenuBar_About_Click(wxCommandEvent& e)
{
  wxMessageDialog * messageDialog  = new wxMessageDialog(this->frmMainWindow," IRCE version 0.1 ","About");
  messageDialog->ShowModal();
}

// AppIRCEVirtualWorld

void WorldBuilderApp::wxIRCEMenuBar_NewWorldFile_Click(wxCommandEvent& e)
{
  try
  {
    // Obtem o caminho do arquivo
    wxFileDialog * fileDialog = new wxFileDialog(
        this->frmMainWindow,wxT("Informe o arquivo de projeto (*.world)"),
        wxT("."),wxEmptyString,wxT("*.world"));
    fileDialog->ShowModal();
    wxString strPath = fileDialog->GetPath();
    if ( strPath == "" ) { return; }

    // Novo arquivo de mundo virtual
    this->appVirtualWorld->NewWorldFile( string( strPath.c_str() ) );

    // Adiciona o nome o arquivo na arvore
    this->frmMainWindow->SetVirtualWorld( wxFileNameFromPath( wxString( this->appVirtualWorld->project->GetVirtualWorld()->GetWorldFile().c_str() ) ) );

    // Cria um 3D Model Panel conectando os modelos da Aplicacao no controle gr�fico
    wx3DModelPanel * panel = new wx3DModelPanel(this->frmMainWindow->tabModel,-1,this->appIRCEProject->project);
    // DEVE TER O MESMO ID DO wx3DModelPanel !!!!
    wxEvtHandler:Connect(-1,-1, wxEVT_COMMAND_RIGHT_CLICK,
                    (wxObjectEventFunction) (wxEventFunction) (wxModelSelectedEventFunction)
                    &WorldBuilderApp::wx3DModelPanel_On_Model_Selected );
    this->frmMainWindow->tabModel->AddPage(panel,wxT("Virtual World"));

    // Seleciona o tab item
    this->frmMainWindow->tabModel->SetSelection( this->frmMainWindow->tabModel->GetPageCount() -1 );
    // Atualiza o menu
    this->menuBar->SetState(VIRTUAL_WORLD_SELECTED);
    
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_CleanVirtualWorld_Click(wxCommandEvent& e)
{
  try
  {
    this->appVirtualWorld->CleanVirtualWorld();
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxIRCEMenuBar_LoadWorldFile_Click(wxCommandEvent& e)
{
  try
  {
    // Obtem o caminho do arquivo
    wxFileDialog * fileDialog = new wxFileDialog(
        this->frmMainWindow,
        wxT("Informe o arquivo de projeto (*.world)"),
        wxT("."),
        wxEmptyString,
        wxT("*.world"));
    fileDialog->ShowModal();
    wxString strPath = fileDialog->GetPath();
    if ( strPath == "" ) { return; }

    // Carrega o arquivo
    this->appVirtualWorld->LoadVirtualWorld( string( strPath.c_str() ) );

    // Atualiza a arvore de arquivos na janela principal
    this->frmMainWindow->SetVirtualWorld( wxString( this->appVirtualWorld->project->GetVirtualWorld()->GetWorldFile().c_str() ) );

    // Cria um 3D Model Panel conectando os modelos da Aplicacao no controle gr�fico
    wx3DModelPanel * panel = new wx3DModelPanel(this->frmMainWindow->tabModel,-1,this->appIRCEProject->project);
    // DEVE TER O MESMO ID DO wx3DModelPanel !!!!
    wxEvtHandler:Connect(-1,-1, wxEVT_COMMAND_RIGHT_CLICK,
                    (wxObjectEventFunction) (wxEventFunction) (wxModelSelectedEventFunction)
                    &WorldBuilderApp::wx3DModelPanel_On_Model_Selected );
    this->frmMainWindow->tabModel->AddPage(panel,wxT("Virtual World"));

    // Adiciona as cameras no combobox de tipos de visoes
    Model ** arrModels = this->appIRCEProject->project->GetVirtualWorld()->GetModels();
    int modelCount = this->appIRCEProject->project->GetVirtualWorld()->GetNumModels();
    for (int i=0;i < modelCount; i++)
    {
      //this->wx3DModelPanel_UpdateViewTypeCombobox((*arrModels)[i]);
      this->wx3DModelPanel_UpdateViewTypeCombobox(arrModels[i]);
    }

    // Seleciona o tab item
    this->frmMainWindow->tabModel->SetSelection( this->frmMainWindow->tabModel->GetPageCount() -1 );
    // Atualiza o menu
    this->menuBar->SetState(VIRTUAL_WORLD_SELECTED);
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_SaveWorldFile_Click(wxCommandEvent& e)
{
  try
  {
    this->appVirtualWorld->SaveWorldFile();
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_Add3DModel_Click(wxCommandEvent& e)
{
  try
  {    
    // Janela para capturar os dados de entrada 
    wx3DModelDialog * modelDialog = new wx3DModelDialog(this->frmMainWindow,-1,"Select 3D Model and Position",
      wxDefaultPosition,wxDefaultSize);
    modelDialog->ShowModal();

    if (!modelDialog->IsButtonAddPressed())
    {
      return;
    }

    wxString str3DModelType = modelDialog->Get3DModelType();
    GzVector position       = modelDialog->Get3DModelPosition();
    wxString str3DModelID   = modelDialog->Get3DModelID();

    // Executa a aplicacao e desempenha a tarefa
    this->appVirtualWorld->Add3DModel( string( str3DModelType.c_str() ), string( str3DModelID.c_str() ),position);        

    // Se foi adicionado uma camera, esta deve ser adicionada na lista de visoes
    Model* model = this->appVirtualWorld->project->GetVirtualWorld()->GetModel( str3DModelID.c_str() );
    this->wx3DModelPanel_UpdateViewTypeCombobox(model);
    this->frmMainWindow->tabModel->Layout();    
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxIRCEMenuBar_Remove3DModel_Click(wxCommandEvent& e)
{
  try
  {
    // Mostra um dialogo com o nome dos modelos 
    //int i3DModelCount = this->appIRCEProject->project->GetVirtualWorld()->GetParentModels()->size();
    int i3DModelCount = this->appIRCEProject->project->GetVirtualWorld()->GetParentNumModels();
    wxString * arrStr3DModelChoices = new wxString[i3DModelCount];
    for (unsigned int index=0;index < i3DModelCount; index++)
    {
      arrStr3DModelChoices[index] = wxString( this->appIRCEProject->project->GetVirtualWorld()->GetParentModels()[index]->GetId() );
    }
    wxSingleChoiceDialog * dialog = new wxSingleChoiceDialog(
      this->frmMainWindow,
      wxT("Select 3D Model from Virtual World"),
      wxT("Erase 3D Model"),
      i3DModelCount,
      arrStr3DModelChoices);
    dialog->ShowModal();    

    // Recupera o ID do dialogo
    wxString strID = dialog->GetStringSelection();

    if (strID != wxT(""))
    {
      // Recupera um modelo 3D do projeto IRCE pelo ID
      Model * model = this->appIRCEProject->project->GetVirtualWorld()->GetModel( strID.c_str() );

      // Janela que captura o modelo 3D selecionado
      this->appVirtualWorld->Remove3DModel(model);
    }
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_Select3DModel_Click(wxCommandEvent& e)
{
  try
  {
    // Mostra um dialogo com o nome dos modelos 
    //int i3DModelCount = this->appIRCEProject->project->GetVirtualWorld()->GetParentModels()->size();
    int i3DModelCount = this->appIRCEProject->project->GetVirtualWorld()->GetParentNumModels();
    wxString * arrStr3DModelChoices = new wxString[i3DModelCount];

    int arrayIndex = 0;
    for (int index=0;index < i3DModelCount; index++)
    {
      Model * model = this->appIRCEProject->project->GetVirtualWorld()->GetParentModels()[index];
      // Apenas os modelos que nao estao subordinados a outros modelos sao acrescentados a lista
      if (model->parent == NULL)
      {
        string strID = model->GetId();
        arrStr3DModelChoices[arrayIndex++] = wxString( strID.c_str() );
      }
    }
    wxSingleChoiceDialog * dialog = new wxSingleChoiceDialog(
      this->frmMainWindow,
      wxT("Select 3D Model from Virtual World"),
      wxT("Select 3D Model"),
      i3DModelCount,
      arrStr3DModelChoices);
    dialog->ShowModal();

       // Recupera o ID do dialogo
    wxString strID = dialog->GetStringSelection();

    if (strID != wxT(""))
    {
      // Recupera um modelo 3D do projeto IRCE pelo ID
      Model * model = this->appIRCEProject->project->GetVirtualWorld()->GetModel( strID.c_str() );
      // Lista de propriedades do modelo 3D selecionado
      map<string,string> * lstProperties;

      // Janela que captura o modelo 3D selecionado
      this->appVirtualWorld->GetPropertyList( model, &lstProperties ); 
      
      // Limpa as propriedades da janela
      for(unsigned int i=0;i < this->arrProperties.size();i++)
      {
        delete this->arrProperties[i];
      }
      this->arrProperties.clear();  

      // Mostra as propriedades na janela principal    
      int id = 20;
      map<string,string>::iterator it; 
      for(it = lstProperties->begin();it != lstProperties->end(); it++)
      {
        wxString strPropertyName      = (*it).first.c_str();
        wxString strPropertyValue     = (*it).second.c_str();
        wxGridSizer * gridSizer       = this->frmMainWindow->sizerProperties;
        
        wxStaticText * lbPropertyName  = new wxStaticText(this->frmMainWindow->tabitemProperties,id++, strPropertyName);
        lbPropertyName->SetSize(80,20);
        wxTextCtrl * txtPropertyValue = new wxTextCtrl(this->frmMainWindow->tabitemProperties,id++,strPropertyValue);
        txtPropertyValue->SetSize(180,20);
        gridSizer->Add(lbPropertyName,1); //,wxEXPAND,0);
        gridSizer->Add(txtPropertyValue,1); //,wxEXPAND,0);

        this->arrProperties.push_back(lbPropertyName);
        this->arrProperties.push_back(txtPropertyValue);
      }

      // Deleta o mapa de propriedades da memoria
      delete lstProperties;

       // Adiciona um botao de atualizacao das propriedades
      wxButton * btUpdateModel = new wxButton(this->frmMainWindow->tabitemProperties,++id,wxT(" Update "));
      this->frmMainWindow->sizerProperties->Add(btUpdateModel);
      this->arrProperties.push_back(btUpdateModel);
      // Conecta o evento do botao da janela na aplicacao
      wxEvtHandler::Connect( id,    wxEVT_COMMAND_BUTTON_CLICKED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::TabProperties_On_UpdateButton_Click);

      // Faz o layout do controle
      this->frmMainWindow->tabitemProperties->Layout();
    }
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Eventos de wx3DModelPanel

void WorldBuilderApp::wx3DModelPanel_On_Model_Selected(wxModelSelectedEvent& e)
{
  try
  {
    if ( e.mouseMessageType == Mouse_Right_Up )
    {
      int j=0;
    }

    // Se apenas selecionou com o mouse, mostra as propriedades
    if (e.mouseMessageType == Mouse_Right_Down || e.mouseMessageType == Mouse_Right_Up)
    {
      // Obtem as propriedades do model
      map<string,string> * lstProperties = NULL;
      this->appVirtualWorld->GetPropertyList(e.model,&lstProperties);
      // Obtem a referencia para o modelo selecionado
      this->selected3DModel = e.model;

      // Limpa as propriedades da janela
      for(unsigned int i=0;i < this->arrProperties.size();i++)
      {
        delete this->arrProperties[i];
      }
      this->arrProperties.clear();  

      // Mostra as propriedades na janela principal    
      int id = 20;
      map<string,string>::iterator it; 
      for(it = lstProperties->begin();it != lstProperties->end(); it++)
      {
        wxString strPropertyName      = (*it).first.c_str();
        wxString strPropertyValue     = (*it).second.c_str();
        wxGridSizer * gridSizer       = this->frmMainWindow->sizerProperties;
        
        wxStaticText * lbPropertyName  = new wxStaticText(this->frmMainWindow->tabitemProperties,id++, strPropertyName);
        lbPropertyName->SetSize(80,20);
        wxTextCtrl * txtPropertyValue = new wxTextCtrl(this->frmMainWindow->tabitemProperties,id++,strPropertyValue);
        txtPropertyValue->SetSize(180,20);
        gridSizer->Add(lbPropertyName,1); //,wxEXPAND,0);
        gridSizer->Add(txtPropertyValue,1); //,wxEXPAND,0);

        this->arrProperties.push_back(lbPropertyName);
        this->arrProperties.push_back(txtPropertyValue);
      }
      // Deleta o mapa da memoria
      delete lstProperties;

      // Adiciona um botao de atualizacao das propriedades
      wxButton * btUpdateModel = new wxButton(this->frmMainWindow->tabitemProperties,++id,wxT(" Update "));
      this->frmMainWindow->sizerProperties->Add(btUpdateModel);
      this->arrProperties.push_back(btUpdateModel);
      // Conecta o evento do botao da janela na aplicacao
      wxEvtHandler::Connect( id,    wxEVT_COMMAND_BUTTON_CLICKED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &WorldBuilderApp::TabProperties_On_UpdateButton_Click);

      // For�a a atualizacao de layout do Notebook
      this->frmMainWindow->tabitemProperties->Layout();
    }
    // Se for arrasto do mouse, move a posicao do modelo 3D, de acordo
    // o contexto de visao que gerou o evento
    else if ( e.mouseMessageType == Mouse_Right_Dragging )
    {
      GzPose pose = e.model->GetPose();

      if (e.viewContextName == wxT("X/Y"))
      {
        pose.pos.x += e.velocityX;
        pose.pos.y += e.velocityY;
      }
      else if (e.viewContextName == wxT("Y/Z"))
      {
        pose.pos.y += e.velocityY;
        pose.pos.z += e.velocityY;
      }
      else if (e.viewContextName == wxT("X/Z"))
      {
        pose.pos.x += e.velocityX;
        pose.pos.z += e.velocityY;
      }

      e.model->SetPose(pose);

      //////////////////////////////////////////////////////////////////////////////
      //Se o modelo 3D � uma camera o ViewContext deve ser atualizado tambem      
      //////////////////////////////////////////////////////////////////////////////
      if ( ModelUtility::IsModelCamera(e.model) )
      {
        // Obtem o painel selecionado ( page )
        wx3DModelPanel * modelPanel = this->frmMainWindow->Get3DModelPanel();

        // Atualiza o contexto de visao da camera
        ViewContext * viewContext = modelPanel->worldGLCanvas->GetViewContext(e.model->GetId());
        //viewContext->eyePosition    = ((Camera*)e.model)->GetEyePosition();
        //viewContext->centerPosition = ((Camera*)e.model)->GetCenterPosition();
        //viewContext->upVector       = ((Camera*)e.model)->GetUpVector();
        viewContext->eyePosition    = ModelUtility::GetEyePosition(e.model);
        viewContext->centerPosition = ModelUtility::GetCenterPosition(e.model);
        viewContext->upVector       = ModelUtility::GetUpVector(e.model);
      }
    }
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::TabProperties_On_UpdateButton_Click(wxCommandEvent& e)
{
  try
  {
    // Recupera o modelo ( se houver )
    if ( this->selected3DModel == NULL )
    {
      throw new SystemException(wxT("N�o h� modelo 3D selecionado"),wxT("WorldBuilderApp::TabProperties_On_UpdateButton_Click"));
    }

    // Recupera a lista de propriedades dos text controls
    map<string,string> lstProperties;
    for (unsigned int i=0;i < this->arrProperties.size()-1; i+=2)
    {
      wxStaticText * label = (wxStaticText*)this->arrProperties[i];
      wxTextCtrl   * text  = (wxTextCtrl*)this->arrProperties[i+1];
      wxString strLabel = label->GetLabel();
      wxString strText  = text->GetLineText(0);
      lstProperties[string( strLabel.c_str() )] = string( strText.c_str() );
    }

    // Faz a atualizacao do modelo pela lista de propriedades
    this->appVirtualWorld->SetPropertyList(this->selected3DModel,&lstProperties);

    // Atualiza a visualizacao 3D
    wx3DModelPanel * modelPanel = this->frmMainWindow->Get3DModelPanel();
    modelPanel->worldGLCanvas->Draw();

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wx3DModelPanel_UpdateViewTypeCombobox(Model * model)
{
  if ( ModelUtility::IsModelCamera(model) )
  {
    // Obtem o painel selecionado ( page )
    wx3DModelPanel * modelPanel = this->frmMainWindow->Get3DModelPanel();
    // Transforma o modelo em camera
    /////////////Camera * camera = (Camera*)model;      
    // Adiciona uma nova visao ao componente de visualizacao 3D ( worldGLCanvas )
    // com o nome, a posicao e a orientacao da camera
    wxModelGLCanvas * glCanvas = modelPanel->worldGLCanvas;
    ViewContext * viewContext = new ViewContext();
    //viewContext->name = camera->GetId();
    viewContext->name = model->GetId();
    viewContext->orthographic = false;
    viewContext->eyePosition = GzVectorSet(0.0,0.0,5.0);
    viewContext->centerPosition = GzVectorSet(0.0,0.0,0.0);
    viewContext->upVector = GzVectorSet(0.0,1.0,0.0);
    //////////viewContext->camera = camera;
    viewContext->camera = model;

    glCanvas->AddViewContext(viewContext);

    // Adiciona o nome no combobox
    wxComboBox * viewTypeComboBox = modelPanel->cmbbxViewType; 
    viewTypeComboBox->Append(viewContext->name);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_Add3DChildModel(wxCommandEvent& e)
{
  try
  {
//     // Cria uma janela de dialogo
//     wx3DChildModelDialog * childModelDialog = new wx3DChildModelDialog(
//       this->appVirtualWorld->project->GetVirtualWorld()->GetModels(),
//       this->frmMainWindow,50,wxT("Selecione um modelo filho"));
//     childModelDialog->ShowModal();

     // Cria uma janela de dialogo
     wx3DChildModelDialog * childModelDialog = new wx3DChildModelDialog(
       this->appVirtualWorld->project->GetVirtualWorld(),
       this->frmMainWindow,50,wxT("Selecione um modelo filho"));
     childModelDialog->ShowModal();

    // Botao cancelar foi pressionado
    if (! childModelDialog->IsButtonAddPressed() )
    {
      return;
    }

    // Obtem os dados informados
    Model * parentModel = childModelDialog->Get3DParentModel();
    wxString strChildModelID    = childModelDialog->Get3DChildModelID();
    wxString strChildModelType  = childModelDialog->Get3DChildModelType();
    GzVector childModelPosition = childModelDialog->Get3DChildModelPosition();

    // Executa a aplica��o passando os parametros
    this->appVirtualWorld->AddChildModel(
      parentModel, 
      string(strChildModelType.c_str()), 
      string(strChildModelID.c_str()),
      childModelPosition);

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_Remove3DChildModel(wxCommandEvent& e)
{
  try
  {
    ///////////////////////////////////////////////////
    // Janela de Dialogo de Modelos Pais
    ///////////////////////////////////////////////////
    // Mostra um dialogo com o nome dos modelos 
    //int i3DModelCount = this->appIRCEProject->project->GetVirtualWorld()->GetModels()->size();
    int i3DModelCount = this->appIRCEProject->project->GetVirtualWorld()->GetNumModels();
    wxString * arrStr3DModelChoices = new wxString[i3DModelCount];
    for (int index=0;index < i3DModelCount; index++)
    {
      const char * strID = (this->appIRCEProject->project->GetVirtualWorld()->GetModels()[index])->GetId();
      arrStr3DModelChoices[index] = wxString( strID );
    }
    wxSingleChoiceDialog * dialog = new wxSingleChoiceDialog(
      this->frmMainWindow,
      wxT("Select Parent 3D Model from Virtual World"),
      wxT("Choose Id"),
      i3DModelCount,
      arrStr3DModelChoices);
    dialog->ShowModal();   
    // Recupera o ID do dialogo
    wxString strID = dialog->GetStringSelection();
    if ( strID == wxT("") )
    {
      return;
    }
    // Recupera um modelo 3D do projeto IRCE pelo ID
    Model * parentModel = this->appIRCEProject->project->GetVirtualWorld()->GetModel( strID.c_str() );

    ///////////////////////////////////////////////////
    // Janela de Dialogo de Modelos Pais
    ///////////////////////////////////////////////////
    // Mostra um dialogo com o nome dos modelos 
    //i3DModelCount = parentModel->GetModels().size();
    //arrStr3DModelChoices = new wxString[i3DModelCount];
    //for (int index=0;index < i3DModelCount; index++)
    //{
    //  string strID = parentModel->GetModels()[index]->GetId();
    //  arrStr3DModelChoices[index] = wxString( strID.c_str() );
    //}
    vector<Model*> * arrSubModels = ModelUtility::GetChildModels(parentModel);
    i3DModelCount = arrSubModels->size();
    arrStr3DModelChoices = new wxString[i3DModelCount];
    for (int index=0;index < i3DModelCount; index++)
    {
      string strID = (*arrSubModels)[index]->GetId();
      arrStr3DModelChoices[index] = wxString( strID.c_str() );
    }
    dialog = new wxSingleChoiceDialog(
      this->frmMainWindow,
      wxT("Select Child 3D Model from") + wxString( strID.c_str() ),
      wxT("Remove Child Model"),
      i3DModelCount,
      arrStr3DModelChoices);
    dialog->ShowModal();   
    // Recupera o ID do dialogo
    strID = dialog->GetStringSelection();
    if ( strID == wxString("") )
    {
      return;
    }
    // Recupera um modelo 3D do projeto IRCE pelo ID
    Model * childModel = NULL;
    //for (int index2=0; index2 < parentModel->GetModels().size(); index2++)
    //{
    //  if ( parentModel->GetModels()[index2]->GetId() == string( strID.c_str() ) )
    //  {
    //    childModel = parentModel->GetModels()[index2];
    //  }
    //}
    for (unsigned int index2=0; index2 < arrSubModels->size(); index2++)
    {
      if ( (*arrSubModels)[index2]->GetId() == string( strID.c_str() ) )
      {
        childModel = (*arrSubModels)[index2];
      }
    }
    delete arrSubModels;

    ////////////////////////////////////////////////
    // Executa a aplicacao
    ////////////////////////////////////////////////
    this->appVirtualWorld->RemoveChildModel(parentModel,childModel); 
  
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_NewControlProgram_Click(wxCommandEvent& e)
{
  try
  {
    //// Mostra um Dialog para informar o programa de controle
    //wxClientProgramDialog * clientProgramDialog = new wxClientProgramDialog(
    //  this->frmMainWindow,400,"Informe um Programa de Controle");
    //clientProgramDialog->SetSize(400,150);
    //clientProgramDialog->ShowModal();
    //if ( ! clientProgramDialog->IsButtonOkPressed() )
    //{
    //  return;
    //}    
    //// Obtem os dados da janela
    //wxString strPath   = clientProgramDialog->GetPath();
    ////Model *  model     = clientProgramDialog->GetModel();
    ////wxString strDriver = clientProgramDialog->GetDriver();
    ////wxString strPort   = clientProgramDialog->GetPort();
    wxFileDialog fileDialog(this->frmMainWindow,wxT("Informe o arquivo fonte do programa de controle"),
      wxT("."),wxEmptyString,wxT("*.cpp"));
    fileDialog.ShowModal();
    if ( fileDialog.GetPath() == "" ) { return; }
    wxString strPath = fileDialog.GetPath();

    // Executa a aplicacao passando os parametros necessarios
    this->appControlProgram->AddControlProgram(string( strPath.c_str() ));// , model ,string( strDriver.c_str() ),string( strPort.c_str() ) );

    // Obtem o Programa de Controle Adicionado no projeto
    PlayerControl * playerControl = this->appControlProgram->project->GetPlayerControlProgram( string( strPath.c_str() ) );
    // Adiciona o elemento na arvore
    this->frmMainWindow->AddControlProgram(playerControl);

    // Cria um painal para armazenar o programa de controle
    wxClientControlPanel * clientPanel = new wxClientControlPanel(
      this->frmMainWindow->tabModel,
      -1,
      playerControl);
    this->frmMainWindow->tabModel->AddPage( clientPanel, wxFileNameFromPath( wxString( playerControl->GetClientControlFile().c_str() ) ) );
    this->frmMainWindow->tabModel->SetSelection( this->frmMainWindow->tabModel->GetPageCount() -1 );
    this->menuBar->SetState(CLIENT_CONTROL_SELECTED);

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxIRCEMenuBar_LoadControlProgram_Click(wxCommandEvent& e)
{
  try
  {
//* POR ACASO, A CONFIGURACAO ENTRE O CONTROLE E O MODELO ESTA SENDO CRIADA ADEQUADAMENTE ???
//* DEPOIS, COMPILE CONTROL, SIMULATE E TRANSFER CONTROL

    //// Mostra um Dialog para informar o programa de controle
    //wxClientProgramDialog * clientProgramDialog = new wxClientProgramDialog(
    //  this->frmMainWindow,400,"Informe um Programa de Controle");
    //clientProgramDialog->SetSize(400,150);
    //clientProgramDialog->ShowModal();
    //if ( ! clientProgramDialog->IsButtonOkPressed() )
    //{
    //  return;
    //}    
    //// Obtem os dados da janela
    //wxString strPath   = clientProgramDialog->GetPath();
    ////Model *  model     = clientProgramDialog->GetModel();
    ////wxString strDriver = clientProgramDialog->GetDriver();
    ////wxString strPort   = clientProgramDialog->GetPort(); 

    wxFileDialog fileDialog(this->frmMainWindow,wxT("Informe o arquivo fonte do programa de controle"),
      wxT("."),wxEmptyString,wxT("*.cpp"));
    fileDialog.ShowModal();
    if ( fileDialog.GetPath() == "" ) { return; }
    wxString strPath = fileDialog.GetPath();

    // Executa a aplicacao passando os parametros necessarios
    this->appControlProgram->LoadControlProgram(string( strPath.c_str() ));// , model ,string( strDriver.c_str() ),string( strPort.c_str() ) );

    // Obtem o Programa de Controle Adicionado no projeto
    PlayerControl * playerControl = this->appControlProgram->project->GetPlayerControlProgram( string( strPath.c_str() ) );

    // Adiciona o elemento na arvore de arquivos do painel a direita
    this->frmMainWindow->AddControlProgram(playerControl);

    // Cria um painal para armazenar o programa de controle
    wxClientControlPanel * clientPanel = new wxClientControlPanel(
      this->frmMainWindow->tabModel,
      -1,
      playerControl);
    this->frmMainWindow->tabModel->AddPage( clientPanel, wxFileNameFromPath( wxString( playerControl->GetClientControlFile().c_str() ) ) );
    this->frmMainWindow->tabModel->SetSelection( this->frmMainWindow->tabModel->GetPageCount() -1 );
    this->menuBar->SetState(CLIENT_CONTROL_SELECTED);

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxIRCEMenuBar_SaveControlProgram_Click(wxCommandEvent& e)
{
  try
  {
    // Obtem o painel selecionado ( page )
    wxClientControlPanel * clientControlPanel = this->frmMainWindow->GetCurrentClientControlPanel();
    PlayerControl * playerControl = clientControlPanel->GetPlayerControl();
   
    // Chama a aplicacao correspondente
    this->appControlProgram->SaveControlProgram( playerControl );

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxIRCEMenuBar_RemoveControlProgram_Click(wxCommandEvent& e)
{
  try
  {
    // Obtem o painel selecionado ( page )
    wxClientControlPanel * clientControlPanel = this->frmMainWindow->GetCurrentClientControlPanel();
    PlayerControl * playerControl = clientControlPanel->GetPlayerControl();
    // Pede confirmacao da acao pelo usuario
    wxMessageDialog * messageDialog  = new wxMessageDialog(this->frmMainWindow,"Confirma exclus�o de arquivo de controle ?","Aviso",wxYES_NO);
    if ( messageDialog->ShowModal() != wxOK )
    {
      return;
    }
    // Chama a aplicacao correspondente
    this->appControlProgram->RemoveControlProgram( playerControl );
    // Remove o painel da janela principal
    this->frmMainWindow->tabModel->RemovePage( this->frmMainWindow->tabModel->GetSelection() );
    delete clientControlPanel;
    // Atualiza o menu de acordo com o painel selecionado correntemente
    wxNotebookPage * page = this->frmMainWindow->tabModel->GetPage( this->frmMainWindow->tabModel->GetSelection() );
    // Configura o menu de acordo com o painel selecionado
    if ( dynamic_cast<wx3DModelPanel*>(page) )
    {
      this->menuBar->SetState(VIRTUAL_WORLD_SELECTED);
    }
    else if ( dynamic_cast<wxClientControlPanel*>(page) )
    {
      this->menuBar->SetState(CLIENT_CONTROL_SELECTED);
    }
    else if ( dynamic_cast<wxConfigurationPanel*>(page) )
    {  
      this->menuBar->SetState(CONFIGURATION_SELECTED);
    }
    else
    {
      throw new SystemException("WorldBuilderApp::wxIRCEMenuBar_RemoveConfigurationFile_Click","Tipo de painel desconhecido");
    }
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_CompileControlProgram_Click(wxCommandEvent& e)
{
  try
  {
    // Obtem o painel selecionado ( page )
    wxClientControlPanel * clientControlPanel = this->frmMainWindow->GetCurrentClientControlPanel();

    // Atribui o texto na interface grafica para o playerControl
    int lineCount = clientControlPanel->txtClientControl->GetNumberOfLines();
    vector<string> arrSourceCode;
    for (int i=0; i < lineCount; i++)
    {
      const char * sourceLine = clientControlPanel->txtClientControl->GetLineText(0).c_str();
      string strSourceLine = sourceLine;
      arrSourceCode.push_back( strSourceLine );
    }
    PlayerControl * playerControl = clientControlPanel->GetPlayerControl();
    playerControl->SetSourceCode(arrSourceCode);

    // Faz uma Chamada a aplicacao do Programa de Controle
    vector<string> arrStrOutput;
    vector<string> arrStrError;
    this->appControlProgram->CompileControlProgram(
      playerControl,
      arrStrOutput,
      arrStrError);

    // Mostra a sa�da do comando na interface grafica
    wxString strOutput;
    for (unsigned int a=0; a < arrStrOutput.size(); a++)
    {
      strOutput.Append(wxString(arrStrOutput[a].c_str()));
    }
    for (unsigned  int b=0; b < arrStrError.size(); b++)
    {
      strOutput.Append(wxString(arrStrError[b].c_str()));
    }
    clientControlPanel->txtOutput->SetValue(strOutput);

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxIRCEMenuBar_NewConfigurationFile_Click(wxCommandEvent& e)
{
  try
  {
    // Dialogo padrao onde o usuario informa o caminho do
    // arquivo de configuracao
    wxFileDialog fileDialog(this->frmMainWindow,wxT("Informe o arquivo de configura�ao"),
      wxT("."),wxEmptyString,wxT("*.cfg"));
    fileDialog.ShowModal();
    if ( fileDialog.GetPath() == "" ) { return; }

    // Se o caminho foi informado, chama a aplicacao passando o caminho 
    string strPath = string( fileDialog.GetPath().c_str() );
    this->appConfiguration->AddConfigurationFile( strPath );

    // Cria um painel com os dados do novo arquivo de configuracao
    ConfigurationFile * configurationFile = this->appIRCEProject->project->GetConfigurationFile( strPath );
//     wxConfigurationPanel * clientPanel = new wxConfigurationPanel(
//       this->frmMainWindow->tabModel,
//       10,
//       configurationFile,
//       &this->appIRCEProject->project->GetVirtualWorld()->GetModels());

    wxConfigurationPanel * clientPanel = new wxConfigurationPanel(
      this->frmMainWindow->tabModel,
      10,
      configurationFile);

    // Conecta os eventos no Controller WorldBuilder
    // DEVE TER O MESMO ID DO wx3DModelPanel !!!!
    wxEvtHandler:Connect(11,11, wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                 &WorldBuilderApp::wxConfigurationPanel_On_SetSimulationConfiguration_Button_Click );
    wxEvtHandler::Connect(12,12, wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                 &WorldBuilderApp::wxConfigurationPanel_On_AddConfigurationPlayer_Button_Click );
    wxEvtHandler::Connect(13,13, wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                 &WorldBuilderApp::wxConfigurationPanel_On_UpdateConfigurationPlayer_Button_Click );
    wxEvtHandler::Connect(14,14, wxEVT_COMMAND_BUTTON_CLICKED,
                 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                 &WorldBuilderApp::wxConfigurationPanel_On_RemoveConfigurationPlayer_Button_Click );
    // Adiciona e seleciona o painel na janela principal
    this->frmMainWindow->tabModel->AddPage( clientPanel, wxFileNameFromPath( wxString( configurationFile->GetConfigurationFile().c_str() ) ) );
    this->frmMainWindow->tabModel->SetSelection( this->frmMainWindow->tabModel->GetPageCount() -1 );
    // Atualiza o menu
    this->menuBar->SetState(CONFIGURATION_SELECTED);

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxIRCEMenuBar_LoadConfigurationFile_Click(wxCommandEvent& e)
{
  try
  {
    // Dialogo padrao onde o usuario informa o caminho do
    // arquivo de configuracao
    wxFileDialog fileDialog(this->frmMainWindow,wxT("Informe o arquivo de configura�ao"),
      wxT("."),wxEmptyString,wxT("*.cfg"));
    fileDialog.ShowModal();
    if ( fileDialog.GetPath() == "" ) { return; }
    // Se o caminho foi informado, chama a aplicacao passando o caminho 
    this->appConfiguration->LoadConfigurationFile( string( fileDialog.GetPath().c_str() ) );
    // Cria um painel com os dados do novo arquivo de configuracao
    ConfigurationFile * configurationFile = this->appIRCEProject->project->GetConfigurationFile( string( fileDialog.GetPath().c_str() ) );

//     wxConfigurationPanel * clientPanel = new wxConfigurationPanel(
//       this->frmMainWindow->tabModel,
//       -1,
//       configurationFile,
//       &this->appIRCEProject->project->GetVirtualWorld()->GetModels());

    wxConfigurationPanel * clientPanel = new wxConfigurationPanel(
      this->frmMainWindow->tabModel,
      -1,
      configurationFile);

    this->frmMainWindow->tabModel->AddPage( clientPanel, wxFileNameFromPath( wxString( configurationFile->GetConfigurationFile().c_str() ) ) );
    this->frmMainWindow->tabModel->SetSelection( this->frmMainWindow->tabModel->GetPageCount() -1 );
    this->menuBar->SetState(CONFIGURATION_SELECTED);
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxIRCEMenuBar_SaveConfigurationFile_Click(wxCommandEvent& e)
{
  try
  {
    // Obtem o painel selecionado ( page )
    wxConfigurationPanel * configurationPanel = this->frmMainWindow->GetCurrentConfigurationPanel();
    ConfigurationFile * configurationFile = configurationPanel->GetConfigurationFile();
    // Numero o numero da porta no Painel de arquivo de configuracao selecionado
    wxTextCtrl * txtPortNumber = this->frmMainWindow->GetCurrentConfigurationPanel()->txtPortNumber;
    wxString strPortNumber = txtPortNumber->GetLineText(0);
    string portNumber = string( strPortNumber.c_str() );
    // Chama a aplicacao e salva a configuracao de programa
    this->appConfiguration->SaveConfigurationFile(portNumber,configurationFile); 
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxIRCEMenuBar_RemoveConfigurationFile_Click(wxCommandEvent& e)
{
  try
  {
    // Obtem o painel selecionado ( page )
    wxConfigurationPanel * configurationPanel = this->frmMainWindow->GetCurrentConfigurationPanel();
    ConfigurationFile * configurationFile = configurationPanel->GetConfigurationFile();
    // Pede confirmacao da acao pelo usuario
    wxMessageDialog * messageDialog  = new wxMessageDialog(this->frmMainWindow,"Confirma exclus�o de arquivo de configuracao ?","Aviso",wxYES_NO);
    if ( messageDialog->ShowModal() != wxID_YES)
    {
      return;
    }
    // Remove o arquivo de configuracao
    this->appConfiguration->RemoveConfigurationFile(configurationFile);
    // Remove o painel da janela
    this->frmMainWindow->tabModel->RemovePage( this->frmMainWindow->tabModel->GetSelection() );
    delete configurationPanel;
    // Atualiza o menu de acordo com o painel selecionado correntemente
    wxNotebookPage * page = this->frmMainWindow->tabModel->GetPage( this->frmMainWindow->tabModel->GetSelection() );
    // Configura o menu de acordo com o painel selecionado
    if ( dynamic_cast<wx3DModelPanel*>(page) )
    {
      this->menuBar->SetState(VIRTUAL_WORLD_SELECTED);
    }
    else if ( dynamic_cast<wxClientControlPanel*>(page) )
    {
      this->menuBar->SetState(CLIENT_CONTROL_SELECTED);
    }
    else if ( dynamic_cast<wxConfigurationPanel*>(page) )
    {  
      this->menuBar->SetState(CONFIGURATION_SELECTED);
    }
    else
    {
      throw new SystemException("WorldBuilderApp::wxIRCEMenuBar_RemoveConfigurationFile_Click","Tipo de painel desconhecido");
    }
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxConfigurationPanel_On_SetSimulationConfiguration_Button_Click(wxCommandEvent& e)
{
  try
  {
    // Mostra o janela de dialogo de Configuracao de Simulacao
    wxSimulationConfigurationDialog * simulationConfigurationDialog = new wxSimulationConfigurationDialog( this->frmMainWindow, 10, wxT("Informe a configuracao da simulacao"));
    // Obtem o painel selecionado ( page )
    wxConfigurationPanel * configurationPanel = this->frmMainWindow->GetCurrentConfigurationPanel();
    ConfigurationFile * configurationFile = configurationPanel->GetConfigurationFile();
    // Atribuir os dados a janela
    if ( configurationFile->GetSimulationConfiguration() == NULL )
    {
      configurationFile->SetSimulationConfiguration(new SimulationConfiguration());
    }
    SimulationConfiguration * simulationConfiguration = configurationFile->GetSimulationConfiguration();
    simulationConfigurationDialog->txtName->SetValue( wxString( simulationConfiguration->GetName().c_str() ) );
    simulationConfigurationDialog->txtProvides->SetValue( wxString( simulationConfiguration->GetDevice().c_str() ) );
    simulationConfigurationDialog->txtPlugIn->SetValue( wxString( simulationConfiguration->GetPlugIn().c_str() ) );
    simulationConfigurationDialog->txtServerId->SetValue( wxString( simulationConfiguration->GetServerID().c_str() ) );
    int iReturn  = simulationConfigurationDialog->ShowModal();
    if ( iReturn != wxID_OK )
    {
      return;
    }
    // Obtem os dados informados na janela
    string strName     = string( simulationConfigurationDialog->txtName->GetValue().c_str() );
    string strDevice   = string( simulationConfigurationDialog->txtProvides->GetValue().c_str() );
    string strPlugIn   = string( simulationConfigurationDialog->txtPlugIn->GetValue().c_str() );
    string strServerId = string( simulationConfigurationDialog->txtServerId->GetValue().c_str() );   
    // Chama a aplicacao passando os dados do arquivo de configuracao
    this->appConfiguration->SetSimulationConfiguration(strName,strDevice,strPlugIn,strServerId,configurationFile);    
    // Atualiza o grid 
    configurationPanel->gridServerDriver->SetCellValue(0,0,wxString( strName.c_str() ) );
    configurationPanel->gridServerDriver->SetCellValue(0,1,wxString( strDevice.c_str() ) );
    configurationPanel->gridServerDriver->SetCellValue(0,2,wxString( strPlugIn.c_str() ) );
    configurationPanel->gridServerDriver->SetCellValue(0,3,wxString( strServerId.c_str() ) );
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::wxConfigurationPanel_On_AddConfigurationPlayer_Button_Click(wxCommandEvent& e)
{
  try
  {
    // Executa o dialog para obter os dados da configuracao de modelo
    wxPlayerConfigurationDialog * playerConfigurationDialog = new wxPlayerConfigurationDialog(
      this->frmMainWindow,20,
      wxT("Informe os dados da configuracao do modelo 3D"),
      this->appIRCEProject->project->GetVirtualWorld());
    if ( playerConfigurationDialog->ShowModal() != wxID_OK )
    {
      return;
    }
    // Obtem os dados da nova configuracao informados pelo usuario no dialog
    string strName    = string( playerConfigurationDialog->txtName->GetValue().c_str() );
    string strDevice  = string( playerConfigurationDialog->txtProvides->GetValue().c_str() );
    string strModelID = string( playerConfigurationDialog->cmbbxGzId->GetStringSelection().c_str() );
    Model * model = this->appIRCEProject->project->GetVirtualWorld()->GetModel( strModelID.c_str() );
    // Obtem o painel selecionado ( page )
    wxConfigurationPanel * configurationPanel = this->frmMainWindow->GetCurrentConfigurationPanel();
    ConfigurationFile * configurationFile = configurationPanel->GetConfigurationFile();
    // Chama a aplicacao passando os dados acima
    this->appConfiguration->AddPlayerConfigurationFile(strName,strDevice,model,configurationFile);    
    // Atualiza o grid com a configuracao nova
    configurationPanel->gridModelDriver->AppendRows();
    int row = configurationPanel->gridModelDriver->GetRows()-1;
    configurationPanel->gridModelDriver->SetCellValue(row,0,strName.c_str());
    configurationPanel->gridModelDriver->SetCellValue(row,1,strDevice.c_str());
    configurationPanel->gridModelDriver->SetCellValue(row,2,strModelID.c_str());
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxConfigurationPanel_On_UpdateConfigurationPlayer_Button_Click(wxCommandEvent& e)
{
  try
  {    
    // Faz o casting do painel de configuracao correspondente
    wxConfigurationPanel * configurationPanel = this->frmMainWindow->GetCurrentConfigurationPanel();
    // Obtem o grid de configuracoes de modelo
    wxGrid * gridModelDriver = configurationPanel->gridModelDriver;
    wxArrayInt selectedRows = gridModelDriver->GetSelectedRows();    
    // Verifica se pelo menos uma linha foi selecionada
    if ( selectedRows.GetCount() == 0 )
    {
      throw new ApplicationException("Selecione qual configuracao de modelo no grid que se deseja atualizar os dados");
    }
    // Obtem o numero da ( 1a ) linha selecionada
    int row = selectedRows[0];
    // Obtem o id do modelo, o driver e o dispositivo da configuracao na linha selecionada no grid
    wxString strDriverName = gridModelDriver->GetCellValue(row,0);
    wxString strDeviceName = gridModelDriver->GetCellValue(row,1);
    wxString strModelId    = gridModelDriver->GetCellValue(row,2);
    
    // Obtem a configuracao do id de modelo obtido acima no arquivo 
    // de configuracao que esta no panel selecionado
    ConfigurationFile * configurationFile = configurationPanel->GetConfigurationFile();
    // Obtem a configuracao de modelo 
    PlayerConfiguration * playerConfiguration = configurationFile->GetPlayerConfiguration(
                                                                       string(strDriverName.c_str()),
                                                                       string(strDeviceName.c_str()),
                                                                       string(strModelId.c_str()) );
    // Cria a janela de dialogo de configuracao de modelo
/*    wxPlayerConfigurationDialog * playerConfigurationDialog = new wxPlayerConfigurationDialog(
      this->frmMainWindow,
      -1,
      "Atualize os dados da configuracao",
      this->appIRCEProject->project->GetVirtualWorld()->GetModels());*/
    wxPlayerConfigurationDialog * playerConfigurationDialog = new wxPlayerConfigurationDialog(
      this->frmMainWindow,
      -1,
      "Atualize os dados da configuracao",
      this->appIRCEProject->project->GetVirtualWorld());

    // Passa os dados da configuracao selecionada no grid
    playerConfigurationDialog->txtName->SetValue(wxString(playerConfiguration->GetDriverName().c_str()));
    playerConfigurationDialog->txtProvides->SetValue(wxString(playerConfiguration->GetDeviceName().c_str()));
    playerConfigurationDialog->cmbbxGzId->SetStringSelection(strModelId);
    // Executa o dialogo
    if (playerConfigurationDialog->ShowModal() != wxID_OK)
    {
      return;    
    }    
    // Obtem os dados informados no dialogo
    wxString strName     = playerConfigurationDialog->txtName->GetValue();
    wxString strProvides = playerConfigurationDialog->txtProvides->GetValue();
    wxString strGzId     = playerConfigurationDialog->cmbbxGzId->GetStringSelection();
    Model*   model       = this->appIRCEProject->project->GetVirtualWorld()->GetModel( strGzId.c_str() );
    if ( model == NULL )
    {
      throw new SystemException("WorldBuilderApp::wxConfigurationPanel_On_UpdateConfigurationPlayer_Button_Click","Nao existe nenhum modelo com o ID informado");
    }
    // Executa a aplicacao passando os dados necessarios
    this->appConfiguration->UpdatePlayerConfigurationFile(
      string( strName.c_str() ),
      string( strProvides.c_str() ),      
      model,
      playerConfiguration,
      configurationFile);
    // Atualiza a linha do grid
    configurationPanel->gridModelDriver->SetCellValue(row,0,strName);
    configurationPanel->gridModelDriver->SetCellValue(row,1,strProvides);
    configurationPanel->gridModelDriver->SetCellValue(row,2,strGzId);
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}
void WorldBuilderApp::wxConfigurationPanel_On_RemoveConfigurationPlayer_Button_Click(wxCommandEvent& e)
{
  try
  {
    // Obtem o painel selecionado ( page ) para obter o elemento selecionado no grid
    wxConfigurationPanel * configurationPanel = this->frmMainWindow->GetCurrentConfigurationPanel();
    // Obtem o grid de configuracoes de modelo
    wxGrid * gridModelDriver = configurationPanel->gridModelDriver;
    wxArrayInt selectedRows = gridModelDriver->GetSelectedRows();
    //POR QUE ESSA $%#$% NAO FUNCIONA PARA A 2a LINHA !!
    // Verifica se pelo menos uma linha foi selecionada
    if ( selectedRows.GetCount() == 0 )
    {
      throw new ApplicationException("Selecione qual configuracao de modelo no grid que se deseja excluir");
    }
    // Obtem o numero da ( 1a ) linha selecionada
    int row = selectedRows[0];
    // Obtem o id do modelo da configuracao na linha selecionada no grid
    wxString strDriverName = gridModelDriver->GetCellValue(row,0);
    wxString strDeviceName = gridModelDriver->GetCellValue(row,1);
    wxString strModelId    = gridModelDriver->GetCellValue(row,2);
    // Obtem a configuracao do id de modelo obtido acima no arquivo 
    // de configuracao que esta no panel selecionado
    ConfigurationFile * configurationFile = configurationPanel->GetConfigurationFile();
    // Obtem a configuracao de modelo 
    PlayerConfiguration * playerConfiguration = configurationFile->GetPlayerConfiguration(
                                                                       string(strDriverName.c_str()),
                                                                       string(strDeviceName.c_str()),
                                                                       string(strModelId.c_str()) );
    // Pede confirmacao da acao pelo usuario
    wxMessageDialog * messageDialog  = new wxMessageDialog(this->frmMainWindow,"Confirma exclus�o da configuracao desse modelo ?","Aviso",wxYES_NO);
    if ( messageDialog->ShowModal() != wxID_YES )
    {
      return;
    }
    // Executa a aplicacao correspondente
    this->appConfiguration->RemovePlayerConfigurationFile(playerConfiguration,configurationFile);
    // Atualiza o grid com a configuracao nova
    configurationPanel->gridModelDriver->DeleteRows(row);
    configurationPanel->gridModelDriver->Layout();

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}


void WorldBuilderApp::TreeControlItems_Item_Activated(wxTreeEvent& e)
{
  try
  {
    wxTreeItemId treeId = e.GetItem();

    int tabItemPageNumber = this->frmMainWindow->GetNotebookPage(treeId);
    
    // Se for o item de um mundo virtual for selecionado, mostrar ou dar foco ao tab
    if (tabItemPageNumber == -1)
    {
      throw new SystemException("WorldBuilderApp::TreeControlItems_Item_Activated","Erro no software: Item selecionado da arvore n�o possui um painel correspondente");
    }

    this->frmMainWindow->tabModel->SetSelection( tabItemPageNumber );

    // Atualiza o menu
    if ( treeId == this->frmMainWindow->GetVirtualWorldId() )
    {
      this->menuBar->SetState(VIRTUAL_WORLD_SELECTED);
    }
    else if (( treeId != this->frmMainWindow->GetConfigurationId() )&&
            ( treeId != this->frmMainWindow->GetProjectId() ))
    {
      this->menuBar->SetState(CLIENT_CONTROL_SELECTED);
    }

  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

void WorldBuilderApp::TabModel_Page_Changed(wxNotebookEvent& e)
{
  try
  {
    int currentPageSelected = e.GetSelection();
    wxNotebookPage * page = this->frmMainWindow->tabModel->GetPage(currentPageSelected);
    if ( dynamic_cast<wx3DModelPanel*>(page) )
    {
      this->menuBar->SetState(VIRTUAL_WORLD_SELECTED);
    }
    else if ( dynamic_cast<wxClientControlPanel*>(page) )
    {
      this->menuBar->SetState(CLIENT_CONTROL_SELECTED);
    }
    else
    {
      this->menuBar->SetState(CONFIGURATION_SELECTED);
    }
  }
  catch(Exception * e)
  {
    ExceptionHandler::Handler(e);
  }
}

