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
 * CVS: $Id: GarminGPS.cc,v 1.23 2006/02/22 14:58:57 natepak Exp $
 */

/// @addtogroup models 
/// @{ 
/** @defgroup GarminGPS Garmin GPS


@htmlinclude GarminGPS_view.html

The GarminGPS model simulates GPS information by using the
flat-world approximation.  It doesn't account for GPS occlusion or
loss of signal, etc.

When using the GPS modle, one must also define a UTM offset and zone
in the @ref models_global "global parameters" section of the world
file.  The UTM offset governs the transformation between simulation
coordinates and UTM coordinates; i.e., the point (0, 0, 0) in the
simulation frame is mapped to point (utmOffset.x, utmOffset.y,
utmOffset.z) in the UTM frame.  The UTM zone is used to convert UTM
coordinates to latitude and longitude.


@par libgazebo interfaces

This model supports the @ref gps interface.

@par Player drivers

GPS position information is available through the %gz_gps driver.


@par Attributes

The following attributes are supported.

@htmlinclude default_attr_include.html


@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:GarminGPS>
  <id>gps1</id>
</model:GarminGPS>
@endverbatim

@todo Horizontal and vertical errors (Garmin specific).
@todo Model satellite geometry for DOP.
@todo Compute satellite occlusions.

@par Views

@htmlinclude GarminGPS_more_views.html

@par Authors

Pranav Srivastava

*/
/// @}

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <assert.h>

#include "gazebo.h"
#include "Error.hh"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "Body.hh"
#include "CylinderGeom.hh"

#include "GarminGPS.hh"


//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("GarminGPS", GarminGPS);


