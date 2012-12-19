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
/* Desc: Model that emitts light
 * Author: Nathan Koenig
 * Date: Jan 2004
 * CVS: $Id: LightSource.hh,v 1.5 2004/06/01 17:12:49 natepak Exp $
 */

#ifndef LIGHTSOURCE_HH
#define LIGHTSOURCE_HH

class LightGeom;

#include "Model.hh"


class LightSource : public Model
{
  // Constructor, destructor
  public: LightSource( World *world );
  public: virtual ~LightSource();
  
  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );


  public: void On();
  public: void Off();
  public: void Toggle();
  public: bool IsOn() const;
  
  // Is the light on?
  private: bool lightOn;

  // Canonical body
  private: Body *body;
  
  // The light geom
  private: LightGeom *lightGeom;

  public: void Start();

};


#endif
