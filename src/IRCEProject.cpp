#ifdef WIN32
#include <windows.h>
#endif

#include "IRCEProject.h"
#include "ApplicationException.h"
#include "SystemException.h" 
///////////////////////////#include "WorldFile.h" 
#include "ModelFactory.hh" 
///////////////////////////#include "XMLNodeFactory.h" 
#include "Conversion.h" 
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/process.h>
#include <wx/regex.h>
#include <wx/msgdlg.h>
#include "FileName.h"

#include <vector>

#include "XMLCommand.h"
#include "Interval.h"
#include "IRCEConfiguration.h"

// Formato de um arquivo de projeto
//<IRCEProject>
//	<worldFile> "diret�rio e nome do arquivo world do mundo virtual" </worldFile>
//	<controls>
//		<control file="diret�rio e nome do arquivo" language=" C" />
//			:	:	:
//	</controls>
//	<configurations>
//		<configuration file="arquivo de configuracao" />
//			:	:	:
//	</configurations>
//</IRCEProject>

using namespace std;

IRCEProject::IRCEProject()
{ 
  //this->xmlDoc = NULL;
  this->isSaved = true;    
  this->virtualWorld = new VirtualWorld();
}

IRCEProject::~IRCEProject(void)
{
}

// Verifica se o arquivo ja existe
bool IRCEProject::ExistFile(string fileName)
{
  return wxFileExists(wxString( fileName.c_str() ) );
}

string IRCEProject::GetProjectFile()
{
  return this->projectFile;
}

void IRCEProject::SetProjectFullPath(string strProjectFullPath)
{
  if (this->ExistFile(projectFile))
  {
    throw new ApplicationException(" Arquivo de projeto informado ja existe. Escolha outro caminho. ");
  }
  // Cria um arquivo
  wxTextFile file( wxString( strProjectFullPath.c_str() ) );
  //file.Create();
  //file.Close();
  
  // Extrai o arquivo, diretorio e extens�o
  wxString strFullPath = wxString( strProjectFullPath.c_str() );
  wxString strExtension;  
  wxString strProjectFile;
  wxString strProjectDirectory;
  wxFileName::SplitPath(strFullPath,&strProjectDirectory,&strProjectFile,&strExtension);

  // Atribui o diretorio e o nome do arquivo de projeto
  this->projectDirectory = string( strProjectDirectory.c_str() );
  this->projectFile = string( (strProjectFile + wxT(".") + strExtension).c_str() );
  
}

string IRCEProject::GetProjectDirectory()
{
  return this->projectDirectory;
}

void IRCEProject::SaveProjecFile()
{
  // Atribui como novo diretorio corrente para o 
  // diretorio do arquivo de projeto
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@@@ TESTAR SE ISSO FUNCIONA !!!
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  wxSetWorkingDirectory( this->GetProjectDirectory().c_str() );

  ////////////////////////////////////////////////////  
  // Salva os arquivos que comp�em o projeto utilizando
  // path relativo ao do working directory acima
  ////////////////////////////////////////////////////

  // Salva arquivo do munsdo virtual (*.world)
  this->GetVirtualWorld()->SaveWorldFile();

  // Salva os arquivos de programa de controle
  for (unsigned int i = 0; i < this->GetPlayerControlPrograms().size(); i++)
  {
    this->GetPlayerControlPrograms()[i]->SaveControlProgram();
  }

  // Salva os arquivos de configuracao
  for (unsigned int i = 0; i < this->GetConfigurationFiles().size(); i++)
  {
    this->GetConfigurationFiles()[i]->SaveConfiguration();
  }

  ///////////////////////////////////////////////////
  // L�gica para compor o arquivo XML de projeto
  ///////////////////////////////////////////////////

  // Cria o documento XML do arquivo de projeto
  TiXmlDocument doc( "IRCEProject" );
  TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

  // Cria tag do arquivo do mundo virtual no arquivo de projeto
  TiXmlElement * worldFileElement = new TiXmlElement( "worldFile" );	
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //  TESTAR ISSO !!!
  //  Salva o arquivo world relativo ao arquivo de projeto
  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  TiXmlText * worldFileText = new TiXmlText( this->GetVirtualWorld()->GetWorldFile().c_str() );
  //wxString strWorldFilePath = this->GetVirtualWorld()->GetWorldFile().c_str();
  //wxString strRelativeWorldFilePath = wxRealPath( strWorldFilePath );
  //TiXmlText * worldFileText = new TiXmlText( strRelativeWorldFilePath.c_str() );
  	worldFileElement->LinkEndChild( worldFileText );
  doc.LinkEndChild( worldFileElement );

  // Tag dos controles
  TiXmlElement * controlsElement = new TiXmlElement( "controls" );

  for (unsigned int i = 0; i < this->arrPlayerControlPrograms.size(); i++)
  {
    // Cria a tag do arquivo de controle no arquivo de projeto
    TiXmlElement * controlElement = new TiXmlElement("control");
    controlElement->SetAttribute("file", this->arrPlayerControlPrograms[i]->GetClientControlFile().c_str() );
    controlElement->SetAttribute("language", this->arrPlayerControlPrograms[i]->GetProgrammingLanguage().c_str() );
    controlsElement->LinkEndChild( controlElement );
  }

	doc.LinkEndChild( controlsElement );	

  // Tag das configuracoes
  TiXmlElement * configurationsElement = new TiXmlElement( "configurations" );

  for (unsigned int i = 0; i < this->arrConfigurationFiles.size(); i++)
  {
    // Cria a tag para cada arquivo de configuracao
    TiXmlElement * configurationElement = new TiXmlElement("configuration");
    configurationElement->SetAttribute("file",this->arrConfigurationFiles[i]->GetConfigurationFile().c_str() );
    configurationElement->SetAttribute("port",this->arrConfigurationFiles[i]->GetPortNumber().c_str() );
    configurationsElement->LinkEndChild( configurationElement );
  }
  
	doc.LinkEndChild( configurationsElement );

  // Salva o arquivo XML resultante
  string strProjectFullPath = this->projectDirectory + "/" + this->projectFile;
  doc.SaveFile( strProjectFullPath.c_str() );
}

