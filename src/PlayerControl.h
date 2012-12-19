
#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H 

#include<string>
#include<vector>

using namespace std;

// Representa um controle cliente de um dispositivo que é
// interpretado pelo servidor de controle Player
class PlayerControl
{
public: PlayerControl(string clientControlFile,string programmingLanguage);
public: virtual ~PlayerControl(void);

private: string programmingLanguage;
public:  string GetProgrammingLanguage();
public:  void  SetProgrammingLanguage(string programmingLanguage);

// Obtem e atribui o arquivo de controle cliente
private: string clientControlFile;
public:  string GetClientControlFile();
public:  void SetClientControlFile(string clientControlFile);

// Obtem e atribui o texto do codigo-fonte do controle cliente
private: vector<string> sourceCode;
public:  vector<string> GetSourceCode();
public:  void SetSourceCode(vector<string> sourceCode);

// Métodos de programa de controle
public:  static PlayerControl * LoadControlProgram(string strControlProgramFile,string strControlLanguage);
public:  void SaveControlProgram();

public:  bool IsCompiled();

};

#endif

