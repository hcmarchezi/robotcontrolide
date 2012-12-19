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

#ifndef WORLDGLCANVAS_H
#define WORLDGLCANVAS_H

/////////////////////////////////////////////////////////////////
////////////////////////////// Nao troque esses includes de lugar 
#include <wx/wx.h>
#include <wx/glcanvas.h>
//////////////////////////////////////////////////////////////////

#include "Model.hh"
#include "wxModelSelectedEvent.h"

#include <vector>
/////////////////////////////#include "Camera.h"

#include "RenderOptions.hh"


using namespace std;

// Guarda a ultima alteracao em uma visao (X/Y, X/Z, ObserverCam, etc ...)
// para que esta possa ser recuperada mais tarde ao ser selecionada
// novamente na janela principal
//typedef 
//struct 
//{
//  bool     orthographic; // true for orthographic view and false for perspective view
//  GzVector eyePosition;
//  GzVector centerPosition;
//  GzVector upVector;
//  wxString name;  
//} ViewContext;

class ViewContext
{
public: bool orthographic;
public: GzVector eyePosition;
public: GzVector centerPosition;
public: GzVector upVector;
public: wxString name;
//public: Camera* camera;
public: Model * camera;
public: ViewContext() { camera = NULL; }
};

// Define um tipo para o ponteiro para um callback com o evento model select event
typedef void (wxEvtHandler::*wxModelSelectedEventFunction)(wxModelSelectedEvent&);

/**
@author Humberto Cardoso Marchezi
*/
class wxModelGLCanvas : public wxGLCanvas
{
protected:
  bool     initGL;
  wxPoint  mousePosition;
  GLdouble velocityX;
  GLdouble velocityY;

//private: GLdouble angleX;
//private: GLdouble angleY;
//private: GLdouble zoom;
//private: wxPoint  mousePosition;

  GzVector eyePosition;
  GzVector centerPosition;
  GzVector upVector;
  bool     bOrthographic; // Visao Ortografica ou em perspectiva
  wxString viewContextName; // Nome da camera

  double   current[4][4];          // Armazena uma matriz 4x4 do OpenGL
  GzVector orthoOrientation; // Vetor que descreve a orientacao para visualizacao ortografica

  VirtualWorld * virtualWorld;

  // Vetor que guarda as informacoes de posicionamento e orientacao de cada visao 3D
  vector<ViewContext *> arrViewContexts;
  int iCurrentViewContext;

public:
  wxModelGLCanvas(	    
    wxWindow *parent, 
	  const wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,long style = 0, 
    const wxString& name = wxT("GLCanvas"));
  ~wxModelGLCanvas();

private:
  wxModelSelectedEvent * glevent;
  // Inicia o widget com as configuracoes de OpenGL
  void InitGL();
  // faz a renderiazacao em modo RENDER visual da cena
  void DrawScene();
  // contem os comandos de descricao da cena usados nos metodos acima
  void Render();
  // Atualiza a visualizacao do widget
  void OnPaint(wxPaintEvent& e);
  // Ao arrastar o mouse
  void OnMouse(wxMouseEvent& e);

public:
  // faz a renderizacao em modo SELECTION da cena
  Model * SelectObjects(int xPos,int yPos);
  void Draw();

  void SetEyePosition(GzVector eyePosition);
  GzVector GetEyePosition();

  void SetCenterPosition(GzVector centerPosition);
  GzVector GetCenterPosition();

  void SetUpVector(GzVector upVector);
  GzVector GetUpvector();

  bool IsOrthographicView();
  void EnableOrthographicView(bool enable);

  wxString GetViewContextName();
  void SetViewContextName(wxString name);
  
  void SetVirtualWorld(VirtualWorld * virtualWorld);

  ViewContext* GetCurrentViewContext();
  void SetCurrentViewContext(int i);
  void SetCurrentViewContext(wxString contextName);
  ViewContext* GetViewContext(int i);
  ViewContext* GetViewContext(wxString contextName);
  void RemoveViewContext(ViewContext* viewContext);
  void AddViewContext(ViewContext* viewContext);
};

#endif