IRCEProject * IRCEProject::LoadProject(string projectFullPath)
{
  // Cria projeto e passa o nome do arquivo
  IRCEProject * project = new IRCEProject();
  project->SetProjectFullPath( projectFullPath );

  ////////////////////////////////////////////////////////////
  // Leitura do arquivo XML do projeto
  ////////////////////////////////////////////////////////////  
  
  // Carrega o arquivo XML do projeto
  TiXmlDocument doc;
  doc.LoadFile( projectFullPath.c_str() );

  ////////////////////////////////////////////////////////////
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //@@@@@ Seta o diretorio padr�o para o diretorio do projeto
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  wxSetWorkingDirectory( wxString( project->GetProjectDirectory().c_str() ) );

  // Carrega o nome do arquivo world file
  TiXmlElement * worldFileElement = doc.FirstChildElement("worldFile");
  if ( worldFileElement == NULL )
  {
    throw new SystemException("O arquivo de projeto eh invalido pois nao possui a tag <worldFile>.","IRCEProject::LoadProject");
  }
  const char * pStrWorldFile = worldFileElement->GetText();

  vector<string> arrStrControlProgramFile;
  vector<string> arrStrControlProgramLanguage;

  vector<string> arrStrConfigurationFile;
  vector<string> arrStrConfigurationPort;

  // Carrega os nomes dos arquivos dos programas de controle
  TiXmlElement * controlsElement = doc.FirstChildElement("controls");
  if ( controlsElement == NULL )
  {
    throw new SystemException("O arquivo de projeto eh invalido pois nao possui a tag <controls>.","IRCEProject::LoadProject");
  }

  TiXmlElement * controlElement = controlsElement->FirstChildElement("control");
  while (controlElement != NULL)
  {
    TiXmlAttribute * fileAttribute = controlElement->FirstAttribute();
    TiXmlAttribute * languageAttribute = fileAttribute->Next();
    const char * pStrControlProgramFile = fileAttribute->Value();
    const char * pStrPogrammingLanguage = languageAttribute->Value();

    if ((pStrControlProgramFile == NULL) || (pStrPogrammingLanguage == NULL))
    {
      throw new SystemException("Erro no conteudo do arquivo XML de projeto: Arquivo do programa de controle e a linguagem de programa��o devem ser informados","IRCEProject::LoadProject");
    }

    arrStrControlProgramFile.push_back( string( pStrControlProgramFile ) );
    arrStrControlProgramLanguage.push_back( string( pStrPogrammingLanguage ) );

    controlElement = controlElement->NextSiblingElement();
  }

  // Le a tag de configuracoes <configurations>
  TiXmlElement * configurationsElement = doc.FirstChildElement("configurations");
  if ( configurationsElement == NULL )
  {
    throw new SystemException("O arquivo de projeto eh invalido pois nao possui a tag <configurations>.","IRCEProject::LoadProject");
  }

  TiXmlElement * configurationElement = configurationsElement->FirstChildElement("configuration");
  while (configurationElement != NULL)
  {
    //TiXmlAttribute * fileAttribute = configurationElement->FirstAttribute();
    //const char * pStrConfigurationFile = fileAttribute->Value();

    const char * pStrConfigurationFile = configurationElement->Attribute("file");
    const char * pStrPortNumber        = configurationElement->Attribute("port");

    if ((pStrConfigurationFile == NULL)||(pStrPortNumber == NULL))
    {
      throw new SystemException("Erro no conteudo do arquivo XML de projeto: Arquivo de configuracao deve ser informado","IRCEProject::LoadProject");
    }

    arrStrConfigurationFile.push_back( string( pStrConfigurationFile ) );
    arrStrConfigurationPort.push_back( string( pStrPortNumber ) );
   
    configurationElement = configurationElement->NextSiblingElement();
  }

 
  /////////////////////////////////////////////////////
  // Leitura do Arquivo do mundo virtual
  /////////////////////////////////////////////////////
  if ( pStrWorldFile != NULL )
  {
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@ TESTAR: Transforma path relativo em absoluto
    //@@@@ usando o path do projeto como referencia    
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    // o diretorio ja esta setado como o diretorio do projeto
    project->SetVirtualWorld( VirtualWorld::LoadWorldFile( pStrWorldFile ) );
  }

  /////////////////////////////////////////////////////
  // Leitura dos arquivos de programa
  /////////////////////////////////////////////////////
  for (unsigned int i=0; i < arrStrControlProgramFile.size(); i++)
  {
    project->AddPlayerControlProgram( PlayerControl::LoadControlProgram( arrStrControlProgramFile[i], arrStrControlProgramLanguage[i] ) );
  }

  /////////////////////////////////////////////////////
  // Leitura dos arquivos de configuracao
  /////////////////////////////////////////////////////
  for (unsigned int i=0; i < arrStrConfigurationFile.size(); i++)
  {
    project->AddConfigurationFile( ConfigurationFile::LoadConfiguration( arrStrConfigurationFile[i], arrStrConfigurationPort[i] , project->GetVirtualWorld() ) );
  }

  return project;
}


