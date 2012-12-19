
#include "SimulationConfiguration.h"

SimulationConfiguration::SimulationConfiguration(void)
{
  this->name = "";
  this->device = "";
  this->plugin = "";
  this->serverID = "";
}

SimulationConfiguration::~SimulationConfiguration(void)
{
}

string SimulationConfiguration::GetName()
{
  return this->name;
}
void   SimulationConfiguration::SetName(string name)
{
  this->name = name;
}

string SimulationConfiguration::GetDevice()
{
  return this->device;
}
void SimulationConfiguration::SetDevice(string device)
{
  this->device = device;
}

string SimulationConfiguration::GetPlugIn()
{
  return this->plugin;
}
void SimulationConfiguration::SetPlugIn(string plugin)
{
  this->plugin = plugin;
}

string SimulationConfiguration::GetServerID()
{
  return this->serverID;
}
void SimulationConfiguration::SetServerID(string serverID)
{
  this->serverID = serverID;
}