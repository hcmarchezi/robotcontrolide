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
/* Desc: A ball joint
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: BallJoint.hh,v 1.4 2005/08/11 19:42:47 natepak Exp $
 */

#ifndef BALLJOINT_HH
#define BALLJOINT_HH

#include "Joint.hh"

class World;

class BallJoint : public Joint
{
  // Constructor
  public: BallJoint( World *world );

  // Destructor
  public: virtual ~BallJoint();

  // Get joint's anchor point
  public: GzVector GetAnchor() const;

  // Set joint's anchor point
  public: void SetAnchor( GzVector anchor );

};

#endif
