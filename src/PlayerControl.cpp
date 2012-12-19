
#include "PlayerControl.h"
#include <wx/textfile.h>
#include <wx/filename.h>
#include "ApplicationException.h"
#include "SystemException.h"

PlayerControl::PlayerControl(string clientControlFile,string programmingLanguage)
{
  this->clientControlFile   = clientControlFile;
  this->programmingLanguage = programmingLanguage;
}

PlayerControl::~PlayerControl(void)
{
}

string PlayerControl::GetProgrammingLanguage()
{
  return this->programmingLanguage;
}
void  PlayerControl::SetProgrammingLanguage(string programmingLanguage)
{
  this->programmingLanguage = programmingLanguage;
}

string PlayerControl::GetClientControlFile()
{
  return this->clientControlFile;
}

void PlayerControl::SetClientControlFile(string clientControlFile)
{
  this->clientControlFile = clientControlFile;
}

vector<string> PlayerControl::GetSourceCode()
{
  return this->sourceCode;
}

void PlayerControl::SetSourceCode(vector<string> sourceCode)
{
  this->sourceCode = sourceCode;
}

// M�todos de programa de controle
PlayerControl * PlayerControl::LoadControlProgram(string strControlProgramFile,string strControlLanguage)
{
  // Verifica se o arquivo existe
  if (!wxFileExists(wxString(strControlProgramFile.c_str())))
  {
    throw new ApplicationException("O arquivo informado n�o foi encontrado");
  }

  // Abre o arquivo como arquivo texto
  wxTextFile file( wxString( strControlProgramFile.c_str() ) );
  if (!file.Open()) 
  {
    throw new SystemException("IRCEProject::LoadControlProgram","Ocorre um propblema ao abrir o arquivo do programa de controle");
  }

  // Traz a instancia do programa de controle no projeto IRCE
  PlayerControl * playerControl = new PlayerControl(strControlProgramFile, strControlLanguage);

  // Le todo o conteudo do arquivo 
  vector<string> arrStrFileContent;
  for (int i=0; i < file.GetLineCount(); i++)
  {
    arrStrFileContent.push_back( string( file.GetLine(i).c_str() ) );
  }

  // Atribui o conteudo ao programa de controle
  playerControl->SetSourceCode( arrStrFileContent );

  // Fecha o arquivo
  file.Close();

  return playerControl;
}

void PlayerControl::SaveControlProgram()
{
  wxTextFile playerControlFile( wxString( this->clientControlFile.c_str() ) );
  playerControlFile.Create();
    
  // Remove as linhas do arquivo de controle  
  for (int i=0; i < playerControlFile.GetLineCount(); i++)
  {
    playerControlFile.RemoveLine(i);
  }
  
  //playerControlFile.Close();
  //// Salva o C�digo-Fonte em playerControl 
  //playerControlFile.Open();

  for ( int j = 0; j < this->GetSourceCode().size(); j++)
  {
    wxString sourceLine = wxString( this->GetSourceCode()[j].c_str() );
    playerControlFile.AddLine( sourceLine );
  }

  if (! playerControlFile.Write()) 
  {
    throw new SystemException("PlayerControl::SaveControlProgram","Ocorreu um erro ao salvar o arquivo do programa de controle");
  }

}

bool PlayerControl::IsCompiled()
{
  string strCompiledControlFile = "";
  strCompiledControlFile.append( this->GetClientControlFile() );
  strCompiledControlFile.append(".bin");
  return wxFileExists(strCompiledControlFile.c_str());
}