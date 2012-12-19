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
/* Desc: A Very Basic Model for a Garmin GPS
 * Author: Pranav Srivastava
 * Date: 22nd October
 * CVS: $Id: GarminGPS.hh,v 1.8 2004/11/14 07:39:04 inspectorg Exp $
 */

#ifndef GARMINGPS_HH
#define GARMINGPS_HH

#if HAVE_PROJ4
#include <projects.h>
#endif

#include "Model.hh"

// Forward declarations
typedef struct gz_gps gz_gps_t;


class GarminGPS : public Model
{
  // Construct, destructor
  public: GarminGPS( World *world );
  public: virtual ~GarminGPS();

  // Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  // Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  // Finalize the model
  public: virtual int Fini();

  // Update the model state
  public: virtual void Update( double step );
 
  // Update the data in the external interface
  private: void GpsPutData();
      
  // ODE components
  private: Body *body;

  // Time settings
  private: double updatePeriod, updateTime;

  // Simulated GPS data
  private: GzVector utm;
  private: double utc, lat, lon, alt;
  private: int sats, qual;
  private: double hdop, vdop;

#if HAVE_PROJ4
  // Projection
  private: PJ *proj;
#endif

  // External interface
  private: gz_gps_t *gps;

  /////////////////////////////////////////////////
  // Inicia o modelo com o valor padrao das
  // propriedades
  /////////////////////////////////////////////////
  public: void Start();
};

#endif
