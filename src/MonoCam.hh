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
/* Desc: A very simple generic monocular camera
 * Author: Andrew Howard (re-write from scratch)
 * Date: 15 Nov 2005
 * CVS: $Id: MonoCam.hh,v 1.8 2004/11/16 07:03:12 inspectorg Exp $
 */

#ifndef MONOCAM_HH
#define MONOCAM_HH

#include "Body.hh"
#include "Model.hh"

// Forward declarations
/////// class Camera; /////// Comentado para faciltiar o reuso de código
class FrustrumGeom;


class MonoCam : public Model
{
  // Constructor, destructor
  public: MonoCam( World *world );
  public: virtual ~MonoCam();
  
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
  private: void PutCameraData();

  // Frustrum for GUI feedback
  private: FrustrumGeom *frustrum;

  // External interfaces
///////////  private: gz_camera_t *cameraIface; //////// Comentado para facilitar o reuso de código

  // Horizontal field of view
  private: double hfov;
    
  // The camera sensor (does rendering)
///////////  private: Camera *camera; ////////////////// Comentado para facilitar o reuso de código

  // Time between camera updates
  private: double updatePeriod;
  private: double updateTime;

  // ODE objects
  private: Body *body;
  private: Geom *imager;

//////////////////////////////////////////////////////////////////
// Metodos para iniciar a geometria e as propriedades do modelo
//////////////////////////////////////////////////////////////////
public: void Start();

};

#endif

