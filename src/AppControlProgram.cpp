
#include "AppControlProgram.h"
#include "SystemException.h"
#include "FileName.h"

AppControlProgram::AppControlProgram(void)
{
  this->project = NULL;
}

AppControlProgram::~AppControlProgram(void)
{
}

void AppControlProgram::AddControlProgram(string strPath) //, string strDriver,string strDevice, Model * model, string strPort)
{
  //// Modelo 3D nao existe no projeto
  //if ( ! this->project->GetVirtualWorld()->IsAddedModel( model ) )
  //{
  //  throw new SystemException("AppControlProgram::AddControlProgram","Modelo 3D não foi existe no projeto IRCE");
  //}
  //// Adiciona uma configuracao para o controle para o projeto
  //PlayerConfiguration * playerConfiguration = new PlayerConfiguration( strDriver,strDevice,model,strPort);   
  //this->project->AddPlayerConfiguration( playerConfiguration );  

  // Transforma o path absoluto informado em path relativo ao diretorio do arquivo de projeto  
  strPath = FileName::GetRelativeFilename( this->project->GetProjectDirectory(), strPath );
  // Adiciona um programa de controle ao projeto
  this->project->AddPlayerControlProgram( new PlayerControl( strPath, "C++" ) );
}

void AppControlProgram::RemoveControlProgram(PlayerControl * playerControl)
{
  // Remove um programa de controle do projeto
  this->project->RemovePlayerControlProgram( playerControl );
}

void AppControlProgram::CompileControlProgram(PlayerControl * playerControl,vector<string> & arrStrOutput,vector<string> & arrStrErrors)
{
  // Compila o programa de controle informado
  this->project->CompilePlayerControlProgram( playerControl, arrStrOutput, arrStrErrors);
}

void AppControlProgram::LoadControlProgram(string strPath)
{
  // Atribui o diretorio corrente para o diretorio do projeto
  //wxSetWorkingDirectory(wxString(this->project->GetProjectDirectory().c_str()));
  // Transforma o path absoluto em path relativo 
  string strRelativePath = FileName::GetRelativeFilename( this->project->GetProjectDirectory(), strPath );
  // Carrega um programa de controle
  this->project->AddPlayerControlProgram( PlayerControl::LoadControlProgram( strRelativePath, "C++" ) );
}

void AppControlProgram::SaveControlProgram(PlayerControl * playerControl)
{
  // Atribui o diretorio corrente para o diretorio do projeto
  //wxSetWorkingDirectory(wxString(this->project->GetProjectDirectory().c_str()));
  // Salva o arquivo de controle pelo diretorio corrente
  playerControl->SaveControlProgram();
}