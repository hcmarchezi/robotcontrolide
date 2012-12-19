
#ifndef IRCEPROJECT_H
#define IRCEPROJECT_H

#include <string>
#include <vector>
#include "PlayerConfiguration.h" 
#include "PlayerControl.h"
#include "Model.hh"
//#include <libxml/xmlreader.h> 
#include "VirtualWorld.h"
#include "ConfigurationFile.h"
#include <wx/wx.h>

using namespace std;

// Representa um projeto IRCE e possui:
// - Uma colecao de modelos 3D que descrevem o mundo virtual
// - Um arquivo de configuracao para o simulador gazebo e o servidor de controle Player
// - Uma colecao de classes que representam o programa de controle cliente para cada dispositivo e/ou robo
class IRCEProject
{
public: IRCEProject();
public: virtual ~IRCEProject(void);

// Metodo auxiliar - Verifica se um arquivo existe
private: bool ExistFile(string fileName);

// nome do arquivo de projeto IRCE
private: string projectFile;
private: string projectDirectory;
public:  string GetProjectFile();
public:  void   SetProjectFullPath(string strProjectFullPath);
public:  string GetProjectDirectory();

// Salva / Carrega arquivo de projeto IRCE
public: void SaveProjecFile();
public: static IRCEProject * LoadProject(string projectFullPath);

// Marca se o arquivo de projeto esta salvo ou nao
private: bool isSaved;
public:  bool IsSaved();

// ADICIONAR ESTES METODOS para registrar arquivos de configuracao (IMPLEMENTAR )
private: vector<ConfigurationFile*> arrConfigurationFiles;
public:  void AddConfigurationFile( ConfigurationFile * configurationFile );
public:  void RemoveConfigurationFile( ConfigurationFile * configurationFile );
public:  vector<ConfigurationFile*> GetConfigurationFiles();
public:  ConfigurationFile * GetConfigurationFile(string strPath);

// Lista de programas de controle
private: vector<PlayerControl*> arrPlayerControlPrograms;
public:  void AddPlayerControlProgram( PlayerControl * playerControl );
public:  void RemovePlayerControlProgram( PlayerControl * playerControl );
public:  vector<PlayerControl*> GetPlayerControlPrograms(); 
public:  void CompilePlayerControlProgram( PlayerControl * playerControl,vector<string> & arrStrOutput,vector<string> & arrStrErrors);
public:  PlayerControl * GetPlayerControlProgram( string strProgramFileName );

private: VirtualWorld * virtualWorld;
public:  VirtualWorld * GetVirtualWorld();
public:  void SetVirtualWorld(VirtualWorld * virtualWorld);

};

#endif

