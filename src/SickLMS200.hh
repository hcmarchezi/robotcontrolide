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
/* Desc: Model for a Pioneer2AT
 * Author: Andrew Howard
 * Date: 8 May 2003
 * CVS: $Id: SickLMS200.hh,v 1.22 2005/05/04 02:31:31 natepak Exp $
 */

#ifndef SICKLMS200_HH
#define SICKLMS200_HH

#include "Body.hh"
#include "Model.hh"

// Forward declarations
class RayProximity;

/// @brief Simulate SICKLMS200 scanning laser range finder.
class SickLMS200 : public Model
{
  /// @brief Constructor
  public: SickLMS200( World *world );

  /// @brief Destructor
  public: virtual ~SickLMS200();
  
  /// @brief Load the model
  public: virtual int Load( WorldFile *file, WorldFileNode *node );

  /// @brief Initialize the model
  public: virtual int Init( WorldFile *file, WorldFileNode *node );

  /// @brief Finalize the model
  public: virtual int Fini();

  /// @brief Update the model state
  public: virtual void Update( double step );

  /// @brief Load ODE stuff
  private: int OdeLoad( WorldFile *file, WorldFileNode *node );

  /// @brief Load rays
  private: int RayLoad( WorldFile *file, WorldFileNode *node );

  /// @brief Update the data in the laser interface
  private: void PutLaserData();

  /// @brief Update the data in the fiducial interface
  private: void PutFiducialData();

  // Finalize rays
  private: int RayFini();
  
  // Initialize the external interface
  private: int IfaceInit();

  // Finalize the external interface
  private: int IfaceFini();

  // Update the data in the external interface
  private: void IfacePutData();

  // Get command from the interface
  private: bool IFaceGetCmd();
 
  // ODE objects
  private: Body *body;
  
  // Ray sensor
  private: RayProximity *sensor;
  
  // External interfaces
///////////  private: gz_laser_t *laser_iface; //////////// Comentado para evitar erros de compilacao
///////////  private: gz_fiducial_t *fiducial_iface; ////// Comentado para evitar erros de compilacao

  // Laser settings
  private: int rayCount;
  private: int rangeCount;
  private: double laserMinRange, laserMaxRange;
  private: double laserMinAngle, laserMaxAngle;
  private: double laserPeriod;
  private: double laserTime;

  public:  void Start();

};


#endif
