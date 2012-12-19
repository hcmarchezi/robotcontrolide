#include "RenderOptionsSingleton.h"

RenderOptions * RenderOptionsSingleton::instance = 0;

RenderOptions * RenderOptionsSingleton::GetInstance() 
{ 
  if (instance == 0 ) 
  { 
    instance = new RenderOptions();
    instance->displayBBox = false;
    instance->displayAxes = false;
    instance->displayCoM = false;
    instance->displaySkins = false;
    instance->displayRays = false;
    instance->displayFrustrums = false;
    instance->displayMaterials = true;
    instance->displayTextures = true;
    instance->polygonMode = GL_FILL;
    instance->shadeModel = GL_SMOOTH;
    
    instance->farClip = 150.0; // numero equivale ao volume de renderizacao
  }
  return RenderOptionsSingleton::instance; 
}

void RenderOptionsSingleton::DeleteInstance() 
{ 
  delete RenderOptionsSingleton::instance; 
}