
#include "PlayerConfiguration.h"

#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include "ApplicationException.h"
#include "SystemException.h"
#include <vector>
#include "Model.hh" 
#include "ModelFactory.hh" 

using namespace std;

PlayerConfiguration::PlayerConfiguration()
{
}

PlayerConfiguration::PlayerConfiguration(string strDriverName,string strDeviceName,Model * model)
{
  this->SetDriverName(strDriverName);
  this->SetDeviceName(strDeviceName);
  this->SetModel(model);  
}

PlayerConfiguration::~PlayerConfiguration(void)
{
}

string PlayerConfiguration::GetDriverName()
{
  return this->driverName;
}

void PlayerConfiguration::SetDriverName(string driverName)
{
  this->driverName = driverName;
}

string PlayerConfiguration::GetDeviceName()
{
  return this->deviceName;
}

void PlayerConfiguration::SetDeviceName(string deviceName)
{
  this->deviceName = deviceName;
}

Model * PlayerConfiguration::GetModel()
{
  return this->model;
}
void   PlayerConfiguration::SetModel(Model * model)
{
  this->model = model;
}
