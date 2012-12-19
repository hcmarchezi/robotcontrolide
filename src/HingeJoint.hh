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
/* Desc: A body that has a box shape
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: HingeJoint.hh,v 1.8 2005/12/02 16:26:51 natepak Exp $
 */

#ifndef HINGEJOINT_HH
#define HINGEJOINT_HH

#include "Vector.hh"
#include "Joint.hh"

class World;

class HingeJoint : public Joint
{
  /// @brief Constructor
  public: HingeJoint(World *world);

  // Destructor
  public: virtual ~HingeJoint();

  // Get the angle of rotation
  public: double GetAngle() const;

  // Get the rotation rate
  public: double GetAngleRate() const;

  // Get the specified parameter
  public: virtual double GetParam( int parameter ) const;

  /// @brief Set the anchor point
  public: virtual void SetAnchor(GzVector anchor);

  /// @brief Set the axis of rotation
  public: void SetAxis(GzVector axis);

  /// Get the anchor point
  public: virtual GzVector GetAnchor() const;

  /// Get the axis of rotation
  public: GzVector GetAxis() const;

  // Set the parameter to value
  public: virtual void SetParam( int parameter, double value );

  // Set the torque of a joint.
  public: void SetTorque(double torque);
};

#endif

