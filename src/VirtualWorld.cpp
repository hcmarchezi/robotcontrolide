
#include "VirtualWorld.h"
#include "XMLCommand.h"
#include "ModelFactory.hh"
#include "ApplicationException.h"
#include <wx/textfile.h>
#include "ModelUtility.h"

#include "Model.hh"

#include <wx/wx.h>

#include "SimpleSolid.hh"

VirtualWorld::VirtualWorld(int serverId, bool serverForce):World(serverId,serverForce)
{
  this->parentModelCount = 0;
  this->parentMaxModelCount = 0;

//  this->i3DModelIDCounter = 0;
  this->isWorldFileSaved = true;
  this->models = NULL;
  this->parentModels = NULL;
}

VirtualWorld::VirtualWorld(void):World(0,false)
{
  this->parentModelCount = 0;
  this->parentMaxModelCount = 0;

//  this->i3DModelIDCounter = 0;
  this->isWorldFileSaved = true;
 // this->world = new World(0,false); // parametros do objeto World nao importam
  this->models = NULL;
  this->parentModels = NULL;
}

VirtualWorld::~VirtualWorld(void)
{
}

GzVector VirtualWorld::GetGravity()
{
  return this->gravity;
}
void VirtualWorld::SetGravity(GzVector gravity)
{
  this->gravity = gravity;
}

Model *** VirtualWorld::GetModelsReference()
{
  return &this->models;
}

void VirtualWorld::CleanModels()
{
  for (int index =0; index < this->parentModelCount; index++)
  {
    delete this->parentModels[index];
  }
  if ( this->parentModels != NULL )
  {
    delete this->parentModels;
  }

  for (int index =0; index < this->modelCount; index++)
  {
    delete this->models[index];
  }
  if ( this->models != NULL )
  {
    delete this->models;
  }
  this->modelCount = 0;
  this->modelMaxCount = 0;
}

void VirtualWorld::Add3DModel(Model * model)
{
  if (model->GetId() == "")
  { 
    throw new SystemException("Identificacao do modelo nao informada","VirtualWorld::AddModel");
  }

  //if (model->virtualWorld == NULL)
  if (model->world == NULL)
  {
    throw new SystemException("Model nao possui um mundo virtual relacionado","VirtualWorld::AddModel");
  }
  string strModelType = ModelUtility::GetModelTypeName(model);

  this->AddModel(model);

  // Se o modelo for uma luz entao este deve ser inserido na primeira posicao do vetor
  if ( strModelType == "LightSource" )
  {
    //vector<Model*>::iterator it = this->arrModels.begin();
    //this->arrModels.insert(it,model);
    Model * firstModel = this->GetModels()[0];
    Model * lastModel  = this->GetModels()[this->GetNumModels()-1];
    this->GetModels()[0] = lastModel;
    this->GetModels()[this->GetNumModels()-1] = firstModel;
  }
  else
  {
    //this->arrModels.push_back(model);    
  }

  // Adiciona o modelo aos modelos pais
  if (model->parent == NULL)
  {
    this->AddParentModel(model);
  }
}

void VirtualWorld::Add3DModel(string strModelType,string strModelID, GzVector position,Model* parent)
{
  if (strModelType == "") throw new SystemException("Tipo de modelo nao informado","VirtualWorld::AddModel");
  if (strModelID == "") throw new SystemException("Identificacao do modelo nao informada","VirtualWorld::AddModel");

  Model * model = ModelUtility::NewModel(this,strModelType.c_str());
  model->SetId(strModelID.c_str());

  model->parent = parent;
  //model->virtualWorld = this;

  // Atribui a posicao informada
  GzPose pose = model->GetPose();
  pose.pos = position;
  model->SetPose(pose);

//   // Se o modelo for uma luz entao vai para a primeira posicao do vetor de modelos
//   if ( strModelType == "LightSource" )
//   {
//     vector<Model*>::iterator it = this->arrModels.begin();
//     this->arrModels.insert(it,model);
//   }
//   else
//   {
//     // Se nao existir, insere o modelo
//     this->arrModels.push_back(model);
//   }

  this->Add3DModel(model);

}
void VirtualWorld::RemoveModel(Model* model)
{
  // Procura o modelo 3D no projeto IRCE
//   vector<Model*>::iterator it = this->arrModels.begin();
//   while (it != this->arrModels.end())
//   {
//     if ((*it)->GetId() == model->GetId())
//     {
//       // Se achou, remove da lista de modelos 3D
//       this->arrModels.erase(it);
//       return;
//     }
//     it++;
//   }

  int modelIndex = -1;
  for(int index=0; index < this->modelCount; index++)
  {
    Model * modelPtr = this->GetModels()[index];
    if (strcmp(modelPtr->GetId(),model->GetId())==0)
    {
      for (int index2 = index+1; index2 < this->modelCount;index2++)
      {
        this->GetModels()[index2-1] = this->GetModels()[index2];
      }
      this->modelCount--;
    }
  }

  // Se nao existir, mostra um erro
  throw new ApplicationException("Modelo 3D nao existe no projeto IRCE");
}

