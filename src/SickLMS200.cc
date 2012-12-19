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
/* Desc: Model for a SickLMS200
 * Author: Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: SickLMS200.cc,v 1.65 2006/02/22 15:29:24 natepak Exp $
 */

/// @addtogroup models
/// @{ 
/** @defgroup SickLMS200 Sick LMS 200

@htmlinclude SickLMS200_view.html

The SickLMS200 model simulates the ubiquitous SICK LMS200 scanning
laser range-finder.

@par libgazebo interfaces

This model supports the @ref laser and @ref fiducial interfaces.

@par Player drivers

Range and intensity data is available through the %gz_laser driver.
Fiducial information (ID, range, bearing and orientation) is available
through the %gz_fiducial driver.  At this time, only the @ref
SimpleSolid model will return a non-zero fiducial value.


@par Attributes

The following attributes are supported.

@htmlinclude default_attr_include.html

- rangeCount (integer)
  - The number of range simulated readings to generate.
  - Default: 181

- rayCount (integer)
  - The number of rays to test.  If rangeCount > rayCount, the
    additional readings will be interpolated.  Reducing the number of
    rays is a good way to save CPU cycles (at the expense of
    simulation fidelity).
  - Default: 91
  
- rangeMax (float, m)
  - Maximum laser range.
  - Default: 8.192
  
- rangeMin (float, m)
  - Minimum laser range.
  - Default: 0.20
  
- scanRate (float, Hz)
  - The scanning rate, or scans-per-second.
  - Default: 10


@par Bodies

The following bodies are created by this model.

@htmlinclude default_body_include.html

@par Example

@verbatim
<model:SickLMS200>
  <id>laser1</id>
  <xyz>0.15 0 0.20</xyz>
  <rangeCount>181</rangeCount>
  <scanRate>10</scanRate>
</model:SickLMS200>
@endverbatim

@par Views

@htmlinclude SickLMS200_more_views.html

@par Authors

Nate Koenig, Andrew Howard

*/
/// @}

  
#include <assert.h>
#include <math.h>

#include "gazebo.h"
#include "World.hh"
#include "WorldFile.hh"
#include "ModelFactory.hh"
#include "RayProximity.hh" //// "RayProximity/RayProximity.hh" /// MODIFICADO PARA FACILITAR O REUSO DO CODIGO
#include "BoxGeom.hh"
#include "CylinderGeom.hh"
#include "SickLMS200.hh"



//////////////////////////////////////////////////////////////////////////////
// Register this model
GZ_REGISTER_STATIC("SickLMS200", SickLMS200);


