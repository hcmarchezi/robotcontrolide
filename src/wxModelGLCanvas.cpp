/***************************************************************************
 *   Copyright (C) 2006 by Humberto Cardoso Marchezi   *
 *   hcmarchezi@linux   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "wxModelGLCanvas.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <cmath>

#include <iostream>
#include "SystemException.h"
#include <stdlib.h>

//#include "ModelFactory.hh"
#include "ModelUtility.h"
////////////////////////////////////////////////////#include "ViewOptions.h"
#include "RenderOptionsSingleton.h"

#define PI_NUMERIC 3.14156295

wxModelGLCanvas::wxModelGLCanvas(wxWindow *parent, wxWindowID id,const wxPoint& pos, 
				const wxSize& size, long style, const wxString& name) 
:wxGLCanvas(parent, id, pos, size, style, name)
{   
  //this->models = NULL;
  //this->modelCount = 0;
  this->virtualWorld = NULL;
  // Logica do Observador
  this->initGL = true;
  this->eyePosition = GzVectorSet(0.0,0.0,5.0);
  this->centerPosition = GzVectorSet(0.0,0.0,0.0);
  this->upVector = GzVectorSet(0.0,1.0,0.0);

  // Insere os contextos de visoes (views) X/Y, X/Z, Y/Z numa lista
  // e insere cada nome de visao no combobox de visoes  
  ViewContext* viewContext = new ViewContext();
  viewContext->name = wxT("X/Y");
  viewContext->orthographic = true;
  viewContext->eyePosition.x    = 0.0; viewContext->eyePosition.y    = 0.0; viewContext->eyePosition.z    = 0.0;
  viewContext->centerPosition.x = 0.0; viewContext->centerPosition.y = 0.0; viewContext->centerPosition.z = 0.0;
  viewContext->upVector.x       = 0.0; viewContext->upVector.y       = 1.0; viewContext->upVector.z       = 0.0;
  this->arrViewContexts.push_back(viewContext);

  viewContext = new ViewContext();
  viewContext->name = wxT("X/Z");
  viewContext->orthographic = true;
  viewContext->eyePosition.x    = 0.0; viewContext->eyePosition.y    = 0.0; viewContext->eyePosition.z    = 0.0;
  viewContext->centerPosition.x = 0.0; viewContext->centerPosition.y = 0.0; viewContext->centerPosition.z = 0.0;
  viewContext->upVector.x       = 0.0; viewContext->upVector.y       = 0.0; viewContext->upVector.z       = 1.0;
  this->arrViewContexts.push_back(viewContext);

  viewContext = new ViewContext();
  viewContext->name = wxT("Y/Z");
  viewContext->orthographic = true;
  viewContext->eyePosition.x    = 0.0; viewContext->eyePosition.y    = 0.0; viewContext->eyePosition.z    = 0.0;
  viewContext->centerPosition.x = 0.0; viewContext->centerPosition.y = 0.0; viewContext->centerPosition.z = 0.0;
  viewContext->upVector.x       = 0.0; viewContext->upVector.y       = 0.0; viewContext->upVector.z       = 1.0;
  this->arrViewContexts.push_back(viewContext);

  // Contexto corrente (posicao 0)(X/Y)
  this->iCurrentViewContext = 0;

  // Nome do contexto atual
  this->viewContextName = wxT("X/Y");

  // Evento é nulo no inicio
  this->glevent = NULL;

  // Conectando eventos
  this->Connect(id,id,wxEVT_PAINT,(wxObjectEventFunction)(wxEventFunction)(wxPaintEventFunction)&wxModelGLCanvas::OnPaint);
  this->Connect(id,id,wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)&wxModelGLCanvas::OnMouse);
  this->Connect(id,id,wxEVT_RIGHT_DOWN,(wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)&wxModelGLCanvas::OnMouse);
  this->Connect(id,id,wxEVT_MOTION,    (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)&wxModelGLCanvas::OnMouse);  
}


wxModelGLCanvas::~wxModelGLCanvas()
{
  if ( this->glevent != NULL)
  {
    delete this->glevent;
    this->glevent = NULL;
  }

}

// Inicia a configuracao do OpenGL
void wxModelGLCanvas::InitGL()
{
   glClearColor(0.5,0.5,1.0,0.0); 
  
   this->initGL = false;

   // Light   
   //GLfloat mat_shininess[] = { 50.0 };
   //glShadeModel(GL_SMOOTH);
   //glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  //Reset material properties
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, GzColor(0.2, 0.2, 0.2, 1.0));
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, GzColor(0.8, 0.8, 0.8, 1.0));
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, GzColor(0.0, 0.0, 0.0, 1.0));
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);

   glEnable(GL_DEPTH_TEST);
}

// Desenha o cenario 
void wxModelGLCanvas::Draw()
{
#ifndef WIN32
  this->SetCurrent();
#endif
  this->DrawScene();
  glFlush();
}

void wxModelGLCanvas::DrawScene()
{
  // Inicializaçao de OpenGL na 1a execucao
  if (this->initGL)
  {
    this->InitGL();
  }

//   // O efeito de luz é desabilitado no modo WireMesh
//   if (RenderOptionsSingleton::GetInstance()->polygonMode == GL_LINE)
//   {
//     glDisable(GL_LIGHTING);
//   }
//   else
//   {
//     glEnable(GL_LIGHTING); 
//   }

  // Iniciar buffers de cor e profundidade
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Desenha a cena
  this->Render();

  // Faz o troca de buffers (para evitar efeito de flickering)
  this->SwapBuffers();
}


void wxModelGLCanvas::Render()
{
  //////////////////////////////////////////////////////
  // Lutando para fazer o OpenGL funcionar
  //////////////////////////////////////////////////////

  //Set global rendering properties
  glPolygonMode( GL_FRONT_AND_BACK, RenderOptionsSingleton::GetInstance()->polygonMode );
  glShadeModel(RenderOptionsSingleton::GetInstance()->shadeModel);

    // Ortographic  View
  if (this->bOrthographic)
  {
    // Volume de visualizacao
    glOrtho(-5.0,5.0,-5.0,5.0,-100.0,100.0);
    // Limpa a matriz de projecao 
    glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    // Executa a rotacao de acordo com o nome da camera 
    if (this->viewContextName == wxT("X/Y"))
    {     
      // Posiciona o observador no eyePosition
      glTranslated(-this->eyePosition.x,-this->eyePosition.y,this->eyePosition.z);    
    }
    else if (this->viewContextName == wxT("X/Z"))
    {
      glRotated( -90.0,1.0,0.0,0.0);
      glTranslated(this->eyePosition.x,-this->eyePosition.y,-this->eyePosition.z); 
    }
    else if (this->viewContextName == wxT("Y/Z"))
    {
      //glTranslated(5.0,0.0,0.0);
      glRotated(-90.0,0.0,0.0,1.0);
      glRotated(-90.0,0.0,1.0,0.0);
      glTranslated(-this->eyePosition.x,-this->eyePosition.y,-this->eyePosition.z); 
    }
  }

  // Camera View
  if (!this->bOrthographic)
  {     
    gluLookAt(
      this->eyePosition.x,
      this->eyePosition.y,
      this->eyePosition.z,
      this->centerPosition.x,
      this->centerPosition.y,
      this->centerPosition.z,
      this->upVector.x,
      this->upVector.y,
      this->upVector.z);
  }

  /////////////////////////////////////////////////////////
  // Faz o desenho do Cenario
  /////////////////////////////////////////////////////////
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // Desenha os eixos das coordenadas globais usando as cores RGB
  // red = eixo x, green = eixo y, blue = eixo z
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);  
  glColor3f(1.0,0.0,0.0);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(1.0,0.0,0.0);
  glColor3f(0.0,1.0,0.0);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(0.0,1.0,0.0);
  glColor3f(0.0,0.0,1.0);
  glVertex3d(0.0,0.0,0.0);
  glVertex3d(0.0,0.0,1.0);  
  glEnd();
  glEnable(GL_LIGHTING);

  // Inicia a pilha de nomes para a selecao de objetos
  // atraves da posicao do mouse
  glInitNames();
  glPushName(0);

////////// APENAS PARA TESTE /////////////////
//   glPushMatrix();
//   glTranslatef(1.0,1.0,0.0);
//   glLoadName(1);
//   glutWireTeapot(1.0);
//   glPopMatrix();
// 
//   glPushMatrix();
//   glTranslatef(-1.0,-1.0,0.0);
//   glLoadName(2);
//   glutWireCube(2.0);
//   glPopMatrix();
//   
//   glPushMatrix();
//   glTranslatef(1.0,1.0,0.0);
//   glLoadName(3);
//   glutWireIcosahedron();
//   glPopMatrix();
//////////////////////////////////////////////
  
 
  // Aqui onde os modelos serao desenhados
  if (this->virtualWorld != NULL)
  {
    RenderOptions * renderOpt = RenderOptionsSingleton::GetInstance();
    //for (int index=0; index < this->modelCount; index++)
    for (int index=0; index < this->virtualWorld->GetNumModels(); index++)
    {
      Model * model = this->virtualWorld->GetModels()[index];

      if ((this->viewContextName == wxT("X/Y"))||
          (this->viewContextName == wxT("Y/Z"))||
          (this->viewContextName == wxT("X/Z"))||
          ( ModelUtility::GetModelTypeName(model) != "ObserverCam" ))
          //( ModelFactory::GetModelTypeName((* this->models)[index]) != "ObserverCam" ))
      {
        //(* this->models)[index]->Render(true,renderOpt);
        //Model * model = (* this->models)[index];
        ModelUtility::RenderModel(model,true,renderOpt);
      }
    }
  }

  /////////////////////////////////////////////////
  // Flush
  // Camera View - 2nd Part
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (!this->bOrthographic)
  {
    gluPerspective(60.0,1.0,0.1,400.0); //(60.0,1.0,1.5,200.0);
  }
  
  glViewport(0,0,this->GetSize().GetWidth(),this->GetSize().GetHeight());

  glFlush();
}

Model * wxModelGLCanvas::SelectObjects(int xPos,int yPos)
{
  // Inicia o buffer de id's de objetos selecionados - até 512 objetos
  GLuint selectBuffer[512];
  for(int i=0;i < 512;i++)
  {
    selectBuffer[i] = 0;
  }
  // Inicia o buffer de id's de objetos de OpenGL
  glSelectBuffer(512,selectBuffer);

  //////////////////////////////////////////////////////////////
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);

  // Trabalhando com a matriz em mode de projecao
  glMatrixMode(GL_PROJECTION);
  //glPushMatrix();

  //Renderiza em mode de seleçao
  glRenderMode(GL_SELECT);

  // Estabelece um clipping volume para o mouse de 2 pixels
  // para a vertical e 2 pixels para a horizontal
  //glLoadIdentity();
  gluPickMatrix(xPos,viewport[3] - yPos,5,5,viewport);

  this->Render();

  // Captura objetos selecionados (quantidade de objeto na variavel hits)
  GLint hits = glRenderMode(GL_RENDER);

  glMatrixMode(GL_PROJECTION);
  //////////////////////////////////////////////////////////////

  int index = hits * 4 - 1;
  GLuint selectedModelId = selectBuffer[index];

//   wxString strDebug = "";
//   strDebug += " index = ";
//   strDebug += Conversion::LongToString(index).c_str();
//   strDebug += " hits  = ";
//   strDebug += Conversion::LongToString(hits).c_str();
//   strDebug += " selectBuffer = { ";
//   for (int e=0; e < 20; e++)
//   {
//     strDebug += Conversion::LongToString(selectBuffer[e]).c_str();
//     strDebug += ", ";
//   }
//   strDebug += "}";
//   wxMessageDialog messageBox(NULL, strDebug, wxT("DEBUG"), wxOK );
//   messageBox.ShowModal();


  for (GLint j=0; j < this->virtualWorld->GetNumModels(); j++)
  {
    Model * model = this->virtualWorld->GetModels()[j];
    GLuint modelId = model->GetPickId();
    if (selectedModelId == modelId)
    {
      return model;
    }
  }

  return NULL;
}

void wxModelGLCanvas::SetEyePosition(GzVector eyePosition)
{
  this->eyePosition = eyePosition;
}
GzVector wxModelGLCanvas::GetEyePosition()
{
  return this->eyePosition;
}
void wxModelGLCanvas::SetCenterPosition(GzVector centerPosition)
{
  this->centerPosition = centerPosition;
}
GzVector wxModelGLCanvas::GetCenterPosition()
{
  return this->centerPosition;
}
void wxModelGLCanvas::SetUpVector(GzVector upVector)
{
  this->upVector = upVector;
}
GzVector wxModelGLCanvas::GetUpvector()
{
  return this->upVector;
}

bool wxModelGLCanvas::IsOrthographicView()
{
  return this->bOrthographic;
}
void wxModelGLCanvas::EnableOrthographicView(bool enable)
{
  this->bOrthographic = enable;
}

// Metodos necessarios para saber o nome da visao e
// fazer a transformacao de orientacao necessaria
wxString wxModelGLCanvas::GetViewContextName()
{
  return this->viewContextName;
}
void wxModelGLCanvas::SetViewContextName(wxString name)
{
  this->viewContextName = name;  
}

void wxModelGLCanvas::SetVirtualWorld(VirtualWorld * virtualWorld)
{
  this->virtualWorld = virtualWorld;
}

void wxModelGLCanvas::OnPaint(wxPaintEvent &e)
{
  wxPaintDC dc(this);  
  this->Draw();
}

void wxModelGLCanvas::OnMouse(wxMouseEvent& e)
{
  wxSize clientSize = this->GetClientSize();

  if (e.Dragging() && e.LeftIsDown())
  {
    //this->velocityX = 3.0*(double)(e.GetPosition().x - mousePosition.x) / (double)clientSize.GetX();
    //this->velocityY = 3.0*(double)(e.GetPosition().y - mousePosition.y) / (double)clientSize.GetY();
    this->velocityX = -10.0*(double)(e.GetPosition().x - mousePosition.x)/(double)clientSize.GetX();
    this->velocityY = -10.0*(double)(e.GetPosition().y - mousePosition.y)/(double)clientSize.GetY();

    GzVector currentEyePosition    = this->GetEyePosition();
    GzVector currentCenterPosition = this->GetCenterPosition();
    GzVector currentUpVector       = this->GetUpvector();
    
    if (this->arrViewContexts[this->iCurrentViewContext]->name == wxT("X/Y"))
    {
      currentEyePosition.x +=    this->velocityX;
      currentEyePosition.y -=    this->velocityY;
      currentCenterPosition.x += this->velocityX;
      currentCenterPosition.y -= this->velocityY;
    }
    else if (this->arrViewContexts[this->iCurrentViewContext]->name == wxT("X/Z"))
    {
      currentEyePosition.x -=    this->velocityX;
      currentEyePosition.z -=    this->velocityY;
      currentCenterPosition.x -= this->velocityX;
      currentCenterPosition.z -= this->velocityY;
    }
    else if (this->arrViewContexts[this->iCurrentViewContext]->name == wxT("Y/Z"))
    {
      currentEyePosition.y +=    this->velocityX;
      currentEyePosition.z -=    this->velocityY;
      currentCenterPosition.y += this->velocityX;
      currentCenterPosition.z -= this->velocityY;
    }
    else 
    {     
      ViewContext * viewContext = this->arrViewContexts[this->iCurrentViewContext];

      
      //viewContext->camera->GetOrientation().y +=  velocityY * 180.0 / PI_NUMERIC / 10.0;
      //viewContext->camera->GetOrientation().z += -velocityX * 180.0 / PI_NUMERIC / 10.0;
      //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@2 PAREI AQUI !!!!! @@@@@@@@@@@@@@
      //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@22 FAZER ISSO COM CUIDADO !!!! @@@@@@@@@@@@@@@@@@@@
      GzPose pose = viewContext->camera->GetPose();
      GzVector rot = GzQuaternToEuler(pose.rot);

      rot.y += velocityY / 7.0;
      if (rot.y < 0.0) 
      { rot.y += 2 * PI_NUMERIC; } 
      else if (rot.y > 2*PI_NUMERIC) 
      { rot.y -= 2 * PI_NUMERIC;  }

      rot.z += -velocityX / 7.0;
      if (rot.z < 0.0) 
      { rot.z += 2 * PI_NUMERIC; } 
      else if (rot.z > 2*PI_NUMERIC) 
      { rot.z -= 2 * PI_NUMERIC;  }

      pose.rot = GzQuaternFromEuler(rot.x,rot.y,rot.z);

      viewContext->camera->SetPose(pose);

      // Passa as informacoes para o componente de visualizacao 3D
      currentEyePosition    = ModelUtility::GetEyePosition( viewContext->camera );
      currentCenterPosition = ModelUtility::GetCenterPosition( viewContext->camera );
    }
 
    this->SetEyePosition(currentEyePosition);
    this->SetCenterPosition(currentCenterPosition);

  }
  else if (e.RightIsDown() || e.RightDown() || e.RightUp() || e.LeftIsDown()  || e.LeftDown() || e.LeftUp() )  
  {

    // Obtem o Model selecionado no Canvas
    Model * model = NULL;
    //if ( (!e.Dragging()) && (e.RightIsDown() || e.RightDown()) )
    if ( e.RightIsDown() || e.RightDown() )
    {
      model = this->SelectObjects(e.GetPosition().x,e.GetPosition().y);
    }

    if (model == NULL)
    {
        return;
    }

    // Dispara um evento proprio contendo o Model selecionado
    this->glevent = new wxModelSelectedEvent(this->GetId(),wxEVT_COMMAND_RIGHT_CLICK);
    this->glevent->model = model;

    //this->glevent->bMouseDragging = e.Dragging();
    if ( e.LeftDown() )
    {
      this->glevent->mouseMessageType = Mouse_Left_Down;
    }
    else if ( e.LeftUp() )
    {
      this->glevent->mouseMessageType = Mouse_Left_Up;
    }
    else if ( e.LeftIsDown() && e.Dragging() )
    {
      this->glevent->mouseMessageType = Mouse_Left_Dragging;
    }
    else if ( e.RightDown() )
    {
      this->glevent->mouseMessageType = Mouse_Right_Down;
    }
    else if ( e.RightUp() )
    {
      this->glevent->mouseMessageType = Mouse_Right_Up;
    }
    else if ( e.RightIsDown() && e.Dragging() )
    {
      this->glevent->mouseMessageType = Mouse_Right_Dragging;
    }

    //this->velocityX = -10.0*(double)(e.GetPosition().x - mousePosition.x)/(double)clientSize.GetX();
    //this->velocityY = -10.0*(double)(e.GetPosition().y - mousePosition.y)/(double)clientSize.GetY();
    this->glevent->velocityX = (10.0)*(double)(e.GetPosition().x - mousePosition.x) /(double)clientSize.GetX();
    this->glevent->velocityY = -(10.0)*(double)(e.GetPosition().y - mousePosition.y) /(double)clientSize.GetY();
    this->glevent->viewContextName = this->viewContextName;
    this->AddPendingEvent(*this->glevent);
    this->ProcessEvent(*this->glevent);

  } 
  this->mousePosition.x = e.GetPosition().x;
  this->mousePosition.y = e.GetPosition().y;
  this->Draw();
}

ViewContext* wxModelGLCanvas::GetCurrentViewContext()
{
  return this->arrViewContexts[this->iCurrentViewContext];
}
void wxModelGLCanvas::SetCurrentViewContext(int i)
{
  this->iCurrentViewContext = i;
}
void wxModelGLCanvas::SetCurrentViewContext(wxString contextName)
{
  int i = 0;
  for (i=0; i < this->arrViewContexts.size(); i++)
  {
     if (this->arrViewContexts[i]->name.compare(contextName)==0)
     {
       this->iCurrentViewContext = i;
       break;
     }
  }
  if ( i >= this->arrViewContexts.size() )
  {
    throw SystemException("O nome de contexto de visao informado não existe","wxModelGLCanvas::SetCurrentViewContext");
  }
}
ViewContext* wxModelGLCanvas::GetViewContext(int i)
{
  if ((i < 0)||(i > this->arrViewContexts.size()))
  {
    throw new SystemException("indice fora do limite do array de ViewContexts","wxModelGLCanvas::GetViewContext");
  }
  return this->arrViewContexts[i];
}
ViewContext* wxModelGLCanvas::GetViewContext(wxString contextName)
{
   int i=0;
   for (i=0; i < this->arrViewContexts.size(); i++)
   {
      if (this->arrViewContexts[i]->name.compare(contextName) == 0)
      {
        return this->arrViewContexts[i];
      }
   }
   throw SystemException("O nome de contexto de visao informado não existe","wxModelGLCanvas::GetViewContext");
}
void wxModelGLCanvas::RemoveViewContext(ViewContext* viewContext)
{
  // Se o contexto de visao existir este é deletado do array
  vector<ViewContext*>::iterator it = this->arrViewContexts.begin();
  while(it != this->arrViewContexts.end())
  {
    if ((*it)->name.compare(viewContext->name) == 0)
    {
      this->arrViewContexts.erase(it);
      break;
    }    
    it++;
  }
  // Se o contexto nao existir emite um erro de excecao
  if (it == this->arrViewContexts.end())
  {
    throw new SystemException("O contexto de visao informado nao existe no array","wxModelGLCanvas::RemoveViewContext");
  }
}
void wxModelGLCanvas::AddViewContext(ViewContext* viewContext)
{
  //Verifica se existe, erro se existir
  vector<ViewContext*>::iterator it = this->arrViewContexts.begin();
  while(it != this->arrViewContexts.end())
  {
    if ((*it)->name.compare(viewContext->name) == 0)
    {
      throw new SystemException("O contexto de visao informado ja existe no array","wxModelGLCanvas::RemoveViewContext");
    }    
    it++;
  }
  // Adiciona o contexto ao array
  if (it == this->arrViewContexts.end())
  {
    this->arrViewContexts.push_back(viewContext);
  }
}
