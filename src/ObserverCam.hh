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
/* Desc: An Observer camera model 
 * Author: Nate Koenig
 * Date: 20 May 2004
 * CVS: $Id: ObserverCam.hh,v 1.13 2004/11/15 22:19:45 inspectorg Exp $
 */

#ifndef OBSERVERCAM_HH
#define OBSERVERCAM_HH

#include "Model.hh"

////// class Camera; /// Comentado para facilitar reuso deo codigo do modelo
class ObserverWindow;
class HUDElementText;
typedef struct gz_guicam gz_guicam_t;


class ObserverCam : public Model
{
  // Constructor
  public: ObserverCam( World *world);

  // Destructor
  public: virtual ~ObserverCam();

  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );

  // Get camera commands
  private: void GetCameraCmd();

  // Send camera data
  private: void PutCameraData();

  // Time in seconds between updates
  private: double updatePeriod;

  // The time the last update occured
  private: double lastSimTime;
  private: double lastPauseTime;

  // A dummy body (every model must have one)
  private: Body *body;

  // Camera sensor
//////  private: Camera *camera; ///// Comentado para facilitar reuso de codigo do modelo

  // Interface
//////  private: gz_guicam_t *guicam; // Comentado para facilitar reuso de codigo do modelo

  // Spot state (tracking, not tracking)
  private: int cmdSpotState, lastSpotState;

  // Spot mode (translation, zoom, rotation)
  private: int cmdSpotMode;
    
  // Commanded spot transform
  private: GzVector cmdSpotA, cmdSpotB;

  // Initial pose
  private: GzPose initialPose;

  // Lock out roll changes?
  private: bool rollLock;

  // Save frames to disk?
  private: bool saveFrames;

  // Current rendering options
  private: RenderOptions renderOpt;

  public: void Start();
};

#endif


