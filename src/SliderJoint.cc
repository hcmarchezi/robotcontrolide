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
 * CVS: $Id: SliderJoint.cc,v 1.4 2005/12/02 16:26:51 natepak Exp $
 */

#include "SliderJoint.hh"
#include "JointGroup.hh"


//////////////////////////////////////////////////////////////////////////////
// Constructor
SliderJoint::SliderJoint( dWorldID worldId, JointGroup *group )
  : Joint()
{
  if (group)
    this->jointId = dJointCreateSlider( worldId, group->GetGroupId() );
  else
    this->jointId = dJointCreateSlider( worldId, NULL );
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
SliderJoint::~SliderJoint()
{
}


//////////////////////////////////////////////////////////////////////////////
// Get the axis of rotation
void SliderJoint::GetAxis( dVector3 result ) const
{
  dJointGetSliderAxis( this->jointId, result );
}


//////////////////////////////////////////////////////////////////////////////
// Get the position of the joint
double SliderJoint::GetPosition() const
{
  return dJointGetSliderPosition( this->jointId );
}


//////////////////////////////////////////////////////////////////////////////
// Get the rate of change
double SliderJoint::GetPositionRate() const
{
  return dJointGetSliderPositionRate( this->jointId );
}


//////////////////////////////////////////////////////////////////////////////
// Get the _parameter
double SliderJoint::GetParam( int parameter ) const
{
  return dJointGetSliderParam( this->jointId, parameter );
}


//////////////////////////////////////////////////////////////////////////////
// Set the axis of motion
void SliderJoint::SetAxis( double x, double y, double z)
{
  dJointSetSliderAxis( this->jointId, x, y, z );
}


//////////////////////////////////////////////////////////////////////////////
// Set the _parameter
void SliderJoint::SetParam( int parameter, double value )
{
  dJointSetSliderParam( this->jointId, parameter, value );
}
//////////////////////////////////////////////////////////////////////////////
// Set the slider force
void SliderJoint::SetSliderForce(double force)
{
  dJointAddSliderForce(this->jointId, force);
}