bool IRCEProject::IsSaved()
{
  return this->isSaved;
}

void IRCEProject::AddConfigurationFile( ConfigurationFile * configurationFile )
{
  // Verifica se ja existe uma configuracao no proejto para o dispositivo informado
  vector<ConfigurationFile*>::iterator it = this->arrConfigurationFiles.begin();
  while ( it != this->arrConfigurationFiles.end() )
  {
    if ( (*it)->GetConfigurationFile() == configurationFile->GetConfigurationFile() )
    {
      throw new ApplicationException(" O arquivo de configuracao ja existe no projeto ");
    }
    it++;
  }
  // Se nao existe, a configuracao � criada e adicionada no projeto
  this->arrConfigurationFiles.push_back(configurationFile);
}
void IRCEProject::RemoveConfigurationFile( ConfigurationFile * configurationFile )
{
  // Se achar a configuracao para o dispositivo informado , deleta no projeto
  vector<ConfigurationFile*>::iterator it = this->arrConfigurationFiles.begin();
  while ( it != this->arrConfigurationFiles.end() )
  {
    if ( (*it)->GetConfigurationFile() == configurationFile->GetConfigurationFile() )
    {
      this->arrConfigurationFiles.erase(it);
      return;
    }
    it++;
  }
  // Se nao achou a configura��o , mostra uma msg de erro
  throw new ApplicationException(" Nao existe configura��o para o modelo 3D informado ");
}
vector<ConfigurationFile*> IRCEProject::GetConfigurationFiles()
{
  return this->arrConfigurationFiles;
}

ConfigurationFile * IRCEProject::GetConfigurationFile(string strPath)
{
  string strLocalPath = "";
  strLocalPath = FileName::GetRelativeFilename( this->GetProjectDirectory(), strPath );
  
  for (unsigned int i = 0; i < this->arrConfigurationFiles.size(); i++)
  {
    if ( this->arrConfigurationFiles[i]->GetConfigurationFile() == strLocalPath )
    {
      return this->arrConfigurationFiles[i];
    }
  }
  throw new SystemException("IRCEProject::GetConfigurationFile","Nao existe arquivo de configuracao com o caminho informado. ");
}

