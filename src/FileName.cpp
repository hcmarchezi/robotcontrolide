
#include "FileName.h"
#include <wx/filename.h>
#include "SystemException.h"

string FileName::strReturn = "";

string FileName::GetRelativeFilename(string strBaseDirectory,string strAbsolutePath)
{   
  strBaseDirectory += wxFileName::GetPathSeparators();
  wxFileName fileName(strAbsolutePath.c_str());
  bool success = fileName.MakeRelativeTo(strBaseDirectory.c_str());
  if (!success)
  {
    string strMessage = "Falha ao transformar o path absoluto (";
    strMessage += strAbsolutePath;
    strMessage += ") em path relativo.";
    throw new SystemException("FileName::GetRelativeFilename",strMessage.c_str());
  }
  string strRelativePath( fileName.GetFullPath().c_str() );
  strReturn = strRelativePath;
  return strReturn;
}
string FileName::GetAbsoluteFilename(string strBaseDirectory,string strRelativePath)
{ 
  strBaseDirectory += wxFileName::GetPathSeparators();
  wxFileName fileName(strRelativePath.c_str());
  bool success = fileName.MakeAbsolute(strBaseDirectory.c_str());
  if (!success)
  {
    string strMessage = "Falha ao transformar o path relativo (";
    strMessage += strRelativePath;
    strMessage += ") em path absoluto.";
    throw new SystemException("FileName::GetAbsoluteFilename",strMessage.c_str());
  }
  string strAbsolutePath( fileName.GetFullPath().c_str() );
  strReturn = strAbsolutePath;
  return strReturn;
}
bool FileName::IsAbsolutePath(string strPath)
{
  wxFileName fileName( strPath.c_str() );
  return fileName.IsAbsolute();
}
bool FileName::IsRelativePath(string strPath)
{
  wxFileName fileName( strPath.c_str() );
  return fileName.IsRelative();
}


