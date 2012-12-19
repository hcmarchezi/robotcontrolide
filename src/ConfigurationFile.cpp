
#include "ConfigurationFile.h"

#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/process.h>
#include <wx/regex.h>
#include "ApplicationException.h"
#include "SystemException.h"

ConfigurationFile::ConfigurationFile(string strConfigurationFile)
{
  this->configurationFile = strConfigurationFile;
  this->simulationConfiguration = NULL;
}

ConfigurationFile::~ConfigurationFile(void)
{
}

void ConfigurationFile::SetConfigurationFile(string strConfigurationFile)
{
  this->configurationFile = strConfigurationFile;
}
string ConfigurationFile::GetConfigurationFile()
{
  return this->configurationFile;
}

string ConfigurationFile::GetPortNumber()
{
  return this->portNumber;
}
void ConfigurationFile::SetPortNumber(string portNumber)
{
  this->portNumber = portNumber;
}

void ConfigurationFile::SetSimulationConfiguration(SimulationConfiguration * simulationConfiguration)
{
  this->simulationConfiguration = simulationConfiguration;
}
SimulationConfiguration * ConfigurationFile::GetSimulationConfiguration()
{
  return this->simulationConfiguration;
}

void ConfigurationFile::AddPlayerConfiguration( PlayerConfiguration * playerConfiguration )
{
  // Verifica se ja existe a mesma configuracao no projeto
  vector<PlayerConfiguration*>::iterator it = this->arrPlayerConfigurations.begin();
  while ( it != this->arrPlayerConfigurations.end() )
  {
    if (( (*it)->GetModel() == playerConfiguration->GetModel() )&&
         ( (*it)->GetDeviceName() == playerConfiguration->GetDeviceName() )&&
         ( (*it)->GetDriverName() == playerConfiguration->GetDriverName() )) 
    {
      throw new ApplicationException(" A configuracao informada ja existe no projeto ");
    }
    it++;
  }
  // Se nao existe, a configuracao � criada e adicionada no projeto
  this->arrPlayerConfigurations.push_back(playerConfiguration);
}
void ConfigurationFile::RemovePlayerConfiguration( PlayerConfiguration * playerConfiguration )
{
  // Se achar a configuracao para o dispositivo informado , deleta no projeto
  vector<PlayerConfiguration*>::iterator it = this->arrPlayerConfigurations.begin();
  while ( it != this->arrPlayerConfigurations.end() )
  {
    if ( (*it)->GetModel() == playerConfiguration->GetModel() )
    {
      this->arrPlayerConfigurations.erase(it);
      return;
    }
    it++;
  }
  // Se nao achou a configura��o , mostra uma msg de erro
  throw new ApplicationException(" Nao existe configuracao para o modelo 3D informado ");
}
vector<PlayerConfiguration *> ConfigurationFile::GetPlayerConfigurations()
{
  return this->arrPlayerConfigurations;
}

// Obtem uma das configuracoes de modelo no arquivo de configuracao pela
// identificacao do modelo que este afeta
PlayerConfiguration * ConfigurationFile::GetPlayerConfiguration(string strDriverName,string strDeviceName,string strModelId)
{
  // Verifica se ja existe uma configuracao no proejto para o dispositivo informado
  vector<PlayerConfiguration*>::iterator it = this->arrPlayerConfigurations.begin();
  while ( it != this->arrPlayerConfigurations.end() )
  {
    if ( ((*it)->GetModel()->GetId() == strModelId )&&
         ((*it)->GetDeviceName() == strDeviceName)&&
         ((*it)->GetDriverName() == strDriverName) )
    {
      return (*it);
    }
    it++;
  }
  // Se nao achou a configura��o , mostra uma msg de erro
  throw new SystemException("ConfigurationFile::GetPlayerConfiguration","Nao existe configura��o para o modelo 3D informado");
}

