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
/* Desc: The base joint class
 * Author: Nate Keonig, Andrew Howard
 * Date: 21 May 2003
 * CVS: $Id: Joint.hh,v 1.7 2005/08/31 23:06:00 natepak Exp $
 */

#ifndef JOINT_HH
#define JOINT_HH

#include <ode/ode.h>
#include "Vector.hh"

class Body;

class Joint
{
  // Constructor
  protected: Joint();

  // Destructor
  protected: virtual ~Joint();

  // Get the type of the joint
  public: int GetType() const;

  // Get the body to which the joint is attached according the _index
  public: Body *GetJointBody( int index ) const;

  // Determines of the two bodies are connected by a joint
  public: bool AreConnected( Body *one, Body *two ) const;

  // Get the _parameter
  public: virtual double GetParam(int parameter) const;

  // Make this joint a fixed joint
  // Use this only when absolutely necessary
  public: void SetFixed();

  // Attach the two bodies with this joint
  public: void Attach( Body *one, Body *two );

  // Detach this joint from all bodies
  public: void Detach();

  // Set the anchor point
  public: virtual void SetAnchor( GzVector anchor ) {}

  // Set the anchor point
  public: virtual GzVector GetAnchor() const
          {return GzVectorSet(0,0,0);}


  // Set the _parameter to _value
  public: virtual void SetParam( int parameter, double value );

  // This is our id
  protected: dJointID jointId;

  // The first body this joint connects to
  private: Body* body1;

  // The second body this joint connects to
  private: Body* body2;
};

#endif