void IRCEProject::AddPlayerControlProgram(PlayerControl * playerControl)
{
  // Verifica se ja existe um programa de controle com o mesmo nome de arquivo
  vector<PlayerControl*>::iterator it = this->arrPlayerControlPrograms.begin();
  while (it != this->arrPlayerControlPrograms.end())
  {
    if ((*it)->GetClientControlFile() == playerControl->GetClientControlFile())
    {
      throw new ApplicationException("O programa de controle ja existe no projeto");
    }
    it++;
  }
  // Adiciona o programa de controle no projeto  
  this->arrPlayerControlPrograms.push_back(playerControl);
  // Se o arquivo nao existe, cria um vazio no caminho informado
  if (! wxFileExists( wxString( playerControl->GetClientControlFile().c_str() ) ) )
  {
    wxTextFile file(wxString(playerControl->GetClientControlFile().c_str()));
    file.Create();
  }
}

void IRCEProject::RemovePlayerControlProgram( PlayerControl * playerControl )
{
   // Verifica se ja existe um programa de controle com o caminho ( path ) informado
  vector<PlayerControl*>::iterator it = this->arrPlayerControlPrograms.begin();
  while (it != this->arrPlayerControlPrograms.end())
  {
    if ((*it)->GetClientControlFile() == playerControl->GetClientControlFile() )
    {
      // Remove o arquivo do programa de controle
      wxRemoveFile( wxString( playerControl->GetClientControlFile().c_str() ) );
       
      // Remove o programa de controle do projeto
      this->arrPlayerControlPrograms.erase(it);
    }
    it++;
  }
  // Mostra o erro se nao achou o programa de controle
  throw new ApplicationException("O programa de controle informado n�o pode ser removido pois n�o existe no projeto");
}

vector<PlayerControl*> IRCEProject::GetPlayerControlPrograms()
{
  return this->arrPlayerControlPrograms;
}

void IRCEProject::CompilePlayerControlProgram( PlayerControl * playerControl, vector<string> & arrStrOutput,vector<string> & arrStrErrors)
{
  wxArrayString output;
  wxArrayString errors;

  string strCommand = "";
  strCommand += IRCEConfiguration::strCPPCompilerPath;
  strCommand += " -I";
  strCommand += IRCEConfiguration::strPlayerHeaderPath;
  strCommand += " -L";
  strCommand += IRCEConfiguration::strPlayerLibPath;
  strCommand += " ";
  strCommand += IRCEConfiguration::strPlayerLibs;
  strCommand += " ";

  //strCommand += "g++ -I/usr/local/include -I/usr/local/include/player-2.0 -L/usr/local/lib -lplayerc++ ";

  strCommand += wxString( playerControl->GetClientControlFile().c_str() );
  strCommand += " -o ";
  strCommand += wxString( playerControl->GetClientControlFile().c_str() ) ;
  strCommand += ".bin";

  wxExecute(wxString( strCommand.c_str() ),output,errors);

  // Copia o array do wxWidgets para o ver
  arrStrOutput.push_back( strCommand + string("\n\n") );
  for (unsigned int i=0; i < output.Count(); i++)
  {
    arrStrOutput.push_back( string( output[i].c_str()  ) + string("\n") );
  }

  // Copia o array do wxWidgets para o vector 
  for (unsigned int j=0; j < errors.Count(); j++)
  {
    arrStrErrors.push_back( string( errors[j].c_str() ) + string("\n") );
  }

  int errorsCount = arrStrErrors.size();
  if ( errorsCount == 0 )
  {
    arrStrOutput.push_back( string("Compiled successfully") );
  }

}

PlayerControl * IRCEProject::GetPlayerControlProgram( string strPath )
{ 
  strPath = FileName::GetRelativeFilename( this->GetProjectDirectory(), strPath );

  for (unsigned int i = 0; i < this->arrPlayerControlPrograms.size(); i++)
  {
    if ( this->arrPlayerControlPrograms[i]->GetClientControlFile() == strPath )
    {
      return this->arrPlayerControlPrograms[i];
    }
  }
  return NULL;
}

VirtualWorld * IRCEProject::GetVirtualWorld()
{
  return this->virtualWorld;
}

void IRCEProject::SetVirtualWorld(VirtualWorld * virtualWorld)
{
  this->virtualWorld = virtualWorld;
}
