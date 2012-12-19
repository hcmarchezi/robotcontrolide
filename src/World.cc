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
/* Desc: The world; all models are collected here
 * Author: Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: World.cc,v 1.94 2006/02/14 03:00:24 natepak Exp $
 */


/// @addtogroup models 
/// @{ 

/** @defgroup models_global Global Attributes

The @c param:Global tag is used to specify certain global parameters
for the server.  E.g.:

@verbatim
<param:Global>
  <gravity>0.0 0.0 -9.8 </gravity>
  <stepTime>0.020</stepTime>
</param:Global>
@endverbatim

The parameters are as follows:

- speed (float)
  - Target simulation speed (e.g. speed 2 yields twice real time).
  - Default 1.0

- gravity (float vector)
  - The gravity vector (m/sec/sec); the default corresponds to Earth gravity.
  - Default 0 0 -9.8

- stepTime (float)
  - The minimum step time for the simulator.  Reducing the step time
    will increase the fidelity of the physical simulation, but consume
    more CPU time.  If you have particulary complex system that appears to
    be diverging (i.e., objects "explode" when they come into collision), consider
    reducing the step time.
  - Default 0.020

- pause (bool)
  - Set to true to start the simulator in paused mode.
  - Default: false

- dayTime (float)
  - Real time seconds since 12am. Negative number for local time
  - Default: -1.0

- utmZone (integer)
  - UTM (Universal Transverse Mercator) zone for calculating latitudes
    and longitudes from UTM easting and northings.
  - Default 0

- utmOffset (float vector)
  - UTM value (easting and northings) of world origin (used for
    calculating object UTM coordinates).  The z-value can also be used
    to specify an altitude offset.
  - Default 0

- skyColor (float vector) 
  - The color of the "sky", i.e., the default background color.
  - Default 0.7 0.7 0.9

- fogDensity (float)
  - Density of the ambient fog (see OpenGL docs for details).
  - Default 0.25

*/
/// @} 


#if HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_LIBDL
#include <dlfcn.h>
#endif

#include <assert.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <libgen.h>
#include <unistd.h>

#include "gazebo.h"

