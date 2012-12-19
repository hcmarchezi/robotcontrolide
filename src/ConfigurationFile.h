
#ifndef CONFIGURATIONFILE_H
#define CONFIGURATIONFILE_H

#include<string>
#include<vector>
#include "PlayerConfiguration.h"
#include "SimulationConfiguration.h"
#include "VirtualWorld.h"

using namespace std;

// Representa um arquivo de configuracao do projeto IRCE
// Cada arquivo de configuracao possui uma configuracao para um servidor e
// varias configuracaoes para os modelos
// Cada configuracao eh uma instancia da classe PlayerConfiguration
class ConfigurationFile
{
public: ConfigurationFile(string strConfigurationFile);
public: virtual ~ConfigurationFile(void);

private: string configurationFile;
public:  void SetConfigurationFile(string strConfigurationFile);
public:  string GetConfigurationFile();

// E a porta que serea usada pelo Player ao ler o arquivo de configuracao
private: string portNumber;
public:  string GetPortNumber();
public:  void SetPortNumber(string portNumber);

// Dados da configuracao do servidor ( só existe uma configuracao dessa por arquivo )
private: SimulationConfiguration * simulationConfiguration;
public: void SetSimulationConfiguration(SimulationConfiguration * simulationConfiguration);
public: SimulationConfiguration * GetSimulationConfiguration();

// Metodos para adicionar e remover configuracoes do Player
private: vector<PlayerConfiguration*> arrPlayerConfigurations;
public:  void AddPlayerConfiguration( PlayerConfiguration * playerConfiguration );
public:  void RemovePlayerConfiguration( PlayerConfiguration * playerConfiguration );
public:  vector<PlayerConfiguration *> GetPlayerConfigurations();
public:  PlayerConfiguration * GetPlayerConfiguration(string strDriverName,string strDeviceName,string strModelId);

// Metodos para carregar e salvar um arquivo de configuracao
public:  static ConfigurationFile * LoadConfiguration(string strConfigurationFile, string strPortNumber,VirtualWorld * virtualWorld  );
public:  void   SaveConfiguration();
};

#endif