// vector<Model*> * VirtualWorld::GetModels()
// {
//   return &this->arrModels;
// }

Model * VirtualWorld::GetModel(const char * strID)
{
//   // Obtem todos os modelos do mundo virtual
//   vector<Model *> arrAllModels;
//   arrAllModels.insert(arrAllModels.end(),this->arrModels.begin(),this->arrModels.end()); //push_back(this->arrModels);
//   for (int index=0; index < this->arrModels.size(); index++)
//   {
//     vector<Model *> arrSubModels = this->GetModels(this->arrModels[index]);
//     arrAllModels.insert(arrAllModels.end(),arrSubModels.begin(),arrSubModels.end());
//   }

  Model ** arrAllModels = this->GetModels();

  // Obtem o modelo pelo id
  for (int index=0; index < this->modelCount; index++)
  {
    Model * model = arrAllModels[index];

    const char * strModelID = model->GetId();
    if (strcmp(strID, strModelID) == 0)
    {
      return model;
    }
  }
  return NULL;
}
bool VirtualWorld::IsAddedModel(Model* model)
{
//   vector<Model *>::iterator it = this->arrModels.begin();
//   while ( it != this->arrModels.end() )
//   {
//     if ((*it) == model)
//     {
//       return true;
//     }
//     it++;
//   }
//   return false;

  const char * strID = model->GetId();
  for (int index=0; index < this->modelCount; index++)
  {
    Model * ptrModel = this->GetModels()[index];
    const char * strModelID = ptrModel->GetId();    
    if (strcmp(strID, strModelID) == 0)
    {
      return true;
    }
  }
  return false;
}

// vector<Model*> VirtualWorld::GetModels(Model * model)
// {
//   vector<Model*> * arrModels = ModelUtility::GetChildModels(model);
//   this->arrAux3DModels.clear();
//   this->arrAux3DModels.insert(this->arrAux3DModels.end(),arrModels->begin(),arrModels->end());
//   delete arrModels;
//   return this->arrAux3DModels;
// }

void VirtualWorld::AddParentModel(Model * model)
{
  // Re-size array as needed
  if (this->parentModelCount >= this->parentMaxModelCount)
  {
    this->parentMaxModelCount += 10;
    this->parentModels = (Model**) realloc( this->parentModels, this->parentMaxModelCount * sizeof(Model*));
    assert( this->parentModels );
  }

  this->parentModels[this->parentModelCount] = model;
  this->parentModelCount++;
  
  return;
}


int VirtualWorld::GetParentNumModels() const
{
  return this->parentModelCount;
}


Model** VirtualWorld::GetParentModels()
{
  return this->parentModels;

/*  vector<Model*> * arrParentModels = new vector<Model*>();
  for(int i=0; i < this->arrModels.size(); i++)
  {
    Model * model = this->arrModels[i];
   
    Model * parent = model->parent;
    const char * id = model->GetId();
    const char * strModelType = ModelUtility::GetModelTypeName(model); 
    GzVector pos = model->GetPose().pos;
   
//     wxString strDebug = "";
//     strDebug += " id = (";
//     strDebug += id;
//     strDebug += ") parent = (";
//     if (parent != NULL)
//     {
//       strDebug += "!NULL";
//     }
//     else
//     {
//       strDebug += "NULL";
//     }
//     strDebug += ") type = (";
//     strDebug += strModelType;
//     strDebug += ")";
// 
//     if (strcmp(strModelType,"SimpleSolid")==0)
//     {
//       strDebug += " shape = (";
//       strDebug += ((SimpleSolid*)model)->GetShape().c_str();
//       strDebug += ")";
//     }
//   
//     wxMessageDialog messageBox(NULL,strDebug, wxT("DEBUG"), wxOK );
//     messageBox.ShowModal();

    if ( model->parent == NULL )
    {
      arrParentModels->push_back(model);
    }
  }
  return arrParentModels;*/
}