//////////////////////////////////////////////////////////////////////////////
// Constructor
GarminGPS::GarminGPS( World *world )
    : Model( world )
{ 
  this->body = NULL;
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
GarminGPS::~GarminGPS()
{
  if (this->body)
    delete this->body;

  this->body = NULL;

#if HAVE_PROJ4
  pj_free(this->proj);
#endif
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Load the model
int GarminGPS::Load( WorldFile *file, WorldFileNode *node )
{
  Geom *geom;
  double diam, height;

  // Dimensions
  diam = node->GetTupleLength("size", 0, 0.08);
  height = node->GetTupleLength("size", 1, 0.05);
  
  // Create a body
  this->body = new Body( this->world );
  this->AddBody( this->body, true );

  // Create a basic geom
  geom = new CylinderGeom(this->body, this->modelSpaceId, diam / 2, height);
  geom->SetRelativePosition(GzVectorSet(0, 0, height / 2));
  geom->SetMass(node->GetDouble("mass", 1, 0));
  geom->SetColor(node->GetColor("color", GzColor(1, 1, 1)));

  // Load time settings
  this->updateTime = 0.0;
  this->updatePeriod = 1.0 / node->GetTime("updateFreq", 1.0);

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int GarminGPS::Init( WorldFile *file, WorldFileNode *node )
{
///////////////////////////////////////////////////////////
// COMENTADO PARA FACILITAR O REUSO
///////////////////////////////////////////////////////////
//   // Create GPS interface
//   this->gps = gz_gps_alloc();
//   assert(this->gps);
//   if (gz_gps_create(this->gps, this->world->gz_server, this->GetId(),
//                     "GarminGPS", this->GetIntId(), this->GetParentIntId()) != 0)
//     return -1;  
//   
// #if HAVE_PROJ4
//   // Initialize the projection
//   int lat, lon;
//   char lonZone[128];
//   char *params[4];
// 
//   lon = (int) ((this->world->utmZone.x * 6) - 180);
//   lat = (int) (this->world->utmZone.y * 8);
//   
//   if (lon > 0)
//     snprintf(lonZone, sizeof(lonZone), "lon_0=%de", +lon);
//   else
//     snprintf(lonZone, sizeof(lonZone), "lon_0=%dw", -lon);
// 
//   params[0] = "proj=utm";
//   params[1] = "ellps=WGS84";
//   params[2] = "lat_0=0";
//   params[3] = lonZone;
// 
//   PRINT_MSG4(3, "proj4 projection %s %s %s %s",
//              params[0], params[1], params[2], params[3]);
//   
//   this->proj = pj_init(4, params);
//   if (this->proj == NULL)
//   {
//     PRINT_ERR("unable to initialize cartographic projection");
//     return -1;
//   }
// 
//   // Convert from lat/lon to utm to get the utm offset
//   projUV ll;
//   ll.u = this->world->utmOffset.x;
//   ll.v = this->world->utmOffset.y;
//   ll = pj_inv(ll, this->proj);
//   ll.v = RTOD(ll.v);
//   ll.u = RTOD(ll.u);
// 
//   PRINT_MSG4(3, "using UTM origin %.0f %.0f = lon/lat %.6f %.6f",
//              this->world->utmOffset.x, this->world->utmOffset.y,
//              ll.u, ll.v);
// 
// #else
//   PRINT_WARN("proj4 disabled at compile time; lat/lon will not be computed");
// #endif
  
  return 0;
}




//////////////////////////////////////////////////////////////////////////////
// Finalize the model
int GarminGPS::Fini()
{
///////////////////////////////////////////////////////////
// COMENTADO PARA FACILITAR O REUSO
///////////////////////////////////////////////////////////
//   // Finalize GPS interface
//   gz_gps_destroy( this->gps );
//   gz_gps_free( this->gps );
//   this->gps = NULL;

  return 0;
}




//////////////////////////////////////////////////////////////////////////////
// Update model
void GarminGPS::Update( double /*step*/ )
{
  GzVector pos;

  if (this->world->GetSimTime() - this->updateTime > this->updatePeriod)
  {
    this->updateTime = this->world->GetSimTime();

    // Compute UTC time
    this->utc = this->world->utcOffset + this->world->GetSimTime();
    
    // GPS position in global cs
    pos = this->GetPose().pos;

    // Compute GPS position in UTM coords
    pos = GzVectorAdd(pos, this->world->utmOffset);
    this->utm = pos;
    this->lon = 0;
    this->lat = 0;
    this->alt = pos.z;
    
#if HAVE_PROJ4    
    // Convert from UTM to lat/lon
    projUV ll;
    ll.u = pos.x;
    ll.v = pos.y;
    ll = pj_inv(ll, this->proj);
    this->lat = RTOD(ll.v);
    this->lon = RTOD(ll.u);
#endif
   
    // Dummy up the remaining values
    this->sats = 10;
    this->qual = 2;
    this->hdop = 1.00;
    this->vdop = 1.00;

    // Update the outgoing data
    this->GpsPutData();
  }

  return;
}


// Update the data in the interface
void GarminGPS::GpsPutData()
{
//   gz_gps_data_t *data;
// 
//   gz_gps_lock(this->gps, 1);
//   
//   data = this->gps->data;
//   
//   // Data timestamp
//   this->gps->data->time = this->world->GetSimTime();
// 
//   this->gps->data->utc_time = this->utc;
//   this->gps->data->latitude =  this->lat;
//   this->gps->data->longitude = this->lon;
//   this->gps->data->altitude =  this->alt;
//   this->gps->data->utm_e = this->utm.x;
//   this->gps->data->utm_n = this->utm.y;
//   this->gps->data->satellites =  this->sats;
//   this->gps->data->quality =  this->qual;
//   this->gps->data->hdop =  this->hdop;
//   this->gps->data->vdop =  this->vdop;
//   this->gps->data->err_horz = 0;
//   this->gps->data->err_vert = 0;
// 
//   gz_gps_unlock(this->gps);
//   gz_gps_post(this->gps);
  
  return;
}

void GarminGPS::Start()
{
  Geom *geom;
  double diam, height;

  // Dimensions
  diam = 0.08;
  height = 0.05;
  
  // Create a body
  this->body = new Body( this->world );
  this->AddBody( this->body, true );

  // Create a basic geom
  geom = new CylinderGeom(this->body, this->modelSpaceId, diam / 2, height);
  geom->SetRelativePosition(GzVectorSet(0, 0, height / 2));
  geom->SetMass(1);
  geom->SetColor(GzColor(1, 1, 1));

  // Load time settings
  this->updateTime = 0.0;
  this->updatePeriod = 1.0 / 1.0;
}

