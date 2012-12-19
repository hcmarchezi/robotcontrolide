
#ifndef PLAYERCONFIGURATION_H
#define PLAYERCONFIGURATION_H

#include <string>
#include "Model.hh"
#include <vector>

using namespace std;

// Representa uma configuração do Player que sera gravado num arquivo 
// de configuracao
class PlayerConfiguration
{
public: PlayerConfiguration();
public: PlayerConfiguration(string strDriverName,string strDeviceName,Model * model);
public: virtual ~PlayerConfiguration(void);

// nome do driver
private: string driverName; 
public:  string GetDriverName();
public:  void SetDriverName(string driverName);

// nome da interface do dispositivo implementado pelo driver
private: string deviceName;
public:  string GetDeviceName();
public:  void   SetDeviceName(string deviceName);

// identificacao do modelo 3D a ser controlado pelo driver
private: Model * model;
public:  Model * GetModel();
public:  void   SetModel(Model * model);
};

#endif

