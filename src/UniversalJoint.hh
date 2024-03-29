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
/* Desc: A universal joint
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: UniversalJoint.hh,v 1.7 2005/12/02 16:26:51 natepak Exp $
 */

#ifndef UNIVERSALJOINT_HH
#define UNIVERSALJOINT_HH

#include "Joint.hh"

class World;

class UniversalJoint : public Joint
{
  /// @brief Constructor
  public: UniversalJoint(World *world);

  // Destuctor
  public: virtual ~UniversalJoint();

  // Get the anchor point
  public: virtual GzVector GetAnchor() const;

  // Get the first axis of rotation
  public: GzVector GetAxis1() const;

  // Get the second axis of rotation
  public: GzVector GetAxis2() const;

  // Get the angle of axis 1
  public: double GetAngle1() const;

  // Get the angle of axis 2
  public: double GetAngle2() const;

  // Get the angular rate of axis 1
  public: double GetAngleRate1() const;

  // Get the angular rate of axis 2
  public: double GetAngleRate2() const;

  // Set the anchor point
  public: virtual void SetAnchor( GzVector anchor );

  // Set the first axis of rotation
  public: void SetAxis1( GzVector axis );

  // Set the second axis of rotation
  public: void SetAxis2( GzVector axis );

  // Set the parameter to value
  public: virtual void SetParam( int parameter, double value );

  // Set the torque of a joint.
  public: virtual void SetTorque(double torque1, double torque2);
};

#endif

