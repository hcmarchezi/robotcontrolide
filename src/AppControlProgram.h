
#ifndef APPCONTROLPROGRAM_H
#define APPCONTROLPROGRAM_H

#include "IRCEProject.h"
#include "PlayerControl.h"
#include <string>

using namespace std;

class AppControlProgram
{
public: IRCEProject * project;

public: AppControlProgram(void);
public: virtual ~AppControlProgram(void);

public: void AddControlProgram(string strPath); //, string strDriver,string strDevice, Model * model, string strPort);
public: void RemoveControlProgram(PlayerControl * playerControl);
public: void CompileControlProgram(PlayerControl * playerControl,vector<string> & arrStrOutput,vector<string> & arrStrErrors);
public: void LoadControlProgram(string strPath);
public: void SaveControlProgram(PlayerControl * playerControl);

};

#endif