//////////////////////////////////////////////////////////////////////////////
// Constructor
SickLMS200::SickLMS200( World *world )
    : Model( world )
{
  /////////////////////////////////////////////
  // Adicionando propriedades com meta-dados
  // utilizadas pelo IRCE
  /////////////////////////////////////////////
  this->SetField(new IntegerField("rangeCount",0));
  this->SetField(new IntegerField("rayCount",0));
  this->SetField(new DoubleField("maxRange",0.0));
  this->SetField(new DoubleField("minRange",0.0));
  this->SetField(new DoubleField("scanRate",0.0));
  /////////////////////////////////////////////

  this->body = NULL;
  this->sensor = NULL;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
SickLMS200::~SickLMS200()
{
  if (this->body)
    delete this->body;
  if (this->sensor)
    delete this->sensor;

  this->body = NULL;
  this->sensor = NULL;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Load the model
int SickLMS200::Load( WorldFile *file, WorldFileNode *node )
{
  // Number of simulated  rays
  this->rayCount = node->GetInt("rayCount", 91);

  // Number of range readings
  this->rangeCount = node->GetInt("rangeCount", 181);

  this->laserPeriod = 1.0 / (node->GetDouble("scanRate", 10) + 1e-6);
  this->laserMaxRange = node->GetLength("rangeMax", 8.192);
  this->laserMinRange = node->GetLength("rangeMin", 0.20);
  this->laserMinAngle = node->GetAngle("minAngle", -M_PI * 0.5);
  this->laserMaxAngle = node->GetAngle("maxAngle", +M_PI * 0.5);

  // Create the ODE objects
  if (this->OdeLoad(file, node) != 0)
    return -1;

  // Create ray sensor
  if (this->RayLoad(file, node) != 0)
    return -1;
 
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load ODE objects
int SickLMS200::OdeLoad( WorldFile *file, WorldFileNode *node )
{
  double oz, length, width, height, mass;
  Geom *geom;
    
  length = 0.15;
  width = 0.15;
  height = 0.20;
  mass = 7.0;

  oz = height / 2;

  this->body = new Body( this->world );
  this->AddBody( this->body, true );
  
  geom = new BoxGeom( this->body, this->modelSpaceId, length, width, 0.2 * height );
  geom->SetRelativePosition(GzVectorSet(0, 0, oz + (-0.5 + 0.2 / 2) * height) );
  geom->SetMass( mass / 3 );
  geom->SetColor( GzColor(0, 0, 1) );
  
  geom = new BoxGeom( this->body, this->modelSpaceId, length, width, 0.4 * height );
  geom->SetRelativePosition(GzVectorSet(0, 0, oz + (+0.5 - 0.4 / 2) * height));
  geom->SetMass( mass / 3 );
  geom->SetColor( GzColor(0, 0, 1) );
  
  geom = new BoxGeom( this->body, this->modelSpaceId, length / 2, width, 0.9 * height );
  geom->SetRelativePosition(GzVectorSet( -0.25 * length, 0, oz));
  geom->SetMass( mass / 3 );
  geom->SetColor( GzColor(0, 0, 1) );
  
  geom = new CylinderGeom( this->body, this->modelSpaceId, length / 2, 0.9 * height );
  geom->SetRelativePosition( GzVectorSet(0, 0, oz) );
  geom->SetColor( GzColor(0, 0, 0) );

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load rays
int SickLMS200::RayLoad( WorldFile *file, WorldFileNode *node )
{
  int i;
  double angle;
  GzVector org, axis, pos, qos;

  // Create the sensor
  this->sensor = new RayProximity(this->world, this->body, this->rayCount);
  
  // Origin for laser rays
  org = GzVectorSet(0, 0, 0.07);
  
  // Create the laser rays
  for (i = 0; i < this->rayCount; i++)
  {
    angle = i * (this->laserMaxAngle - this->laserMinAngle) / (this->rayCount - 1) + this->laserMinAngle;
    axis = GzVectorSet(cos(angle), sin(angle), 0);
        
    // Origin of ray
    pos = GzVectorMul(this->laserMinRange, axis);
    pos = GzVectorAdd(pos, org);

    // End point of ray (unit length)
    qos = GzVectorMul(this->laserMaxRange, axis);
    qos = GzVectorAdd(qos, org);

    // Set ray endpoints
    this->sensor->SetRay(i, pos, qos);
  }

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the model
int SickLMS200::Init( WorldFile *file, WorldFileNode *node )
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   // Create laser interface
//   this->laser_iface = gz_laser_alloc();
//   assert(this->laser_iface);
//   if (gz_laser_create(this->laser_iface, this->world->gz_server, this->GetId(), "SickLMS200", this->GetIntId(), this->GetParentIntId()) != 0)
//     return -1;
// 
//   // Create fiducial interface
//   this->fiducial_iface = gz_fiducial_alloc();
//   assert(this->fiducial_iface);
//   if (gz_fiducial_create(this->fiducial_iface, this->world->gz_server, this->GetId(), "SickLMS200", this->GetIntId(), this->GetParentIntId()) != 0)
//     return -1;
// 
//   this->laserTime = this->world->GetSimTime();
  
  return 0;
}



//////////////////////////////////////////////////////////////////////////////
// Finalize the model
int SickLMS200::Fini()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   gz_fiducial_destroy( this->fiducial_iface );
//   gz_fiducial_free( this->fiducial_iface );
//   this->fiducial_iface = NULL;
//     
//   gz_laser_destroy( this->laser_iface );
//   gz_laser_free( this->laser_iface );
//   this->laser_iface = NULL;

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Update the model
void SickLMS200::Update( double step )
{
  // Update the laser periodically
  if (this->world->GetSimTime() - this->laserTime < this->laserPeriod)
    return;
  this->laserTime = this->world->GetSimTime();
  IFaceGetCmd();

  // Update the ray sensor
  this->sensor->Update();
  
  // Update the mmap interface with the new data
  this->PutLaserData();
  this->PutFiducialData();
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Update the data in the interface
void SickLMS200::PutLaserData()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   int i, ja, jb;
//   double ra, rb, r, b;
//   int v;
// 
//   gz_laser_lock(this->laser_iface, 1);
// 
//   // Data timestamp
//   this->laser_iface->data->time = this->world->GetSimTime();
//     
//   // Read out the laser range data
//   this->laser_iface->data->min_angle = this->laserMinAngle;
//   this->laser_iface->data->max_angle = this->laserMaxAngle;
//   this->laser_iface->data->res_angle = (this->laserMaxAngle - this->laserMinAngle) / (this->rangeCount - 1);
//   this->laser_iface->data->max_range = this->laserMaxRange;
//   this->laser_iface->data->range_count = this->rangeCount;
//   
// 
//   assert( this->laser_iface->data->range_count < GZ_LASER_MAX_RANGES );
// 
//   // Interpolate the range readings from the rays
//   for (i = 0; i < this->rangeCount; i++)
//   {
//     b = (double) i * (this->rayCount - 1) / (this->rangeCount - 1);
//     ja = (int) floor(b);
//     jb = Min(ja + 1, this->rayCount - 1);    
//     b = b - floor(b);
// 
//     assert(ja >= 0 && ja < this->rayCount);
//     assert(jb >= 0 && jb < this->rayCount);
// 
//     //ra = Min(this->sensor->GetRange(ja) + this->laserMinRange, this->laserMaxRange);
//     //rb = Min(this->sensor->GetRange(jb) + this->laserMinRange, this->laserMaxRange);
//     ra = Min(this->sensor->GetRange(ja) , this->laserMaxRange);
//     rb = Min(this->sensor->GetRange(jb) , this->laserMaxRange);
// 
//     //printf("%d %d %d %f \n", i, ja, jb, ra);
// 
//     // Range is linear interpolation if values are close,
//     // and min if they are very different
//     if (fabs(ra - rb) < 0.10)
//       r = (1 - b) * ra + b * rb;
//     else
//       r = Min(ra, rb);
// 
//     // Intensity is either-or
//     v = (int) this->sensor->GetRetro(ja) || (int) this->sensor->GetRetro(jb);
// 
//     assert(i < GZ_LASER_MAX_RANGES);
//     this->laser_iface->data->ranges[i] =  r + this->laserMinRange;
//     this->laser_iface->data->intensity[i] = v;
//   }
// 
//   gz_laser_unlock(this->laser_iface);
// 
//   // New data is available
//   gz_laser_post(this->laser_iface);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Update the data in the interface
void SickLMS200::PutFiducialData()
{
/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   int i, j, count;
//   gz_fiducial_fid_t *fid;
//   double r, b;
//   double ax, ay, bx, by, cx, cy;
// 
//   gz_fiducial_lock(this->fiducial_iface, 1);
// 
//   // Data timestamp
//   this->fiducial_iface->data->time = this->world->GetSimTime();
//   this->fiducial_iface->data->fid_count = 0;
// 
//   // TODO: clean this up
//   count = 0;
//   for (i = 0; i < this->rayCount; i++)
//   {
//     if (this->sensor->GetFiducial(i) < 0)
//       continue;
// 
// 
//     // Find the end of the fiducial
//     for (j = i + 1; j < this->rayCount; j++)
//     {
//       if (this->sensor->GetFiducial(j) != this->sensor->GetFiducial(i))
//         break;
//     }
//     j--;
// 
//     // Need at least three points to get orientation
//     if (j - i + 1 >= 3)
//     {
//       r = this->laserMinRange + this->sensor->GetRange(i);
//       b = this->laserMinAngle + i * ((this->laserMaxAngle-this->laserMinAngle) / (this->rayCount - 1));
//       ax = r * cos(b);
//       ay = r * sin(b);
// 
//       r = this->laserMinRange + this->sensor->GetRange(j);
//       b = this->laserMinAngle + j * ((this->laserMaxAngle-this->laserMinAngle) / (this->rayCount - 1));
//       bx = r * cos(b);
//       by = r * sin(b);
// 
//       cx = (ax + bx) / 2;
//       cy = (ay + by) / 2;
//       
//       assert(count < GZ_FIDUCIAL_MAX_FIDS);
//       fid = this->fiducial_iface->data->fids + count++;
// 
//       fid->id = this->sensor->GetFiducial(j);
//       fid->pos[0] = cx;
//       fid->pos[1] = cy;
//       fid->rot[2] = atan2(by - ay, bx - ax) + M_PI / 2;
//     }
// 
//     // Fewer points get no orientation
//     else
//     {
//       r = this->laserMinRange + this->sensor->GetRange(i);
//       b = this->laserMinAngle + i * ((this->laserMaxAngle-this->laserMinAngle) / (this->rayCount - 1));
//       ax = r * cos(b);
//       ay = r * sin(b);
// 
//       r = this->laserMinRange + this->sensor->GetRange(j);
//       b = this->laserMinAngle + j * ((this->laserMaxAngle-this->laserMinAngle) / (this->rayCount - 1));
//       bx = r * cos(b);
//       by = r * sin(b);
// 
//       cx = (ax + bx) / 2;
//       cy = (ay + by) / 2;
//       
//       assert(count < GZ_FIDUCIAL_MAX_FIDS);
//       fid = this->fiducial_iface->data->fids + count++;
// 
//       fid->id = this->sensor->GetFiducial(j);
//       fid->pos[0] = cx;
//       fid->pos[1] = cy;
//       fid->rot[2] = atan2(cy, cx) + M_PI;
//     }
// 
//     
//     /*printf("fiducial %d i[%d] j[%d] %.2f %.2f %.2f\n",
//            fid->id, i,j,fid->pos[0], fid->pos[1], fid->rot[2]);
//            */
//     i = j;
//   }
// 
//   this->fiducial_iface->data->fid_count = count;
//   
//   gz_fiducial_unlock(this->fiducial_iface);
//   gz_fiducial_post(this->fiducial_iface);
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get command from the interface
bool SickLMS200::IFaceGetCmd()
{
  bool cmd_new;

/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////  
//   gz_laser_lock(this->laser_iface,1);
// 
//   if (this->laser_iface->data->cmd_new_angle || this->laser_iface->data->cmd_new_length)
//     {
//       cmd_new = true;
//       if (this->laser_iface->data->cmd_new_angle == 1)
// 	this->laser_iface->data->cmd_new_angle = 0;
//       else if (this->laser_iface->data->cmd_new_length == 1)
// 	this->laser_iface->data->cmd_new_length = 0;
// 
//       this->laserMaxRange = this->laser_iface->data->cmd_max_range;
//       this->laserMinAngle = this->laser_iface->data->cmd_min_angle;
//       this->laserMaxAngle = this->laser_iface->data->cmd_max_angle;
//       this->rangeCount = this->laser_iface->data->cmd_range_count;
//     }
//   else
//     cmd_new = false;
// 
//   gz_laser_unlock(this->laser_iface);

  return cmd_new;
}

/////////////////////////////////////////////////////////////////////
// Inicia o modelo com as propriedades com valor padrao e
// iniciar a geometria do mesmo
/////////////////////////////////////////////////////////////////////
void SickLMS200::Start()
{
 // Number of simulated  rays
  this->rayCount = 91;

  // Number of range readings
  this->rangeCount = 181;

  this->laserPeriod = 1.0 / (10 + 1e-6);
  this->laserMaxRange = 8.192;
  this->laserMinRange = 0.20;
  this->laserMinAngle = -M_PI * 0.5;
  this->laserMaxAngle = +M_PI * 0.5;

  double oz, length, width, height, mass;
  Geom *geom;
    
  length = 0.15;
  width = 0.15;
  height = 0.20;
  mass = 7.0;

  oz = height / 2;

  this->body = new Body( this->world );
  this->AddBody( this->body, true );
  
  geom = new BoxGeom( this->body, this->modelSpaceId, length, width, 0.2 * height );
  geom->SetRelativePosition(GzVectorSet(0, 0, oz + (-0.5 + 0.2 / 2) * height) );
  geom->SetMass( mass / 3 );
  geom->SetColor( GzColor(0, 0, 1) );
  
  geom = new BoxGeom( this->body, this->modelSpaceId, length, width, 0.4 * height );
  geom->SetRelativePosition(GzVectorSet(0, 0, oz + (+0.5 - 0.4 / 2) * height));
  geom->SetMass( mass / 3 );
  geom->SetColor( GzColor(0, 0, 1) );
  
  geom = new BoxGeom( this->body, this->modelSpaceId, length / 2, width, 0.9 * height );
  geom->SetRelativePosition(GzVectorSet( -0.25 * length, 0, oz));
  geom->SetMass( mass / 3 );
  geom->SetColor( GzColor(0, 0, 1) );
  
  geom = new CylinderGeom( this->body, this->modelSpaceId, length / 2, 0.9 * height );
  geom->SetRelativePosition( GzVectorSet(0, 0, oz) );
  geom->SetColor( GzColor(0, 0, 0) );
}

