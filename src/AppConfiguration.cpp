
#include "AppConfiguration.h"

#include "SystemException.h"
#include "ApplicationException.h"
#include <wx/wx.h>
#include <wx/textdlg.h>
#include "FileName.h"
#include "SimulationConfiguration.h"

AppConfiguration::AppConfiguration(void)
{
  project = NULL;
}

AppConfiguration::~AppConfiguration(void)
{
}

void AppConfiguration::SetSimulationConfiguration(string strName,string strDevice,string strPlugIn,string strServerId,ConfigurationFile * configurationFile)
{
  // Se arquivo de configuracao nao tem configuracao de simulacao, cria uma
  // e adiciona no arquivo de configuracao
  if ( configurationFile->GetSimulationConfiguration() == NULL )
  {
    configurationFile->SetSimulationConfiguration( new SimulationConfiguration() );
  }
  // Passa os dados para a configuracao da simulacao
  SimulationConfiguration * simulationConfiguration = configurationFile->GetSimulationConfiguration();
  simulationConfiguration->SetName(strName);
  simulationConfiguration->SetDevice(strDevice);
  simulationConfiguration->SetPlugIn(strPlugIn);
  simulationConfiguration->SetServerID(strServerId);
}

void AppConfiguration::AddPlayerConfigurationFile(string strName,string strDevice,Model * model,ConfigurationFile * configurationFile)
{
  configurationFile->AddPlayerConfiguration(new PlayerConfiguration(strName,strDevice,model));
}
void AppConfiguration::UpdatePlayerConfigurationFile(string strName,string strDevice,Model * model,PlayerConfiguration * playerConfiguration,ConfigurationFile * configurationFile)
{
  // Verifica se ja existe uma outra configuracao para o modelo 3D informado
  for (unsigned int i = 0; i < configurationFile->GetPlayerConfigurations().size(); i++)
  {
    PlayerConfiguration * playerConfigurationArr = configurationFile->GetPlayerConfigurations()[i];

    if ((playerConfiguration != playerConfigurationArr)&&(playerConfigurationArr->GetModel() == model))
    {
      throw new ApplicationException("A configuracao ja existe para o modelo 3D informado no projeto");
    }
  }
  // Se nao existe atualiza as informacoes da configuracao
  playerConfiguration->SetDriverName(strName);
  playerConfiguration->SetDeviceName(strDevice);
  playerConfiguration->SetModel(model);
}
void AppConfiguration::RemovePlayerConfigurationFile(PlayerConfiguration * playerConfiguration,ConfigurationFile * configurationFile)
{
  configurationFile->RemovePlayerConfiguration(playerConfiguration);  
}

void AppConfiguration::AddConfigurationFile(string strPath)
{ 
  // O path informado passa a ser relativo ao diretorio do projeto 
  string strRelativePath = FileName::GetRelativeFilename( this->project->GetProjectDirectory(), strPath );
  // Adiciona o arquivo de configuracao no projeto
  this->project->AddConfigurationFile(new ConfigurationFile(strRelativePath));
}
void AppConfiguration::RemoveConfigurationFile(ConfigurationFile * configurationFile)
{
  this->project->RemoveConfigurationFile(configurationFile);
}
void AppConfiguration::LoadConfigurationFile(string strPath)
{
  // Verifica se ja existe um arquivo de configuracao com o caminho
  // informado no projeto   
  // Verifica se o arquivo de configuracao informado ja existe no projeto
  unsigned int i = 0;
  for(i = 0; i < this->project->GetConfigurationFiles().size(); i++)
  {
    if ( this->project->GetConfigurationFiles()[i]->GetConfigurationFile() == strPath )
    {
      break;
    }
  }
  // Se ja existe mostra erro
  if ( i < this->project->GetConfigurationFiles().size() )
  {
    throw new ApplicationException("Ja existe um arquivo de configuracao com esse arquivo no projeto");
  }

  // Dialogo Pede que o usuario informe o numero da porta
  wxTextEntryDialog * portNumberDialog = new wxTextEntryDialog(NULL,wxT("Port Number:"),wxT("Informe a porta"));
  portNumberDialog->ShowModal();
  string strPortNumber = string( portNumberDialog->GetValue().c_str() );

  // Atribui o diretorio corrente para o diretorio do projeto
  //wxSetWorkingDirectory(wxString(this->project->GetProjectDirectory().c_str()));
  // Transforma o path asoluto em path relativo
  strPath = FileName::GetRelativeFilename( this->project->GetProjectDirectory(), strPath );
  // Se nao existe o arquivo de configiracao e carregado e adicionado ao projeto
  this->project->AddConfigurationFile( ConfigurationFile::LoadConfiguration(strPath, strPortNumber, this->project->GetVirtualWorld()) );
}
void AppConfiguration::SaveConfigurationFile(string strPortNumber,ConfigurationFile * configurationFile)
{
  // Verifica se ja existe outro arquivo de configuracao
  // com o mesmo numero de porta
  vector<ConfigurationFile *> arrConfigurationFiles = this->project->GetConfigurationFiles();
  for (unsigned int i=0; i < arrConfigurationFiles.size(); i++)
  {
    if (( configurationFile->GetConfigurationFile() != arrConfigurationFiles[i]->GetConfigurationFile() ) &&
        ( strPortNumber == arrConfigurationFiles[i]->GetPortNumber()))
    {
      throw new ApplicationException("Ja existe outro arquivo com a mesma port (Port Number). Informe outro numero.");
    }
  }

  // Passa o numero da porta para o arquivo de configuracao
  configurationFile->SetPortNumber(strPortNumber);
  // Atribui o diretorio corrente para o diretorio do projeto
  //wxSetWorkingDirectory(wxString(this->project->GetProjectDirectory().c_str()));
  // Salva o arquivo de configuracao
  configurationFile->SaveConfiguration();
}
