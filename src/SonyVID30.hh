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
/* Desc: A Sony VID30 PTZ camera model
 * Author: Nate Koenig
 * Date: 16 July 2003
 * CVS: $Id: SonyVID30.hh,v 1.24 2004/11/13 00:37:22 inspectorg Exp $
 */

#ifndef SONYVID30_HH
#define SONYVID30_HH

#include "Body.hh"
#include "Model.hh"

// Forward declarations
class Camera;
class FrustrumGeom;
class HingeJoint;


class SonyVID30 : public Model
{
  // Constructor, destructor
  public: SonyVID30( World *world );
  public: virtual ~SonyVID30();
  
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
  private: Geom *bodyGeoms[8];
  private: Geom *lensGeom;
  private: HingeJoint *panJoint;
  private: HingeJoint *tiltJoint;

  // Frustrum for GUI feedback
  private: FrustrumGeom *frustrum;

  // External interfaces
//////////  private: gz_camera_t *cameraIface; ///////// Comentado para evitar erros de compilacao
//////////  private: gz_ptz_t *ptzIface; /////////////// Comentado para evitar erros de compilacao

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
  
  // The camera sensor (does rendering)
  private: Camera *camera;

  // Time between ptz updates
  private: double ptzUpdatePeriod;
  private: double ptzUpdateTime;

  // Time between camera updates
  private: double cameraUpdatePeriod;
  private: double cameraUpdateTime;

  //////////////////////////////////////////////////////////////////
  // Inicia a geometria do modelo e os valores padroes das
  // propriedades
  //////////////////////////////////////////////////////////////////
  public: void Start();
 
};

#endif

