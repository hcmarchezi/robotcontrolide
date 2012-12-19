/*
 *  Gazebo - Outdoor Multi-Robot Simulator
 *  Copyright (C) 2003  
 *     Nate Koenig & Andrew Howard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/* Desc: A Canon VCC4 PTZ camera model
 * Author: Konstantinos Karantzis
 */

#ifndef CANONVCC4_HH
#define CANONVCC4_HH

#include "Body.hh"
#include "Model.hh"

// Forward declarations
////////////////class Camera;///COMENTADO PARA FACILITAR REUSO DE CODIGO
class FrustrumGeom;
class HingeJoint;


class CanonVCC4 : public Model
{
  // Constructor, destructor
  public: CanonVCC4( World *world );
  public: virtual ~CanonVCC4();
  
  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );

  // Load ODE stuff
  private: int OdeLoad( WorldFile *file, WorldFileNode *node );

  // Update the data in the external interface
  private: void PutPtzData();
  private: void PutCameraData();
  private: void GetPtzCmd();

  // ODE objects
  private: Body *body;
  private: Body *middle;
  private: Body *head;
  private: Geom *bodyGeoms[4];
  private: Geom *lensGeom;
  private: HingeJoint *panJoint;
  private: HingeJoint *tiltJoint;

  // Frustrum for GUI feedback
  private: FrustrumGeom *frustrum;

  ///////////// COMENTADAS PARA FACILITAR REUTILIZACAO  ///////////
  ///////////// DE CODIGO /////////////////////////////////////////
  //// External interfaces
  //private: gz_camera_t *cameraIface;
  //private: gz_ptz_t *ptzIface;
  ////////////////////////////////////////////////////////////////////

  // Horizontal field of view
  private: double hfov;
  
  // Zoom factor
  private: double zoomMin, zoomMax;

  // Gain terms for controllers
  private: double motionGain, zoomGain;

  // Commanded PTZ values
  private: double cmdPan, cmdTilt, cmdZoom;

  // Current zoom factor
  private: double zoom;

///////////// COMENTADAS PARA FACILITAR REUTILIZACAO  ///////////
///////////// DE CODIGO /////////////////////////////////////////
//  // The camera sensor (does rendering)
//  private: Camera *camera;
/////////////////////////////////////////////////////////////////

  // Time between ptz updates
  private: double ptzUpdatePeriod;
  private: double ptzUpdateTime;

  // Time between camera updates
  private: double cameraUpdatePeriod;
  private: double cameraUpdateTime;
 

//////////////////////////////////////////////////////////////////
// Metodo para iniciar o modelo com as propriedades no seu
// valor padrao
//////////////////////////////////////////////////////////////////
public: void Start();

};

#endif

