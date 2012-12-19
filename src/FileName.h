#ifndef FILENAME_H
#define FILENAME_H

#include <string>

using namespace std;

class FileName
{
private: static string strReturn;
public: static string GetRelativeFilename(string strBaseDirectory,string strAbsolutePath);
public: static string GetAbsoluteFilename(string strBaseDirectory,string strRelativePath);
public: static bool IsAbsolutePath(string strPath);
public: static bool IsRelativePath(string strPath);
};

#endif