// Metodos para carregar e salvar um arquivo de configuracao
ConfigurationFile * ConfigurationFile::LoadConfiguration(string strConfigurationFile, string strPortNumber, VirtualWorld * virtualWorld )
{
  // Verifica se o arquivo existe
  if (!wxFileExists( wxString( strConfigurationFile.c_str() ) ) )
  {
    throw new ApplicationException("O arquivo informado n�o foi encontrado");
  }

  // Abre o arquivo como arquivo texto
  wxTextFile file( wxString( strConfigurationFile.c_str() ) );
  if (!file.Open()) 
  {
    throw new SystemException("ConfigurationFile::LoadConfiguration","Ocorreu um problema ao abrir o arquivo de configura��o");
  }
  
  // Le todo o conteudo do arquivo 
  wxString strFileContent;
  for (unsigned int i=0; i < file.GetLineCount(); i++)
  {
    strFileContent += file.GetLine(i);
  }

  // Fecha o arquivo
  file.Close();

  // Cria a expressao regular de uma configuracao para um modelo
  wxRegEx regexDriver;
  wxString strDriverPattern = wxT("driver[:blank:]*\\([^\\)]+\\)");    

  if (! regexDriver.Compile(strDriverPattern) ) 
  {
    throw new SystemException("ConfigurationFile::LoadConfiguration","Erro na compilacao da expressao regular");
  }

  // Expressao regular para extrair o nome de um driver
  wxRegEx regexName;
  wxString strNamePattern = wxT("name[ \\t]+\"([[:alnum:]\\:_]+)\"");
  if (! regexName.Compile(strNamePattern) )
  {
    throw new SystemException("ConfigurationFile::LoadConfiguration","Erro na compilacao da expressao regular");
  }

  // Expressao Regular para extrair o dispositivo ( device ) do driver
  wxRegEx regexDevice;
  wxString strDevicePattern = wxT("provides[ \\t]+\\[\"([[:alnum:]\\:_]+)\"\\]");
  if (! regexDevice.Compile(strDevicePattern) )
  {
    throw new SystemException("ConfigurationFile::LoadConfiguration","Erro na compilacao da expressao regular");
  }

  // Expressao Regular para extrair a identificacao do modelo
  wxRegEx regexModelID;
  wxString strModelIDPattern = wxT("gz_id[ \\t]+\"([[:alnum:]\\:_]+)\"");
  if (! regexModelID.Compile(strModelIDPattern) )
  {
    throw new SystemException("ConfigurationFile::LoadConfiguration","Erro na compilacao da expressao regular");
  }

   // Expressao Regular para extrair o nome do plugin
  wxRegEx regexPlugIn;
  wxString strPlugInPattern = wxT("plugin[ \\t]+\"([[:alnum:]\\:_]+)\"");
  if (! regexPlugIn.Compile(strPlugInPattern) )
  {
    throw new SystemException("ConfigurationFile::LoadConfiguration","Erro na compilacao da expressao regular");
  }

  // Expressao Regular para extrair a identificacao do servidor
  wxRegEx regexServerID;
  wxString strServerIDPattern = wxT("server_id[ \\t]+\"([[:alnum:]\\:_]+)\"");
  if (! regexServerID.Compile(strServerIDPattern) )
  {
    throw new SystemException("ConfigurationFile::LoadConfiguration","Erro na compilacao da expressao regular");
  }


  // Instancia um objeto de configuracao com o arquivo
  ConfigurationFile * configurationFile = new ConfigurationFile( strConfigurationFile );

  while ( regexDriver.Matches(strFileContent) )
  {
    // Extrai um driver no arquivo
    wxString strDriver = regexDriver.GetMatch(strFileContent);

    // Verifica se o driver e de um modelo 3D
    bool bNameMatch   = regexName.Matches(strDriver);
    bool bDeviceMatch = regexDevice.Matches(strDriver);
    bool bModelID     = regexModelID.Matches(strDriver);
    bool bServerID    = regexServerID.Matches(strDriver);
    bool bPlugIn      = regexPlugIn.Matches(strDriver);

    // Se o driver possui ( name, device, gz_model ) entao extrai os valores
    // para uma instancia de player configuration
    if ((bNameMatch)&&(bDeviceMatch)&&(bModelID))
    {
      wxString strName = regexName.GetMatch(strDriver);  
      strName = strName.substr( strName.find_first_of("\"")+1, strName.find_last_of("\"")-(strName.find_first_of("\"")+1) );

      wxString strDevice = regexDevice.GetMatch(strDriver);
      strDevice = strDevice.substr( strDevice.find_first_of("\"")+1, strDevice.find_last_of("\"")-(strDevice.find_first_of("\"")+1) );

      wxString strModelID = regexModelID.GetMatch(strDriver);
      strModelID = strModelID.substr( strModelID.find_first_of("\"")+1, strModelID.find_last_of("\"")-(strModelID.find_first_of("\"")+1) );
      Model * model = virtualWorld->GetModel( strModelID.c_str() );
      if ( model == NULL )
      {
        wxString strErrorMessage = wxT("O modelo ") + strModelID + wxT(" informado na configuracao, n�o existe no mundo virtual do projeto");
        throw new ApplicationException(strErrorMessage.c_str());
      }
      PlayerConfiguration * playerConfiguration = new PlayerConfiguration(strName.c_str(),strDevice.c_str(),model);
      configurationFile->AddPlayerConfiguration(playerConfiguration);
    }
    // Se o driver possui ( name, device, server_id ) entao extrai os valores
    // para uma instancia de Simulation Configuration
    else if ((bNameMatch)&&(bDeviceMatch)&&(bServerID))
    {
      if ( configurationFile->GetSimulationConfiguration() != NULL )
      {
        throw new ApplicationException("Nao pode haver mais de uma configuracao de simulacao no arquivo de configuracao");
      }

      wxString strName = regexName.GetMatch(strDriver);  
      strName = strName.substr( strName.find_first_of("\"")+1, strName.find_last_of("\"")-(strName.find_first_of("\"")+1) );

      wxString strDevice = regexDevice.GetMatch(strDriver);
      strDevice = strDevice.substr( strDevice.find_first_of("\"")+1, strDevice.find_last_of("\"")-(strDevice.find_first_of("\"")+1) );

      wxString strPlugIn = regexPlugIn.GetMatch(strDriver);
      strPlugIn = strPlugIn.substr( strPlugIn.find_first_of("\"")+1, strPlugIn.find_last_of("\"")-(strPlugIn.find_first_of("\"")+1) );

      wxString strServerID = regexServerID.GetMatch(strDriver);
      strServerID = strServerID.substr( strServerID.find_first_of("\"")+1, strServerID.find_last_of("\"")-(strServerID.find_first_of("\"")+1) );

      SimulationConfiguration * simulationConfiguration = new SimulationConfiguration();
      simulationConfiguration->SetName( strName.c_str() );
      simulationConfiguration->SetDevice( strDevice.c_str() );
      simulationConfiguration->SetPlugIn( strPlugIn.c_str() );
      simulationConfiguration->SetServerID( strServerID.c_str() );
      configurationFile->SetSimulationConfiguration( simulationConfiguration );
    }
   
    // Remove a string achada da string do arquivo
    regexDriver.ReplaceFirst(&strFileContent,wxT(""));
  }

  // Atrbui o numero da porta ao arquivo de configuracao
  // que foi passado como parametro
  configurationFile->SetPortNumber(strPortNumber);

  return configurationFile;
}
void ConfigurationFile::SaveConfiguration()
{
    // Abre o arquivo como arquivo texto
  wxTextFile file( wxString( this->configurationFile.c_str() ) );
  file.Create();

  // Remove as linhas do arquivo de controle  
  for (int i=0; i < file.GetLineCount(); i++)
  {
    file.RemoveLine(i);
  }

  // Adiciona este driver PADRAO no arquiovo de configuracao
  file.AddLine("driver");
  file.AddLine("(");
  file.AddLine("  name \"" + wxString( this->GetSimulationConfiguration()->GetName().c_str() ) + "\"");
  file.AddLine("  provides [\"" + wxString( this->GetSimulationConfiguration()->GetDevice().c_str() ) + "\"]");
  file.AddLine("  plugin \"" + wxString( this->GetSimulationConfiguration()->GetPlugIn().c_str() ) + "\"");
  file.AddLine("  server_id \""+ wxString( this->GetSimulationConfiguration()->GetServerID().c_str() ) + "\"");
  file.AddLine(")");
  file.AddLine("");

  // Adiciona as configuracaoes no arquivo de configuracao
  for(unsigned int i = 0; i < this->GetPlayerConfigurations().size(); i++)
  {
    PlayerConfiguration * playerConfiguration = this->GetPlayerConfigurations()[i];
    file.AddLine("driver");
    file.AddLine("(");
    file.AddLine("  name \"" + wxString( playerConfiguration->GetDriverName().c_str() ) + "\"");
    file.AddLine("  provides [\"" + wxString( playerConfiguration->GetDeviceName().c_str() ) + "\"]");
    file.AddLine("  gz_id \"" + wxString( playerConfiguration->GetModel()->GetId() ) + "\"");
    file.AddLine(")");
    file.AddLine("");
  }

  if (! file.Write()) 
  {
    throw new SystemException("ConfigurationFile::SaveConfiguration","Ocorreu um erro ao salvar o arquivo de configuracao");
  }

  file.Close();
}