
#include "AppVirtualWorld.h"
////////////////////////////////////////////////#include "Exception.h" 
#include "ExceptionHandler.h"
#include "SystemException.h" 
#include "ApplicationException.h"
#include "Conversion.h"
#include "ModelFactory.hh"

#include <wx/wx.h>
#include "FileName.h"

#include "ModelUtility.h"

#include "TestModelFields.h"


AppVirtualWorld::AppVirtualWorld(void)
{
  this->project = NULL;
  this->i3DModelIDCounter = 0;
}

AppVirtualWorld::~AppVirtualWorld(void)
{
}

void AppVirtualWorld::NewWorldFile( string worldFullPath )
{
  // Verifica se ja existe um arquivo no caminho especificado
  // Se houver um arquivo, pede confirmacao para sobreescrever
  if ( wxFileExists( wxString( worldFullPath.c_str() ) ) )
  {
    wxMessageDialog msgDlg(NULL," Ja existe um arquivo no caminho especificado. Deseja sobreescreve-lo ? ","Aviso",wxYES_NO);
    if ( msgDlg.ShowModal() == wxNO )
    {
      return;
    }
  }

  // Se houver um mundo virtual não salvo pede confirmação da operação
  if ( ! this->project->GetVirtualWorld()->HasSavedWorldFile() )
  {
    wxMessageDialog msgDlg(NULL," O projeto IRCE atual possui um mundo virtual não salvo. Prosseguir ? ","Aviso",wxYES_NO);
    if ( msgDlg.ShowModal() == wxNO )
    {
      return;
    }
  }
  // Transforma o path absoluto informado em path relativo  
  worldFullPath = FileName::GetRelativeFilename( this->project->GetProjectDirectory(), worldFullPath );
  // Cria um novo arquivo de mundo virtual
  this->project->GetVirtualWorld()->SetWorldFile( worldFullPath );
}

void AppVirtualWorld::CleanVirtualWorld( )
{
  // Se houver projeto limpa os modelos
  if ( this->project != NULL)
  {
    this->project->GetVirtualWorld()->CleanModels();
  }
  else
  {
    throw new SystemException("Projeto nao foi criado","AppVirtualWorld::CleanVirtualWorld");
  }
}

void AppVirtualWorld::LoadVirtualWorld( string worldFullPath )
{
  // Se houver models associados pede uma confirmacao antes de carregar outro arquivo
  if ( this->project->GetVirtualWorld()->GetNumModels() > 0 && (!this->project->IsSaved()))
  {
    wxMessageDialog messageBox(NULL, wxT("Existem dados nao salvo no projeto. Deseja prosseguir e perder as informacoes alteradas ?"), wxT("Confirmacao"), wxYES_NO );
    // Se nao confirma, cancela o carregamento do arquivo
    if ( messageBox.ShowModal() == wxNO )
    {
      return;
    }
  }
  // Atribui o diretorio corrente para o diretorio do projeto
  //wxSetWorkingDirectory(wxString(this->project->GetProjectDirectory().c_str()));
  // Transforma o path relativo em absoluto  
  worldFullPath = FileName::GetRelativeFilename( this->project->GetProjectDirectory(), worldFullPath );
  // Carrega o arquivo do mundo virtual
  this->project->SetVirtualWorld( VirtualWorld::LoadWorldFile( worldFullPath ) );
}

void AppVirtualWorld::SaveWorldFile( )
{
  // Atribui o diretorio corrente para o diretorio do projeto
  //wxSetWorkingDirectory(wxString(this->project->GetProjectDirectory().c_str()));
  // Salva o arquivo do mundo virtual utilizando o diretorio do projeto como base
  this->project->GetVirtualWorld()->SaveWorldFile();
}

void AppVirtualWorld::Add3DModel( string modelType, string modelID, GzVector position )
{
  if ( modelID == "" )
  {
    string strID = modelType + string("_") + Conversion::IntegerToString(this->project->GetVirtualWorld()->GetNumModels());
    this->i3DModelIDCounter++;
  }
  this->project->GetVirtualWorld()->Add3DModel( modelType,modelID, position);
}
void AppVirtualWorld::Update3DModel( Model * model, map<string,string> * properties )
{
  //////////////////////////////////////////////////
  throw new SystemException("Metodo nao implementado","AppVirtualWorld::Update3DModel");
}
void AppVirtualWorld::Update3DModelPosition( Model * model, GzVector position )
{
  //////////////////////////////////////////////////
  throw new SystemException("Metodo nao implementado","AppVirtualWorld::Update3DModelPosition");
}
void AppVirtualWorld::Remove3DModel( Model * model )
{
  this->project->GetVirtualWorld()->RemoveModel(model);
}

void AppVirtualWorld::GetPropertyList( Model * model,map<string,string> ** modelProperties )
{
  //(*modelProperties) = FieldExtractor::GetStringFieldsFromModel(model);
  (*modelProperties) = ModelUtility::GetStringFieldsFromModel(model);
}

void AppVirtualWorld::SetPropertyList( Model * model,map<string,string> * modelProperties )
{
  //map<string,string>::iterator iterator;
  //for (iterator = modelProperties->begin(); iterator != modelProperties->end(); iterator++)
  //{
  //  FieldExtractor::SetFieldsToModel(model,modelProperties);   
  //}  
  ModelUtility::SetFieldsToModel(model,modelProperties);
}

void AppVirtualWorld::AddChildModel( Model * parentModel, string strModelType, string strModelID, GzVector position )
{
  if ( strModelID == "" )
  {
    strModelID = strModelID + string("_") + Conversion::IntegerToString(this->project->GetVirtualWorld()->GetNumModels());
    this->i3DModelIDCounter++;
  }
  
  //Model * childModel = ModelFactory::NewModel(this->project->GetVirtualWorld()->world,strModelType.c_str());
  //childModel->SetId(strModelID.c_str());
  //GzPose pose = childModel->GetPose();
  //pose.pos = position;
  //childModel->SetPose(pose);
  //parentModel->AddModel(childModel);  

  this->project->GetVirtualWorld()->Add3DModel(strModelType,strModelID,position,parentModel);
  
}

void AppVirtualWorld::RemoveChildModel( Model * parentModel, Model * childModel )
{
  //parentModel->RemoveModel(childModel);
  this->project->GetVirtualWorld()->RemoveModel(childModel);
}

