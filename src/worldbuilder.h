
#ifndef _WORLDBUILDER_H_
#define _WORLDBUILDER_H_

// utilizando a vers�qo wxWidgets 2.4.0

#include <wx/wx.h>

#include "wxIRCEMainWindow.h"
#include "wxModelGLCanvas.h"
#include "wxIRCEMenuBar.h"

#include "AppIRCEProject.h"
#include "AppVirtualWorld.h"
#include "AppControlProgram.h"
#include "AppConfiguration.h"

/**
 * @short Application Main Window
 * @author Humberto Cardoso Marchezi <hcmarchezi@linux>
 * @version 0.1
 */

// Esta classe funciona como o Controller da aplicacao
// de acordo com o padrao MVC
class WorldBuilderApp : public wxApp
{
// Variaveis de interface gr�fica ( View )
private: wxIRCEMenuBar * menuBar;
private: wxIRCEMainWindow * frmMainWindow;

// Variaveis de aplicacao ( Model )
// Variaveis necessarias para calcular as coordenadas esfericas
// arrastando a posicao do mouse pelo botao esquerdo
private: GLdouble velocityX;
private: GLdouble velocityY;
private: GLdouble angleX;
private: GLdouble angleY;
private: GLdouble zoom;
private: wxPoint  mousePosition;

// Modelo 3D selecionada para ter as suas propriedades mostradas
private: Model * selected3DModel;

//////////// Construtor, Destrutor e Iniciacao do Controller
public: WorldBuilderApp(void);
public: virtual ~WorldBuilderApp(void);
public: virtual bool OnInit();

// Aplicacao ( model em MVC ) do cadastramento de projetos
private: AppIRCEProject*  appIRCEProject;
// Eventos de WorldBuilderMenuBar ligados a appIRCEProject
public: void wxIRCEMenuBar_NewProject_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_EditProject_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_LoadProject_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_SaveProject_Click(wxCommandEvent& e);

public: void wxIRCEMenuBar_SimulateProject_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_TransferControl_Click(wxCommandEvent& e);

public: void wxIRCEMenuBar_Exit_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_About_Click(wxCommandEvent& e);

// Aplicacao ( model em MVC ) da montagem do mundo virtual
private: AppVirtualWorld* appVirtualWorld;
// Lista de Static texts de propriedades de um model selecionado pelo mouse
private: vector<wxWindow *> arrProperties;
// Eventos tratados pela aplicacao acima 
public: void wxIRCEMenuBar_NewWorldFile_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_CleanVirtualWorld_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_LoadWorldFile_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_SaveWorldFile_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_Add3DModel_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_Remove3DModel_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_Select3DModel_Click(wxCommandEvent& e);
public: void wx3DModelPanel_On_Model_Selected(wxModelSelectedEvent& e);
public: void TabProperties_On_UpdateButton_Click(wxCommandEvent& e);
private: void wx3DModelPanel_UpdateViewTypeCombobox(Model * model);
private: void wxIRCEMenuBar_Add3DChildModel(wxCommandEvent& e);
private: void wxIRCEMenuBar_Remove3DChildModel(wxCommandEvent& e);

// Aplicacao ( model em MVC ) do programa de controle dos modelos 3D
private: AppControlProgram* appControlProgram;
// Eventos tratados pela aplicacao acima
public: void wxIRCEMenuBar_NewControlProgram_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_LoadControlProgram_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_SaveControlProgram_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_RemoveControlProgram_Click(wxCommandEvent& e);
public: void wxIRCEMenuBar_CompileControlProgram_Click(wxCommandEvent& e);

// Aplicacao ( model em MVC ) dos arquivos de configuracao
private: AppConfiguration * appConfiguration;
public:  void wxIRCEMenuBar_NewConfigurationFile_Click(wxCommandEvent& e);
public:  void wxIRCEMenuBar_LoadConfigurationFile_Click(wxCommandEvent& e);
public:  void wxIRCEMenuBar_SaveConfigurationFile_Click(wxCommandEvent& e);
public:  void wxIRCEMenuBar_RemoveConfigurationFile_Click(wxCommandEvent& e);

public:  void wxConfigurationPanel_On_SetSimulationConfiguration_Button_Click(wxCommandEvent& e);
public:  void wxConfigurationPanel_On_AddConfigurationPlayer_Button_Click(wxCommandEvent& e);
public:  void wxConfigurationPanel_On_UpdateConfigurationPlayer_Button_Click(wxCommandEvent& e);
public:  void wxConfigurationPanel_On_RemoveConfigurationPlayer_Button_Click(wxCommandEvent& e);

// treeControl
public: void TreeControlItems_Item_Activated(wxTreeEvent& e);

// TabModel
public: void TabModel_Page_Changed(wxNotebookEvent& e);

// wxClientControlPanel

// wxConfigurationPanel

// FrmMainWindow

public: void LoadConfigurations();


};

enum WXIRCEMAINWINDOW_MESSAGES
{
  TREECONTROL_ITEMACTIVATED = 200,
  TABMODEL_TABITEM_CHANGED = 201
};


#endif // _WORLDBUILDER_H_


