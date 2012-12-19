#ifndef RENDEROPTIONSSINGLETON_H
#define RENDEROPTIONSSINGLETON_H

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "RenderOptions.hh"

/////////////////////////////////////////////////////////////////////////////////////
// Para evitar que a classe RenderOptions tenha que ser referenciada por cada 
// parametro e metodo do sistema, foi criada esta classe que funciona como um singleton
// em torno de RenderOptions e permite que esta seja obtida de qualquer lugar do sistema.
/////////////////////////////////////////////////////////////////////////////////////
class RenderOptionsSingleton
{
private: static RenderOptions *instance;
public: static RenderOptions * GetInstance();
public: static void DeleteInstance();
};

#endif

