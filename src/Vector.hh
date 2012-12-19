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
/* Desc: Vector classes and functions
 * Author: Andrew Howard
 * Date: 11 Jun 2003
 * CVS: $Id: Vector.hh,v 1.8 2005/11/22 16:44:51 natepak Exp $
 */

#ifndef GZVECTOR_H_
#define GZVECTOR_H_

#include <math.h>

#include "Global.hh"

// Basic 3 vector
struct GzVector
{
  double x, y, z;
};


// Quatnernion
struct GzQuatern
{
  double u, x, y, z;
};


// Combined pose
struct GzPose
{
  GzVector pos;
  GzQuatern rot;
};


/// @brief Matrix for homogeneous transforms on 2D points
typedef struct
{
  double m[3][3];
} GzHomo;



// Create a zero vector
GzVector GzVectorZero();

// Create a vector from the given values
GzVector GzVectorSet(double x, double y, double z);

// Create a vector from a quaternion
GzVector GzVectorSetQuatern(GzQuatern q);

// Add one vector to another (element by element)
GzVector GzVectorAdd(GzVector b, GzVector a);

// Subtract one vector from another (element by element)
GzVector GzVectorSub(GzVector b, GzVector a);

// Mutliply vector by scalar
GzVector GzVectorMul(double s, GzVector a);

// Compute vector magnitude
double GzVectorMag(GzVector a);

// Normalize a vector to unit length
GzVector GzVectorUnit(GzVector a);

// Take cross product of two vectors
GzVector GzVectorCross(GzVector a, GzVector b);

// Take dot product of two vectors
double GzVectorDot(GzVector a, GzVector b);

// See if a vector is finite (e.g., not nan)
bool GzVectorFinite(GzVector a);


// Create an identity quaternion
GzQuatern GzQuaternIdent();

// Create a quaternion from elements
GzQuatern GzQuaternSet(double u, double x, double y, double z);

// Create a quaternion form a vector
GzQuatern GzQuaternSetVector(GzVector v);

// Invert a quaternion
GzQuatern GzQuaternInverse(GzQuatern a);

// Normalize a quaternion
GzQuatern GzQuaternNormal(GzQuatern a);

// Create a quaternion from an axis and angle
GzQuatern GzQuaternFromAxis(double x, double y, double z, double a);

// Create a quaternion from Euler angles
GzQuatern GzQuaternFromEuler(double roll, double pitch, double yaw);

// Create a quatern from Homogeneous matrix
GzQuatern GzQuaternFromHomo(GzHomo homo);

// Convert quaternion to Euler angles (roll, pitch, yaw)
GzVector GzQuaternToEuler(GzQuatern q);

// Convert quaterion to axis and angle (x, y, z, rotation)
GzQuatern GzQuaternToAxis(GzQuatern a);

// Multiply two quaternions
GzQuatern GzQuaternMul(GzQuatern a, GzQuatern b);

// Scale a quaternion rotation
GzQuatern GzQuaternScale(double s, GzQuatern a);

// See if a quaternion is finite (e.g., not nan)
bool GzQuaternFinite(GzQuatern a);


// Create a pose from the given position and rotation
GzPose GzPoseSet(GzVector pos, GzQuatern rot);

/// @brief Create a pose from a triplet of vectors.
/// @param pos specifies the object position.
/// @param at specifies the point to look at (x axis).
/// @param up Specifies the "up" direction (z axis).
GzPose GzPosePointAt(GzVector pos, GzVector at, GzVector up);

// See if a pose is finite (e.g., not nan)
bool GzPoseFinite(GzPose a);




// Add one pose to another: c = b + a
GzPose GzCoordPoseAdd(GzPose bpose, GzPose apose);

// Subtract one pose from another: c = b - a
GzPose GzCoordPoseSub(GzPose bpose, GzPose apose);

// Find the inverse of a pose; i.e., if b = ba + a, given b and ba,
// find a
GzPose GzCoordPoseSolve(GzPose ba, GzPose b);

// Add one position to another: c = b + a
GzVector GzCoordPositionAdd(GzVector bpos, GzVector apos, GzQuatern arot);

// Subtract one position from another: c = b - a
GzVector GzCoordPositionSub(GzVector bpos, GzVector apos, GzQuatern arot);

// Add one rotation to another: c = b + a
GzQuatern GzCoordRotationAdd(GzQuatern b, GzQuatern a);

// Subtract one rotation from another: c = b - a
GzQuatern GzCoordRotationSub(GzQuatern b, GzQuatern a);


/// @brief Compute a homogeneous transform matrix from a Quatern
GzHomo GzHomoFromQuatern(GzQuatern q);

/// @brief Compute the inverse of the given transform matrix
GzHomo GzHomoInverse(GzHomo a);

/// @brief Apply a homogeneous transform
GzVector GzHomoMul(GzHomo m, GzVector a);


#endif
