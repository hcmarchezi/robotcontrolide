
#include "AppIRCEProject.h"
///////////////////////////////////#include "Exception.h"
#include "ExceptionHandler.h"
#include "ApplicationException.h"
#include "IRCEConfiguration.h"

#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/process.h>

#include <vector>

#include "Interval.h"
#include "IRCEConfiguration.h"

AppIRCEProject::AppIRCEProject(void)
{
  this->project = NULL;
}

AppIRCEProject::~AppIRCEProject(void)
{
  if (this->project != NULL)
  {
    delete this->project;
  }
}

void AppIRCEProject::CreateIRCEProject(string fullPath)
{
  // Verifica se ja existe um projeto com o path informado
  // COMO FAZER ISSO ?

  // Cria um projeto IRCE com o path informado
  this->project = new IRCEProject();
  this->project->SetProjectFullPath(fullPath);
  // Diretorio de trabalho padrao passa a ser o diretorio do 
  // arquivo de projeto
  wxSetWorkingDirectory( this->project->GetProjectDirectory().c_str() );
}

void AppIRCEProject::EditIRCEProject(string fullPath)
{
  this->project->SetProjectFullPath(fullPath);
}

void AppIRCEProject::SaveIRCEProject()
{
  // Salva o projeto corrente
  this->project->SaveProjecFile();
}

void AppIRCEProject::LoadIRCEProject(string fullPath)
{
  // Verifica se existe um projeto corrente e se este esta salvo
  if ( this->project != NULL )
  {
    if (! this->project->IsSaved() )
    {
      wxMessageDialog messageBox(NULL, wxT("O projeto corrente nao esta salvo e as alteracoes serao perdidas. Continuar assim mesmo ? "), wxT("Confirmacao"), wxYES_NO );
      // Se nao confirma, cancela o carregamento do arquivo
      if ( messageBox.ShowModal() == wxNO )
      {
        return;
      }
    }
  }
  // Destroi projeto corrente, se houver
  if ( this->project != NULL)
  {
    delete this->project;
  }
  // Carrega um projeto IRCE atraves de um arquivo informado
  this->project = IRCEProject::LoadProject(fullPath);
  // Diretorio de trabalho padrao passa a ser o diretorio do 
  // arquivo de projeto
  wxSetWorkingDirectory( this->project->GetProjectDirectory().c_str() );
}

