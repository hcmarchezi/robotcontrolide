#ifndef APPWORLDBUILDER_H
#define APPWORLDBUILDER_H

#ifdef WIN32
#include <windows.h>
#endif

#include <vector>
#include <string>
#include <map>
#include "WorldFile.h"
#include "Model.h"

using namespace std;

// Dentro do padrao MVC, esta classe funciona como o Model sendo ela
// que contem a logica de negocio provendo os "serviços"
class AppWorldBuilder
{
public: ~AppWorldBuilder();
public: void LoadWorldFile(const char * filename,WorldFile * world);
public: void NewWorldFile (const char * filename,WorldFile * world);
public: void SaveWorldFile(const char * filename,WorldFile * world);

private: vector<Model *> models;
public: vector<Model *> * GetModels();
        
//public: void GetPropertyList(Model * model,map<string,string> * modelProperties);
//public: void SetPropertyList(Model * model,map<string,string> * modelProperties);

};

#endif


