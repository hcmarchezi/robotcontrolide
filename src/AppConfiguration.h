
#ifndef APPCONFIGURATION_H
#define APPCONFIGURATION_H

#include <string>
#include "ConfigurationFile.h"
#include "Model.hh"
#include "IRCEProject.h"

using namespace std;

class AppConfiguration
{
public: AppConfiguration(void);
public: virtual ~AppConfiguration(void);

public: IRCEProject * project;

public: void SetSimulationConfiguration(string strName,string strDevice,string strPlugIn,string strServerId,ConfigurationFile * configurationFile);

public: void AddPlayerConfigurationFile(string strName,string strDevice,Model * model,ConfigurationFile * configurationFile);
public: void UpdatePlayerConfigurationFile(string strName,string strDevice,Model * model,PlayerConfiguration * playerConfiguration,ConfigurationFile * configurationFile);
public: void RemovePlayerConfigurationFile(PlayerConfiguration * playerConfiguration,ConfigurationFile * configurationFile);

public: void AddConfigurationFile(string strPath);
public: void RemoveConfigurationFile(ConfigurationFile * configurationFile);
public: void LoadConfigurationFile(string strPath);
public: void SaveConfigurationFile(string strPortNumber,ConfigurationFile * configurationFile);

};

#endif

