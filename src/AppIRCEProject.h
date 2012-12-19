
#ifndef APPIRCEPROJECT_H
#define APPIRCEPROJECT_H

#include "IRCEProject.h"
#include <string>

using namespace std;

class AppIRCEProject
{
public: AppIRCEProject(void);
public: virtual ~AppIRCEProject(void);
 
// Projeto corrente da aplicacao
public: IRCEProject * project;

public: void CreateIRCEProject(string fullPath);
public: void EditIRCEProject(string fullPath);
public: void SaveIRCEProject();
public: void LoadIRCEProject(string fullPath);

public: void SimulateIRCEProject(wxEvtHandler * parentWindow);
public: void TransferControl();
};

#endif


