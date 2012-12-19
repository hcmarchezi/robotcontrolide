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
/* Desc: Model for the ground plane
 * Author: Nate Koenig
 * Date: 07 July 2003
 * CVS: $Id: GroundPlane.hh,v 1.9 2004/05/26 21:29:48 natepak Exp $
 */

#ifndef GROUNDPLANE_HH
#define GROUNDPLANE_HH

#include "Model.hh"

// Forward Declarations
class PlaneGeom;

class GroundPlane : public Model
{

  // Constructor
  public: GroundPlane( World *world );

  // Destructor
  public: virtual ~GroundPlane();

  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );

  // Bodies, geometry
  private: Body *body;
  private: PlaneGeom *planeGeom;

//////////////////////////////////////////////////////////////////
// Metodo criado para iniciar um objeto com valores padroes
// de suas propriedade 
//////////////////////////////////////////////////////////////////
  public: void Start();

};

#endif

