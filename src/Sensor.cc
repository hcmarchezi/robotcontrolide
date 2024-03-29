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
/* Desc: Base class for all sensors.
 * Author: Nate Koenig
 * Date: 07 July 2003
 * CVS: $Id: Sensor.cc,v 1.6 2004/10/10 20:36:02 inspectorg Exp $
 */

#include <assert.h>

#include "World.hh"
#include "Sensor.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
Sensor::Sensor( World *world )
    : world( world )
{
}

//////////////////////////////////////////////////////////////////////////////
// Destructor
Sensor::~Sensor()
{
}
