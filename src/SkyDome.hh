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
/* Desc: Model for the skydome
 * Author: Nate Koenig
 * Date: 24 Nov. 2004
 * CVS: $Id: SkyDome.hh,v 1.2 2004/11/30 00:57:47 natepak Exp $
 */

#ifndef SKYDOME_HH
#define SKYDOME_HH

#include "Model.hh"

// Forward Declarations
class DomeGeom;

class SkyDome : public Model
{

  // Constructor
  public: SkyDome( World *world );

  // Destructor
  public: virtual ~SkyDome();

  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );

  // Take into account time lapse?
  private: bool timeLapse; 

  // Bodies, geometry
  private: Body *body;
  private: DomeGeom *domeGeom;

  //////////////////////////////////////////////////////////////////
  // Metodo que inicia a geometria e as propriedades do modelo
  //////////////////////////////////////////////////////////////////
  public: void Start();


};

#endif