VirtualWorld * VirtualWorld::LoadWorldFile(string strWorldFile)
{
//   POR QUE O WXGAZEBO NAO CONSEGUE LER O WORLD XML GERADO PELA APLICACAO ?
//   O QUE É PRECISO FAZER EM TESTE.WORLD PARA QUE O WXGAZEBO CONSIGA CARREGA-LO ?
//   COMENTAR O CODIGO ATE CHEGAR NO RESULTADO

  VirtualWorld * virtualWorld = new VirtualWorld();

  //virtualWorld->arrModels.clear();
  virtualWorld->CleanModels();

  virtualWorld->SetWorldFile( strWorldFile );
    
  // Carrega o arquivo XML do projeto
  TiXmlDocument doc;
  if (! doc.LoadFile( strWorldFile.c_str() ))
  {
    string strError = "Houve um erro ao abrir o arquivo de projeto ";
    strError += strWorldFile;
    throw new ApplicationException( strError.c_str() );
  }

  // Procura pelos Elementos model:*
  TiXmlElement * worldElement = doc.FirstChildElement("gz:world");

  TiXmlElement* modelElement = worldElement->FirstChildElement();
  while ( modelElement != NULL )
  {
    string strValue = modelElement->Value();
    int index = strValue.find("model:");
    if ( index > -1 )
    {
      Model * model = XMLCommand::Deserialize( modelElement, virtualWorld );
      //string strModelType = ModelFactory::GetModelTypeName(model);
      string strModelType = ModelUtility::GetModelTypeName(model);
      // Se o modelo for uma luz entao este deve ser inserido na primeira posicao do vetor
      //if ( strModelType == "LightSource" )
      //{
        //vector<Model*>::iterator it = virtualWorld->arrModels.begin();
        //virtualWorld->arrModels.insert(it,model);
        virtualWorld->Add3DModel(model);
      //}
      //else
      //{
        //virtualWorld->AddModel(model);
        //virtualWorld->arrModels.push_back( model );
      //}
    }
    
    modelElement = modelElement->NextSiblingElement();
  } 

  return virtualWorld;
}
void VirtualWorld::SaveWorldFile()
{ 
  
  if (( this->GetNumModels() > 0 )&&( this->worldFile == "" ))
  {
    throw new SystemException("IRCEProject::SaveWorldFile"," Mundo Virtual possui modelos 3D mas nao pode ser salvo pois não foi informado o nome do arquivo World");
  }
  else if ( this->worldFile == "" )
  {
    return;
  }

  TiXmlDocument doc("VirtualWorld");
  TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

  TiXmlElement * xmlGzWorldElement = new TiXmlElement("gz:world");
  xmlGzWorldElement->SetAttribute("xmlns:gz","\"http://playerstage.sourceforge.net/gazebo/xmlschema/#gz\"");
  xmlGzWorldElement->SetAttribute("xmlns:model","\"http://playerstage.sourceforge.net/gazebo/xmlschema/#model\"");
  xmlGzWorldElement->SetAttribute("xmlns:sensor","\"http://playerstage.sourceforge.net/gazebo/xmlschema/#sensor\"");
  xmlGzWorldElement->SetAttribute("xmlns:window","\"http://playerstage.sourceforge.net/gazebo/xmlschema/#window\"");
  xmlGzWorldElement->SetAttribute("xmlns:param","\"http://playerstage.sourceforge.net/gazebo/xmlschema/#params\"");
  xmlGzWorldElement->SetAttribute("xmlns:ui","\"http://playerstage.sourceforge.net/gazebo/xmlschema/#params\""); 
  doc.LinkEndChild( xmlGzWorldElement );
  
  //for (unsigned int i = 0; i < this->arrModels.size(); i++)
  for (int i = 0; i < this->GetNumModels(); i++)
  {
    //if ( this->arrModels[i]->parent == NULL )
    if ( this->GetModels()[i]->parent == NULL )
    {
      //TiXmlElement * xmlElement = XMLCommand::Serialize(this->arrModels[i]);
      TiXmlElement * xmlElement = XMLCommand::Serialize(this->GetModels()[i]);
      xmlGzWorldElement->LinkEndChild(xmlElement);
    }
  }

  doc.SaveFile( this->worldFile.c_str() );

  // Atualiza a flag de arquivo world salvo
  this->isWorldFileSaved = true;
}
string VirtualWorld::GetWorldFile()
{
  return this->worldFile;
}
void VirtualWorld::SetWorldFile(string strWorldFile)
{
  wxTextFile textFile(wxString( strWorldFile.c_str() ) );
  textFile.Create();
  textFile.Close();
  this->worldFile = strWorldFile; 
  this->isWorldFileSaved = true;  
}
bool VirtualWorld::HasSavedWorldFile()
{
  return this->isWorldFileSaved;
}

