
#ifndef SIMULATIONCONFIGURATION_H
#define SIMULATIONCONFIGURATION_H

#include<string>

using namespace std;

class SimulationConfiguration
{
public: SimulationConfiguration(void);
public: virtual ~SimulationConfiguration(void);

private: string name;
public: string GetName();
public: void   SetName(string name);

private: string device;
public:  string GetDevice();
public:  void SetDevice(string device);

private: string plugin;
public:  string GetPlugIn();
public:  void SetPlugIn(string plugin);

private: string serverID;
public:  string GetServerID();
public:  void SetServerID(string serverID);
};

#endif