#include "Error.hh"
#include "WorldFile.hh"
#include "Body.hh"
#include "Model.hh"
#include "ModelFactory.hh"
#include "World.hh"
#include "ContactParams.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
World::World(int serverId, bool serverForce)
{
  //this->worldFile = new WorldFile();    
  this->worldId = dWorldCreate();

  //this->spaceId = dHashSpaceCreate( 0 );
  this->spaceId = dSimpleSpaceCreate( 0 );

  this->contactGroup = dJointGroupCreate( 0 );

  this->modelCount = 0;
  this->modelMaxCount = 0;
  this->models = NULL;

  this->pause = false;
  this->simTime = 0.0;
  this->pauseTime = 0.0;
  this->startTime = 0.0;
  this->dayTime = -1.0;

  this->simSpeed = 1.0;
  this->stepTime = 0.020;
  this->gravity = GzVectorSet(0, 0, -9.8);
  this->utcOffset = 0;
  this->utmZone = GzVectorSet(0, 0, 0);
  this->utmOffset = GzVectorSet(0, 0, 0);
  this->skyColor = GzColor(0.7, 0.7, 0.9, 1.0);
  this->fogEnable = false;
  this->fogDensity = 0.25;

  this->server_id = serverId;
  this->server_force = serverForce;
  this->gz_server = NULL;
  this->gz_sim = NULL;

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
World::~World()
{
  //dJointGroupDestroy( contactGroup );

  dSpaceDestroy( this->spaceId );
  dWorldDestroy( this->worldId );

  this->spaceId = 0;
  this->worldId = 0;

  // TODO: delete array elements

  if (this->models)
    free(this->models);

  //delete this->worldFile;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Initialize the world
int World::Initialize()
{
//////////////////////////////////////////////////////
// COMENTADO PARTE NÀO UTILIZAVEL PARA O IRCE
//////////////////////////////////////////////////////
//   int i;
//   Model *model;
// 
//   assert(this->server_id >= 0);
// 
//   this->startTime = this->GetWallTime();
//   
//   // Create the server object (needs to be done before models initialize)
//   this->gz_server = gz_server_alloc();
//   if (gz_server_init( this->gz_server, this->server_id, this->server_force ) != 0)
//     return -1;
// 
//   // Initialize models
//   for (i = 0; i < this->modelCount; i++)
//   {
//     model = this->models[i];
//     if (model->Init( worldFile, model->node ) != 0)
//       return -1;
//   }
// 
//   // Create the simulator interface
//   this->gz_sim = gz_sim_alloc();
//   if (gz_sim_create( this->gz_sim, this->gz_server, "default" ) != 0)
//     return -1;
// 
//   // Set initial simulator state
//   gz_sim_lock(this->gz_sim, 1);
//   this->gz_sim->data->pause = this->pause;
//   gz_sim_unlock(this->gz_sim);
 
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Finalize the world
int World::Finalize()
{
  int i;
  Model *model;

  // Finalize models
  for (i = 0; i < this->modelCount; i++)
  {
    model = this->models[i];
    if (model->Fini() != 0)
      return -1;
  }

/////////////////////////////////////////////////////////
///// COMENTADO PARA PERMITIR REUSO DO CÓDIGO DO MODELO
/////////////////////////////////////////////////////////
//   // Done with the external interface
//   gz_sim_destroy( this->gz_sim );
//   gz_sim_free( this->gz_sim );
//   gz_server_fini( gz_server );
//   gz_server_free( gz_server );

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load the world
int World::Load( WorldFile *worldFile)
{
  this->worldFile = worldFile;
  
  // Recursively load models
  if (this->LoadModel( this->worldFile->GetRootNode(), NULL ) != 0)
    return -1;

  // Set the gravity
  dWorldSetGravity( this->worldId,
                    this->gravity.x,
                    this->gravity.y, 
                    this->gravity.z );

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Load models (recursive)
int World::LoadModel( WorldFileNode *node, Model *parent )
{
  WorldFileNode *cnode;
  Model *model;
  GzPose pose;
  const char *bodyName;
  const char *pluginName;
  Body *body;

  model = NULL;

  if (node->GetNSPrefix())
  {
    // Check for model nodes
    if (strcmp( node->GetNSPrefix(), "model" ) == 0)
    {
      PRINT_MSG1(2, "Model:%s", node->GetName());
            
      // Load plugin *before* instantiating the model
      pluginName = node->GetString("plugin", NULL, 0);
      if (pluginName)
      {
        if (this->LoadPlugin(pluginName, this->worldFile->filename) != 0)
          return -1;
      }

      // Instantiate the model
      model = ModelFactory::NewModel( this, node->GetName() );
      model->SetName(node->GetName());
      if (model == NULL)
      {
        PRINT_WARN1( "unknown model class or class disabled [%s]", node->GetName() );
        return 0;
      }

      // Add the model to our list
      this->AddModel( model );

      // Recall the node this model is attached to, so we can save
      // back the data later.
      model->node = node;

      // Set the id of the model
      model->SetId( node->GetString( "id", NULL, 0 ) );
      if (model->GetId() == NULL)
      {
        model->SetId( node->GetName() );
      }

      if (parent != NULL)
      {
        // Get the body to attach; if we specify no name, we will
        // always end up with the canonical body.
        bodyName = node->GetString("parentBody", "canonical");
        body = parent->GetBody(bodyName);
        if (body == NULL)
        {
          PRINT_ERR1("body [%s] not defined for parent model", bodyName);
          return -1;
        }

        // Set the parent
        model->SetParent(parent, body);
      }

      // Load the model
      if (model->MasterLoad( this->worldFile, node ) != 0)
        return -1;
      
      // Get the position and orientation of the model (relative to parent)
      pose = GzPoseSet(GzVectorZero(), GzQuaternIdent());
      pose.pos = node->GetPosition( "xyz", pose.pos );
      pose.rot = node->GetRotation( "rpy", pose.rot );
      
      // Set the model's pose (relative to parent)
      this->SetModelPose( model, pose );

      // Record the model's initial pose (for reseting)
      model->initPose = pose;

      // Attach model to its parent
      if (parent != NULL)
        model->Attach();
    }

    // TODO: Need to clean this up. Very ugly with this code here
    else if (strcmp( node->GetNSPrefix(), "param" ) == 0)
    {
      if (strcmp( node->GetName(), "Global" ) == 0)
      {
        this->simSpeed = node->GetDouble("speed", this->simSpeed);        
        this->stepTime = node->GetDouble("stepTime", this->stepTime);
        this->gravity = node->GetPosition( "gravity", this->gravity);

        // Get UTC time offset
        this->utcOffset = node->GetDouble("utcOffset", this->utcOffset);
        
        // Get zone for doing utm to lat/lon conversions (GPS)
        this->utmZone.x = node->GetInt("utmZone", (int) this->utmZone.x);

        // Get UTM position of the world origin
        this->utmOffset = node->GetPosition("utmOffset", this->utmOffset);
        
        this->skyColor = node->GetColor( "skyColor", this->skyColor );
        this->fogEnable = node->IsDefined("fogDensity");
        this->fogDensity = node->GetDouble("fogDensity", this->fogDensity);
        this->dayTime = node->GetDouble("dayTime",this->dayTime);

        // Initial pause state
        this->pause = node->GetBool("pause", this->pause);
      }
    }
  }

  // Load children
  for (cnode = node->GetChild(); cnode != NULL; cnode = cnode->GetNext())
  {
    if (this->LoadModel( cnode, model ) != 0)
      return -1;
  }

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Try to load a given plugin, using a particular search algorithm.
// Returns true on success and false on failure.
// This is copied from Player
int World::LoadPlugin(const char* pluginname, const char *worldfilepath)
{
#if HAVE_LIBDL
  void *handle = NULL;
  char fullpath[PATH_MAX];
  gz_plugin_init_fn_t initfunc;
  char *gazebopath;
  char *tmp;
  char *cfgdir;
  char *error;
  unsigned int i,j;

  // see if we got an absolute path
  if (pluginname[0] == '/' || pluginname[0] == '~')
  {
    strcpy(fullpath,pluginname);
    PRINT_MSG1(1, "loading %s...", fullpath);
    if(!(handle = dlopen(fullpath, RTLD_NOW)))
    {
      PRINT_ERR1("error loading plugin: %s", dlerror());
      return -1;
    }
  }

  // we got a relative path, so search for the module

  // did the user set GAZEBOPATH?
  if(!handle && (gazebopath = getenv("GAZEBOPATH")))
  {
    // yep, now parse it, as a colon-separated list of directories
    i = 0;
    while(i<strlen(gazebopath))
    {
      j=i;
      while(j<strlen(gazebopath))
      {
        if(gazebopath[j] == ':')
        {
          break;
        }
        j++;
      }
      
      memset(fullpath,0,PATH_MAX);
      strncpy(fullpath,gazebopath+i,j-i);
      strcat(fullpath,"/");
      strcat(fullpath,pluginname);

      PRINT_MSG1(1, "loading %s...", fullpath);      
      if((handle = dlopen(fullpath, RTLD_NOW)))
      {
        break;
      }
      else
        PRINT_ERR1("error loading plugin: %s", dlerror());

      i=j+1;
    }
  }
  
  // try to load it from the directory where the config file is
  if (!handle && worldfilepath)
  {
    // Note that dirname() modifies the contents, so
    // we need to make a copy of the filename.
    tmp = strdup(worldfilepath);
    memset(fullpath,0,PATH_MAX);
    cfgdir = dirname(tmp);
    if(cfgdir[0] != '/' && cfgdir[0] != '~')
    {
      getcwd(fullpath, PATH_MAX);
      strcat(fullpath,"/");
    }
    strcat(fullpath,cfgdir);
    strcat(fullpath,"/");
    strcat(fullpath,pluginname);
    free(tmp);

    PRINT_MSG1(1, "loading %s...", fullpath);      
    if(!(handle = dlopen(fullpath, RTLD_NOW)))
      PRINT_ERR1("error loading plugin: %s", dlerror());
  }

  // works in more than one version of autotools.  Fix this later.
  // try to load it from prefix/lib/player/plugins
  if(!handle)
  {
    memset(fullpath,0,PATH_MAX);
    strcpy(fullpath,GAZEBO_PLUGIN_PATH);
    strcat(fullpath,"/");
    strcat(fullpath,pluginname);

    PRINT_MSG1(1, "loading %s...", fullpath);      
    if(!(handle = dlopen(fullpath, RTLD_NOW)))
      PRINT_ERR1("error loading plugin: %s", dlerror());
  }

  // Now invoke the initialization function
  if(handle)
  {
    fflush(stdout);
    initfunc = (gz_plugin_init_fn_t) dlsym(handle, "gazebo_plugin_init");
    if((error = dlerror()) != NULL)
    {
      PRINT_ERR1("plugin init failed [%s]", error);
      return -1;
    }    
    if((*initfunc)() != 0)
    {
      PRINT_ERR("plugin init failed");
      return -1;
    }
    return 0;
  }
  else
    return -1;

  return 0;
  
#else
  PRINT_ERR("Sorry, no support for shared libraries, so can't load plugins");
  return 0;
#endif
}


//////////////////////////////////////////////////////////////////////////////
// Save the world
// This save only changes to pose, and only for top-level objects
int World::Save()
{
  int i;
  Model *model;
  
  for (i = 0; i < this->modelCount; i++)
  {
    model = this->models[i];
    if (model->parent != NULL)
      continue;
    model->initPose = model->GetPose();
    model->node->SetPosition("xyz", model->GetPose().pos);
    model->node->SetRotation("rpy", model->GetPose().rot);
  }

  PRINT_MSG1(1, "\nsaving: %s\n", this->worldFile->filename);
  this->worldFile->Save(NULL);
  
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
// Set the model pose and the pose of its attached children.  
void World::SetModelPose( Model *model, GzPose pose )
{
  int i;
  Model *child;
  GzPose opose, npose, cpose;

  // Get the current global pose of the model
  opose = model->GetPose();

  // Compute the new global pose of the model
  if (model->parent)
  {
    cpose = model->parentBody->GetPose();
    npose = GzCoordPoseAdd( pose, cpose );
  }
  else
  {
    npose = pose;
  }

  // Recursively move children
  for (i = 0; i < this->modelCount; i++)
  {
    child = this->models[i];
    if (child->parent != model)
      continue;

    // Compute current relative pose of child
    cpose = GzCoordPoseSub(child->GetPose(), opose);

    // Compute the new global pose of the child
    cpose = GzCoordPoseAdd( cpose, npose );

    // Compute the new child pose relative to the current model pose
    cpose = GzCoordPoseSub( cpose, opose );

    // Move child 
    this->SetModelPose( child, cpose );
  }

  // Set the new model pose
  model->SetPose( npose );

  // Set our relative pose
  if (model->parentBody && model->joint == NULL)
    model->relPose = GzCoordPoseSub(model->GetPose(), model->parentBody->GetPose());

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Reset the world
void World::ResetModelPoses()
{
  int i;
  Model *model;
  
  // Update models
  for (i = 0; i < this->modelCount; i++)
  {
    model = this->models[i];
    this->SetModelPose(model, model->initPose);
  }

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Single-step the world
void World::Step()
{
//////////////////////////////////////////////////////
// COMENTADO PARTE NÀO UTILIZAVEL PARA O IRCE
//////////////////////////////////////////////////////
//   int i;
//   Model *model;
//   GzPose pose;
//   gz_sim_data_t *data;
//   bool reset, save, pause;
// 
//   data = this->gz_sim->data;
//   
//   // Check for commands from the external interface
//   gz_sim_lock(this->gz_sim, 1);
//   pause = data->pause;
//   reset = data->reset;
//   data->reset = 0;
//   save = data->save;
//   data->save = 0;
//   gz_sim_unlock(this->gz_sim);
// 
//   // Reset poses on request
//   if (reset)
//     this->ResetModelPoses();
// 
//   // Save poses on request
//   if (save)
//     this->Save();
// 
//   // If we are paused, we do something a bit sneaky: we update the
//   // models, but dont advance the simulator time.  This allows models
//   // like the ObserverCam to continue to provide feedback.
//   if (pause)
//   {
//     for (i = 0; i < this->modelCount; i++)
//     {
//       model = this->models[i];
//       model->MasterUpdate( 0.0 );
//     }
// 
//     // Increment the pause time
//     this->pauseTime += this->stepTime;
//   }
// 
//   else
//   {
//     // Update models
//     for (i = 0; i < this->modelCount; i++)
//     {
//       model = this->models[i];
//       model->MasterUpdate( this->stepTime );
//     }
//   
//     // Increment the time
//     this->simTime += this->stepTime;
// 
//     // Do collision detection; this will add contacts to the contact
//     // group
//     dSpaceCollide( this->spaceId, this, CollisionCallback );
//     
//     // Update the dynamical model
//     dWorldStep( this->worldId, this->stepTime );
//     //dWorldStepFast1( this->worldId, step, 10 );
// 
//     // Very important to clear out the contact group
//     dJointGroupEmpty( this->contactGroup );
//   
//     // Move models with dummy bodies, since ODE wont do it
//     for (i = 0; i < this->modelCount; i++)
//     {
//       model = this->models[i];
//       if (model->parentBody != NULL && model->joint == NULL)
//       {
//         pose = GzCoordPoseAdd(model->relPose, model->parentBody->GetPose());
//         model->SetPose(pose);
//       }
//     }
//   }
// 
//   // Update the external interface
//   gz_sim_lock(this->gz_sim, 1);
//   data->sim_time = this->GetSimTime();
//   data->pause_time = this->GetPauseTime();
//   data->real_time = this->GetRealTime();
//   gz_sim_unlock(this->gz_sim);

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Do collision detection
void World::CollisionCallback( void *data, dGeomID o1, dGeomID o2 )
{
  int i,n;
  World *self;
  Geom *geom1, *geom2;
  dContactGeom contactGeoms[10];
  dContact contactInfo;
  dJointID joint;
  int num;

  self = (World*) data;
  
  // Maximum number of contacts
  num = sizeof(contactGeoms) / sizeof(contactGeoms[0]);
    
  // If either geom is a space...
  if (dGeomIsSpace( o1 ) || dGeomIsSpace( o2 ))
  {
    // If the spaces/geoms belong to different spaces, collide them
    if (dGeomGetSpace(o1) != dGeomGetSpace(o2))
      dSpaceCollide2( o1, o2, self, &CollisionCallback );

    // If the spaces belong the world space, collide them
    else if (dGeomGetSpace(o1) == self->spaceId || dGeomGetSpace(o2) == self->spaceId)
      dSpaceCollide2( o1, o2, self, &CollisionCallback );
  }
  else
  {
    // There should be no geoms in the world space
    assert(dGeomGetSpace(o1) != self->spaceId);
    assert(dGeomGetSpace(o2) != self->spaceId);

    // We should never test two geoms in the same space
    assert(dGeomGetSpace(o1) != dGeomGetSpace(o2));

    // Get pointers to the underlying geoms
    geom1 = NULL;
    if (dGeomGetClass(o1) == dGeomTransformClass)
      geom1 = (Geom*) dGeomGetData(dGeomTransformGetGeom(o1));
    else
      geom1 = (Geom*) dGeomGetData(o1);

    geom2 = NULL;
    if (dGeomGetClass(o2) == dGeomTransformClass)
      geom2 = (Geom*) dGeomGetData(dGeomTransformGetGeom(o2));
    else
      geom2 = (Geom*) dGeomGetData(o2);

    assert(geom1 && geom2);
    
    // Detect collisions betweed geoms
    n = dCollide(o1, o2, num, contactGeoms, sizeof(contactGeoms[0]));

    for (i=0; i < n; i++)
    {
      dBodyID body1 = dGeomGetBody(contactGeoms[i].g1);
      dBodyID body2 = dGeomGetBody(contactGeoms[i].g2);

      // Dont add contact joints between already connected bodies.
      // Sometimes the body is unspecified; should probably figure out
      // what this means
      if (body1 && body2 &&
          dAreConnectedExcluding(body1, body2, dJointTypeContact))
          continue;
         
      contactInfo.geom = contactGeoms[i];
      contactInfo.surface.mode = 0;
      
      // Compute the CFM and ERP by assuming the two bodies form a
      // spring-damper system.
      double h, kp, kd;
      h = self->stepTime;
      kp = 1 / (1 / geom1->contact->kp + 1 / geom2->contact->kp);
      kd = geom1->contact->kd + geom2->contact->kd;
      contactInfo.surface.mode |= dContactSoftERP | dContactSoftCFM;
      contactInfo.surface.soft_erp = h * kp / (h * kp + kd);
      contactInfo.surface.soft_cfm = 1 / (h * kp + kd);

      /*
      printf("%f %f %f %f \n",
             kp, kd,
             contactInfo.surface.soft_erp,
             contactInfo.surface.soft_cfm);
      */

      // Compute friction effects; this is standard Coulomb friction
      contactInfo.surface.mode |= dContactApprox1;
      contactInfo.surface.mu = Min(geom1->contact->mu1, geom2->contact->mu1);

      // Compute slipping effects
      //contactInfo.surface.slip1 = (geom1->contact->slip1 + geom2->contact->slip1)/2.0;
      //contactInfo.surface.slip2 = (geom1->contact->slip2 + geom2->contact->slip2)/2.0;
      
      // Construct a contact joint between the two bodies
      joint = dJointCreateContact(self->worldId, self->contactGroup, &contactInfo);
      dJointAttach(joint, body1, body2);
    }
  }

  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the current sim time (elapsed time)
double World::GetSimTime()
{
  return this->simTime;
}


//////////////////////////////////////////////////////////////////////////////
// Get the current pause time (elapsed time)
double World::GetPauseTime()
{
  return this->pauseTime;
}


//////////////////////////////////////////////////////////////////////////////
// Get the current real time (elapsed time)
double World::GetRealTime()
{
  return this->GetWallTime() - this->startTime;
}


//////////////////////////////////////////////////////////////////////////////
// Get the wall clock time
double World::GetWallTime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

//////////////////////////////////////////////////////////////////////////////
// Get the time relative to 12am (seconds)
double World::GetDayTime()
{
  if (this->dayTime >=0)
  {
    return this->dayTime + this->GetRealTime();
  } else {
    double wallTime = this->GetWallTime();
    time_t wallTimeWhole = (time_t)(wallTime);
    wallTime -= wallTimeWhole;

    tm *dayTm = localtime(&wallTimeWhole);

    return dayTm->tm_hour*3600 + dayTm->tm_min*60 + dayTm->tm_sec + wallTime;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Add a model
void World::AddModel( Model *model )
{
  // Re-size array as needed
  if (this->modelCount >= this->modelMaxCount)
  {
    this->modelMaxCount += 10;
    this->models = (Model**) realloc( this->models, this->modelMaxCount * sizeof(this->models[0]));
    assert( this->models );
  }

  this->models[this->modelCount] = model;
  this->modelCount++;
  
  return;
}


//////////////////////////////////////////////////////////////////////////////
// Get the number of models
int World::GetNumModels() const
{
  return this->modelCount;
}


//////////////////////////////////////////////////////////////////////////////
// Get the list of models
Model **World::GetModels() const
{
  return this->models;
}