void AppIRCEProject::SimulateIRCEProject(wxEvtHandler * parentWindow)
{
  //////////////////////////////////////////////////////////
  // Verifica se o projeto esta consistente para simulacao
  //////////////////////////////////////////////////////////
  
  // Verifica se o projeto corrente possui um arquivo World
  if ( this->project->GetVirtualWorld() == NULL )
  {
    throw new ApplicationException(" Simulacao nao pode ocorrer pois nao ha mundo virtual.");
  }
  // Verifica se todos os programas de controle clientes foram compilados
  vector<PlayerControl*> arrPlayerControls = this->project->GetPlayerControlPrograms();
  string strClientPrograms = "";
  for (unsigned int i=0; i < arrPlayerControls.size(); i++)
  {
    if ( arrPlayerControls[i]->IsCompiled() == false )
    {
      strClientPrograms.append( wxFileNameFromPath( arrPlayerControls[i]->GetClientControlFile().c_str() ) );
      strClientPrograms.append(" \n");
    }
  }
  // Se houverem controles nao compilados, mostra uma mensagem de erro
  if ( strClientPrograms != "" )
  {
    string strErrorMessage = "Simulacao nao pode ocorrer enquanto os controles abaixo nao forem compilados.\n";
    strErrorMessage.append( strClientPrograms );
    throw new ApplicationException(strErrorMessage.c_str()); 
  }
  // Verifica se existe um arquivo de configuracao para cada modelo da simulacao
  vector<ConfigurationFile*> arrConfigurationFiles = this->project->GetConfigurationFiles();
  for (unsigned int j=0; j < arrConfigurationFiles.size(); j++)
  {
    if ( arrConfigurationFiles[j]->GetSimulationConfiguration() == NULL )
    {
      throw new ApplicationException("Projeto nao pode ser simulado pois em um dos arquivos de configuracao a configuracao da simulacao nao foi informada");
    }
    // Verifica se foi criada uma configuracao para cada modelo do mundo virtual
    /////////////////// MUITO COMPLICADO - FAZER DEPOIS
  }

  ////////////////////////////////////////////////////////////
  // Inicia os programas externos ( wxgazebo, player, client
  // control ) para comecar a simulacao
  ////////////////////////////////////////////////////////////

  // Executa o mundo virtual do projeto IRCE
  wxExecute("rm   -r   /tmp/gazebo-hcmarchezi-0/ ",wxEXEC_SYNC);

  string strCommand = "";
  wxString strOutput = "";
  string   output = "";
  //strCommand.append("wxgazebo ");
  strCommand.append(IRCEConfiguration::strGazeboPath.c_str());
  strCommand.append(" ");
  strCommand.append( this->project->GetVirtualWorld()->GetWorldFile() );

  wxProcess * wxgazeboProcess = new wxProcess(wxPROCESS_REDIRECT);
  wxString strGazeboCommand = strCommand.c_str();
  long gazeboProcessPid = wxExecute(strGazeboCommand,wxEXEC_ASYNC,wxgazeboProcess);
  wxInputStream* gazeboInputStream = wxgazeboProcess->GetInputStream();

  while ( (strOutput.Find(wxT("Time")) == -1) && (strOutput.First(wxT("error")) == -1) && (strOutput.Find("server died") > -1) )
  {
    if ( wxgazeboProcess->IsInputAvailable() )
    {
      char out = gazeboInputStream->GetC();
      strOutput += out;
      output += out;
    }
  }
 
  // Se ocorre um erro, uma excecao deve ser gerada
  if (( strOutput.Find("error" ) > -1 ) || ( strOutput.Find("server died") > -1 ) )
  {
    wxKill(gazeboProcessPid);
    wxKill(gazeboProcessPid+1);
    wxShell("rm   -r   /tmp/gazebo-hcmarchezi-0/ ");
    ///////////////delete wxgazeboProcess;
    throw new ApplicationException("Ocorreu um erro ao executar o mundo virtual com wxgazebo. Provavelmente o arquivo esta invalido.");
  }

  ////////////////delete wxgazeboProcess;

  Interval::Wait(3);

  // Instancia o Player para cada arquivo de configuracaio
  // Ex: player -p 7000 < arq config >  

  vector<long> arrPlayerProcessPid;
  for (unsigned int k=0; k < arrConfigurationFiles.size(); k++)
  {
    strCommand = "";
    //strCommand.append("player ");
    strCommand.append(IRCEConfiguration::strPlayerPath); 
    strCommand.append(" -p ");
    strCommand.append( arrConfigurationFiles[k]->GetPortNumber() );
    strCommand.append(" ");
    strCommand.append( arrConfigurationFiles[k]->GetConfigurationFile() );
    //strCommand.append(" &");

    // Executa o processo
    wxProcess * playerProcess = new wxProcess(wxPROCESS_REDIRECT);
    wxString strPlayerCommand = strCommand.c_str();
    long playerProcessPid = wxExecute(strPlayerCommand,wxEXEC_ASYNC,playerProcess);
    wxInputStream* playerInputStream = playerProcess->GetInputStream();
    arrPlayerProcessPid.push_back(playerProcessPid);

    //Espera ate a conclusao da execucao do mesmo
    strOutput = "";
    while ( playerProcess->IsInputAvailable() ) //(strOutput.Find("Listening") == -1) && (strOutput.Find("error") == -1) && (strOutput.Find("Lis") == -1))
    {
      char out = playerInputStream->GetC();
      strOutput += out;
      output += out;
      printf ("%c",out);
    }
    ///////////////////////delete playerProcess;

    //Se houver erro, dispara uma excecao
    if ( strOutput.Find("error") > -1 )
    {
      wxKill(gazeboProcessPid);
      for (int j=0; j < arrPlayerProcessPid.size(); j++)
      {
         wxKill(arrPlayerProcessPid[j]);
      }

      string strErrorMsg = "";
      strErrorMsg += " Ocorreu um erro ao executar o seguinte arquivo de configuracao no Player: ";
      strErrorMsg += arrConfigurationFiles[k]->GetConfigurationFile().c_str();
      throw new ApplicationException(strErrorMsg.c_str());
    }

    Interval::Wait(3);

  }

  // Executa cada programa de controle cliente
  vector<long> arrClientProcessPid;
  for (unsigned int p=0; p < arrPlayerControls.size(); p++)
  {
    strCommand = "";
    strCommand += "./";
    strCommand += arrPlayerControls[p]->GetClientControlFile();
    strCommand += ".bin";

    wxProcess * clientControlProcess = new wxProcess(wxPROCESS_REDIRECT);
    wxString strClientControlCommand = strCommand.c_str();
    long clientProcessPid = wxExecute(strClientControlCommand,wxEXEC_ASYNC,clientControlProcess);
    wxInputStream* clientProcessInputStream = clientControlProcess->GetInputStream();
    ///////////////////delete clientControlProcess;

    arrClientProcessPid.push_back(clientProcessPid);
  }

  wxMessageDialog msgDialog(0,wxT("Pressione o bot�o para interromper a simula��o"),wxT("Controle de Simula��o"), wxOK , wxDefaultPosition);
  msgDialog.ShowModal();

  // Mata todos os processos
  wxKill(gazeboProcessPid);   // wxgazebo
  wxKill(gazeboProcessPid+1); // gazebo
  wxExecute("rm   -r    /tmp/gazebo-hcmarchezi-0/  ",wxEXEC_SYNC);
  for (int index1=0; index1 < arrClientProcessPid.size(); index1++)
  {
    wxKill(arrClientProcessPid[index1]);
  }
  for (int index2=0; index2 < arrPlayerProcessPid.size(); index2++)
  {
    wxKill(arrPlayerProcessPid[index2]);
  }

}

void AppIRCEProject::TransferControl()
{
}