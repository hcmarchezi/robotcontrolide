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
 * Author: Nate Koenig, Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: World.hh,v 1.48 2006/02/14 03:00:25 natepak Exp $
 */

#ifndef WORLD_HH
#define WORLD_HH

#include "Geom.hh"
#include "Model.hh"
#include "Sensor.hh"

// Forward declarations
class WorldFile;
class WorldFileNode;

typedef struct gz_server gz_server_t;
typedef struct gz_sim gz_sim_t;


/// @brief The one-and-only world; all models go here.
///
/// The world class keeps a list of all models, handles loading and
/// saving, object dynamics and collision detection for contact
/// joints.
class World
{
  // Constructor
  public: World(int serverId, bool serverForce);

  // Destructor
  public: virtual ~World();

  /// @brief Load the world from a file
  /// @param worldFile Pointer to the world file object; the file
  /// pointer is stored must not be deleted while the world is running.
  public: int Load( WorldFile *worldFile );

  // Load models (recursive)
  public: int LoadModel( WorldFileNode *node, Model *parent );

  // Try to load a given plugin, using a particular search algorithm.
  private: int LoadPlugin(const char* pluginname, const char *worldfilepath);
  
  /// @brief Set the pose of a model (and its attached children).
  /// @param model model to shift.
  /// @param pose New model pose, relative to parent
  public: void SetModelPose( Model *model, GzPose pose );

  /// @brief Reset the world
  ///
  /// Teleport the models to their initial positions, as specified in
  /// the world file.  This does not change the similator clock, or
  /// any other state variables.
  public: void ResetModelPoses();
  
  /// @brief Save the world
  ///
  /// This is used to mainly save changes to model poses.
  public: int Save();

  // Initialize the world
  public: int Initialize();

  // Finalize the world
  public: int Finalize();

  // Move the world forward in time
  public: void Step();

  // Do collision detection
  private: static void CollisionCallback( void *data, dGeomID o1, dGeomID o2 );
  
  /// @brief Get the elapsed simulator time (seconds).
  public: double GetSimTime();

  /// @brief Get accumulated pause time (seconds).
  ///
  /// Note that this is the paused @e simulation time; i.e., simTime +
  /// pauseTime = simSpeed * realTime (assuming a fast processor).
  public: double GetPauseTime();

  /// @brief Get the elapsed real time (elapsed time).
  public: double GetRealTime();

  /// @brief Get the wall clock time (seconds).
  public: double GetWallTime();

  /// @brief Get the time relative to 12am (seconds)
  public: double GetDayTime();

  /// @brief Get the number of models
  public: int GetNumModels() const;

  /// @brief Get the list of models
  public: Model **GetModels() const;

  /// @brief Add a model to the internal list
  protected: void AddModel( Model *model );

  // Get the world space id
  public: dSpaceID GetSpaceId() {return this->spaceId;}

  // World file
  private: WorldFile *worldFile;
  
  // Server id
  public: int server_id;
  private: bool server_force;
  
  // Simulator control interface
  public: gz_server_t *gz_server;
  private: gz_sim_t *gz_sim;

  // Top-level world for all bodies
  public: dWorldID worldId;

  // Top-level space for all sub-spaces/geoms
  private: dSpaceID spaceId;

  // List of dynamic models in the world
  protected: int modelCount, modelMaxCount;
  //
  // TIPO DE ACESSO FOI ALTERADO PARA PROTECTED PARA
  // PERMITIR QUE A CLASSE VIRTUAL WORLD POSSA ACESSAR
  // ESSE CAMPO E REMOVER O MODELO DA LISTA
  //
  protected: Model **models;

  // Collision attributes
  private: dJointGroupID contactGroup;

  /// Simulation speed (e.g. speed 2 yields twice real time).
  public: double simSpeed;
  
  /// Simulation step time
  private: double stepTime;

  /// Flag set if simulation is paused
  private: bool pause;

  /// Current simulation times
  private: double simTime, pauseTime, startTime;
  
  // The gravity vector
  private: GzVector gravity;

  // The time offset
  private: double dayTime;

  /// UTC time offset (seconds since epoch).  Used for GPS times.
  public: double utcOffset;
  
  /// UTM zone.  This is an integer offset; e.g., UTM zone 11 is
  /// converted to x = +11.
  public: GzVector utmZone;
  
  /// UTM offset (UTM coordinate that maps to zero in global cs).
  public: GzVector utmOffset;

  // The color of the sky (default color)
  public: GzColor skyColor;

  // Fog settings
  public: bool fogEnable;
  public: double fogDensity;
  public: GzColor fogColor;


  // Warts
  friend class Model;
};

#endif

