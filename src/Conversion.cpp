#include "Conversion.h"
#include "SystemException.h"
#include <errno.h>

// Metodos para coverter cada um dos tips em string
string Conversion::BoolToString(bool value)
{
  if (value == true)
  {
    return string("true");
  }
  else
  {
    return string("false");
  }
}
string Conversion::IntegerToString(int value)
{
  return Conversion::LongToString(value);
}
string Conversion::LongToString(long value)
{
  char buffer[255];
  sprintf(buffer,"%i",value);
  return string(buffer);
}
string Conversion::DoubleToString(double value)
{
  char buffer[255];
  sprintf(buffer,"%.3f",value);
  return string(buffer); 
}
string Conversion::GzVectorToString(GzVector position)
{
  char buffer[255];
  sprintf(buffer,"%.3f %.3f %.3f",position.x,position.y,position.z);
  return string(buffer);
}
string Conversion::GzColorToString(GzColor color)
{
  GzColor valor = color;
  char buffer[255];
  sprintf(buffer,"%.3f %.3f %.3f %.3f",color.r,color.g,color.b,color.a);
  return string(buffer);
}
string Conversion::DoubleTupleToString(DoubleTuple tuple)
{
  char buffer[255];
  sprintf(buffer,"%.3f %.3f",tuple.x,tuple.y);
  return string(buffer);
}
string Conversion::IntegerTupleToString(IntegerTuple tuple)
{
  char buffer[255];
  sprintf(buffer,"%i %i",tuple.x,tuple.y);
  return string(buffer);
}
// Metodos para converter a string em cada um dos tipos
bool Conversion::StringToBool(string value)
{
  if ((value == string("true"))||(value == string("1")))
  {
    return true;
  }
  else if ((value == string("false"))||(value == string("-1"))||(value == string("0")))
  {
    return false;
  }
  else
  {
    throw new SystemException("Conversion::StringToBool","Valor da string nao representa um booleano");
  }
}
int  Conversion::StringToInt(string value)
{
  errno = 0;
  int numero = atoi(value.c_str());
  int erro = errno;
  if (errno != 0)
  {
    throw new SystemException("Conversion::StringToInt","Ocorreu um erro ao converter string para int");
  }
  return numero;
}
long Conversion::StringToLong(string value)
{
  errno = 0;
  long numero = atol(value.c_str());
  int erro = errno; 
  if (erro != 0)
  {
    throw new SystemException("Conversion::StringToLong","Ocorreu um erro ao converter string para long");
  }
  return numero;
}
double Conversion::StringToDouble(string value)
{
  errno=0;
  double numero = atof(value.c_str());
  int erro = errno;
  if(errno != 0)
  {
    throw new SystemException("Conversion::StringToDouble","Ocorreu um erro ao converter string para double");
  }
  return numero;
}
GzVector Conversion::StringToGzVector(string value)
{
  // GzVector a ser retornado
  GzVector v;
  string numero;
  int i;

  // Obtem x
  numero = string("");
  i =0;
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  v.x = Conversion::StringToDouble(numero);
   // Obtem y
  numero = string("");
  //i =0;
  i++;
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  v.y = Conversion::StringToDouble(numero);
   // Obtem z
  numero = string("");
  //i =0;
  i++;
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  v.z = Conversion::StringToDouble(numero);
  
  return v;
}
GzColor Conversion::StringToGzColor(string value)
{
  string valor = value;
  // GzVector a ser retornado
  GzColor color;
  string numero;
  int i;

  // Obtem r
  numero = string("");
  i =0;
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  color.r = Conversion::StringToDouble(numero);
  while ((value[i] == ' ') && (i < value.size()))
  {
    i++;
  }
   // Obtem g
  numero = string("");
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  color.g = Conversion::StringToDouble(numero);
  while ((value[i] == ' ') && (i < value.size()))
  {
    i++;
  }
   // Obtem b
  numero = string("");
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  color.b = Conversion::StringToDouble(numero);
  while ((value[i] == ' ') && (i < value.size()))
  {
    i++;
  }
  // Obtem alfa
  numero = string("");
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  color.a = Conversion::StringToDouble(numero); 
  
  return color;
}
DoubleTuple Conversion::StringToDoubleTuple(string value)
{
  string numero;
  int i;

  // Obtem 1o double
  numero = string("");
  i =0;
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  double c1 = Conversion::StringToDouble(numero);

  // Obtem 2o double
  i++;
  numero = string("");
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  double c2 = Conversion::StringToDouble(numero);

  // Instancia um DoubleTuple com oss dois valores e retorna
  return DoubleTupleSet(c1,c2);
}
IntegerTuple Conversion::StringToIntegerTuple(string value)
{
  string numero;
  int i;

  // Obtem 1o integer
  numero = string("");
  i =0;
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  int c1 = Conversion::StringToInt(numero);

  // Obtem 2o integer
  i++;
  numero = string("");
  while((value[i] != ' ') && (i < value.size()))
  {
    numero += value[i];
    i++;
  }
  int c2 = Conversion::StringToInt(numero);

  // Retorna uma tupla de inteiros
  return IntegerTupleSet(c1,c2);
}