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
/* Desc: A base class for all sensors
 * Author: Nate Koenig
 * Date: 07 July 2003
 * CVS: $Id: Sensor.hh,v 1.7 2004/10/10 20:36:02 inspectorg Exp $
 */

#ifndef SENSOR_HH
#define SENSOR_HH

// Forward declarations
class World;

/// @brief Base class for sensors
///
/// This doesnt do anything right now apart from making the
/// documentation neat.  It may have some role in the future.
class Sensor
{
  // Constructor
  public: Sensor( World *world );

  // Destructor
  public: virtual ~Sensor();

  // A pointer to the world
  protected: World *world;
};

#endif

