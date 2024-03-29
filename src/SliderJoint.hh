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
/* Desc: A slider or primastic joint
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: SliderJoint.hh,v 1.5 2005/12/02 16:26:51 natepak Exp $
 */

#ifndef SLIDERJOINT_HH
#define SLIDERJOINT_HH

#include "Joint.hh"

class JointGroup;

class SliderJoint : public Joint
{
  // Constructor
  public: SliderJoint( dWorldID worldId, JointGroup *group=NULL );

  // Destructor
  public: virtual ~SliderJoint();

  // Get the axis of rotation
  public: void GetAxis( dVector3 result ) const;

  // Get the position of the joint
  public: double GetPosition() const;

  // Get the rate of change
  public: double GetPositionRate() const;

  // Get the _parameter
  public: virtual double GetParam( int parameter ) const;

  // Set the axis of motion
  public: void SetAxis( double x, double y, double z );

  // Set the _parameter
  public: virtual void SetParam( int parameter, double value);

  public: virtual void SetAnchor( GzVector anchor) {}

  public: void SetSliderForce(double force);
};

#endif
