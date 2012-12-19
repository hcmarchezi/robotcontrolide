
#ifndef WX3DMODELPANEL_H
#define WX3DMODELPANEL_H

#include <wx/wx.h>
#include "wxModelGLCanvas.h"
#include "IRCEProject.h"

class wx3DModelPanel : public wxPanel
{
// Widgets que compoem o painel
public: wxModelGLCanvas * worldGLCanvas;
public: wxStaticText  * lbViewType;
public: wxComboBox    * cmbbxViewType;
public: wxRadioBox    * radbRenderType;

// Variaveis necessarias para calcular as coordenadas esfericas
// arrastando a posicao do mouse pelo botao esquerdo
//private: GLdouble velocityX;
//private: GLdouble velocityY;
//private: GLdouble angleX;
//private: GLdouble angleY;
//private: GLdouble zoom;
//private: wxPoint  mousePosition;
// Vetor que guarda as informacoes de posicionamento e orientacao de cada visao 3D
//private: vector<ViewContext> arrViewContexts;
//private: int iCurrentViewContext;
// Referencia para o evento gerado pelo wxWorldGLCanvas
//private: wxModelSelectedEvent glEvent;

public: wx3DModelPanel(wxWindow* parent, wxWindowID id = -1,IRCEProject* project = NULL);
public: virtual ~wx3DModelPanel(void);

// Metodos para manipular os tipos de visao
public: void AddViewType(ViewContext context);
public: void RemoveViewType(wxString name);
public: ViewContext GetViewTypeContext(wxString name);

// Ao passar o mouse ( com botoes pressionados ou nao ) sobre o GL Canvas
private: void OnGLCanvasMouse(wxMouseEvent& e);
// Ao selecionar um tipo de renderizacao
private: void OnRenderTypeRadioBoxSelected(wxCommandEvent& e);
// Ao selecionar um tipo de visao 3D
private: void OnViewTypeComboboxSelected(wxCommandEvent& e);

};

#endif

