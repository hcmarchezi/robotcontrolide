
#ifndef VIRTUALWORLD_H
#define VIRTUALWORLD_H

#include "Vector.hh"
#include "World.hh"
#include <vector>

class Model;

using namespace std;

class VirtualWorld : public World
{
public: VirtualWorld(int serverId, bool serverForce);
public: VirtualWorld(void);
public: virtual ~VirtualWorld(void);

// Global Parameters
private: GzVector gravity;
public:  GzVector GetGravity();
public:  void     SetGravity(GzVector gravity);

// Obtem uma referencia para a lista de modelos do mundo virtual
public:  Model *** GetModelsReference();
public:  void CleanModels();
public:  void Add3DModel(Model * model);
public:  void Add3DModel(string strModelType,string strModelID, GzVector position,Model* parent=NULL);
public:  void RemoveModel(Model* model);
/////////////////////////public:  vector<Model*> * GetModels();
public:  Model * GetModel(const char * modelId);  ////////////////Model * GetModel(string strID);
public:  bool IsAddedModel(Model* model);
/////////////////////////private: vector<Model*> arrAux3DModels;
private: Model ** auxModels;
/////////////vector<Model*> GetModels(Model * model);
private: Model ** parentModels;
private: int parentModelCount, parentMaxModelCount;
private: void AddParentModel(Model * model);
public:  Model ** GetParentModels(); //////////////////vector<Model*> * GetParentModels();
public: int GetParentNumModels() const;
// ESSE NUMERO ABAIXO TEM QUE SER GUARDADO EM ARQUIVO !!!

// Métodos de Arquivo de Mundo Virtual
private: string worldFile;
private: bool isWorldFileSaved;
public:  static VirtualWorld * LoadWorldFile(string strWorldFile);
public:  void SaveWorldFile();
public:  string GetWorldFile();
public:  void SetWorldFile(string strWorldFile);
public:  bool HasSavedWorldFile();

// Mantem uma referencia para o objeto World
// para fazer os modelos funcionarem
//public: World * world;
 
};

#endif

