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
 * CVS: $Id: Vector.cc,v 1.11 2005/11/22 16:44:51 natepak Exp $
 */

#include <assert.h>
#include <stdio.h>

#include "Error.hh"
#include "Vector.hh"


//////////////////////////////////////////////////////////////////////////////
// Create a zero vector
GzVector GzVectorZero()
{
  GzVector v;

  v.x = 0;
  v.y = 0;
  v.z = 0;

  return v;
}


//////////////////////////////////////////////////////////////////////////////
// Create a vector from the given values
GzVector GzVectorSet(double x, double y, double z)
{
  GzVector v;

  v.x = x;
  v.y = y;
  v.z = z;

  return v;
}


//////////////////////////////////////////////////////////////////////////////
// Create a vector from a quaternion
GzVector GzVectorSetQuatern(GzQuatern q)
{
  GzVector v;

  v.x = q.x;
  v.y = q.y;
  v.z = q.z;

  return v;
}


//////////////////////////////////////////////////////////////////////////////
// Add one vector to another (element by element)
// c = b + a
GzVector GzVectorAdd(GzVector b, GzVector a)
{
  return GzVectorSet(b.x + a.x, b.y + a.y, b.z + a.z);
}


//////////////////////////////////////////////////////////////////////////////
// Subtract one vector from another (element by element)
// c = b - a
GzVector GzVectorSub(GzVector b, GzVector a)
{
  return GzVectorSet(b.x - a.x, b.y - a.y, b.z - a.z);
}


//////////////////////////////////////////////////////////////////////////////
// Mutliply vector by scalar
GzVector GzVectorMul(double s, GzVector a)
{
  GzVector b;

  b.x = s * a.x;
  b.y = s * a.y;
  b.z = s * a.z;

  return b;
}


//////////////////////////////////////////////////////////////////////////////
// Compute vector magnitude
double GzVectorMag(GzVector a)
{
  return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}


//////////////////////////////////////////////////////////////////////////////
// Normalize a vector to unit length
GzVector GzVectorUnit(GzVector a)
{
  double d;
  GzVector b;

  d = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
  b.x = a.x / d;
  b.y = a.y / d;
  b.z = a.z / d;

  return b;
}


//////////////////////////////////////////////////////////////////////////////
// Take cross product of two vectors
GzVector GzVectorCross(GzVector a, GzVector b)
{
  GzVector c;

  c.x =  a.y * b.z - a.z * b.y;
  c.y = -a.x * b.z + a.z * b.x;
  c.z =  a.x * b.y - a.y * b.x;
  
  return c;
}


//////////////////////////////////////////////////////////////////////////////
// Take dot product of two vectors
double GzVectorDot(GzVector a, GzVector b)
{
  return a.x * b.x + a.y * b.y + a.x * b.z;
}


//////////////////////////////////////////////////////////////////////////////
// See if a vector is finite (e.g., not nan)
bool GzVectorFinite(GzVector a)
{
  return finite(a.x) && finite(a.y) && finite(a.z);
}


//////////////////////////////////////////////////////////////////////////////
// Create a quaternion from elements
GzQuatern GzQuaternSet(double u, double x, double y, double z)
{
  GzQuatern q;

  q.u = u;
  q.x = x;
  q.y = y;
  q.z = z;

  return q;
}


//////////////////////////////////////////////////////////////////////////////
// Create a quaternion from a vector
GzQuatern GzQuaternSetVector(GzVector v)
{
  GzQuatern q;

  q.u = 0.0;
  q.x = v.x;
  q.y = v.y;
  q.z = v.z;

  return q;
}


//////////////////////////////////////////////////////////////////////////////
// Create an identity quaternion
GzQuatern GzQuaternIdent()
{
  GzQuatern q;

  q.u = 1.0;
  q.x = 0.0;
  q.y = 0.0;
  q.z = 0.0;
  
  return q;
}


//////////////////////////////////////////////////////////////////////////////
// Create a quaternion from an axis and angle
GzQuatern GzQuaternFromAxis(double x, double y, double z, double a)
{
  double l;
  GzQuatern q;

  l = x * x + y * y + z * z;
  
  if (l > 0.0)
  {
    a *= 0.5;
    l = sin(a) / sqrt(l);
    q.u = cos(a);
    q.x = x * l;
    q.y = y * l;
    q.z = z * l;
  }
  else
  {
    q.u = 1;
    q.x = 0;
    q.y = 0;
    q.z = 0;
  }

  q = GzQuaternNormal(q);
    
  return q;
}


//////////////////////////////////////////////////////////////////////////////
// Convert quaterion to axis and angle
GzQuatern GzQuaternToAxis(GzQuatern a)
{
  GzQuatern b;

  b.x = a.x;
  b.y = a.y;
  b.z = a.z;
  b.u = acos(a.u) * 2;
  
  return b;
}


//////////////////////////////////////////////////////////////////////////////
// Create a quaternion from Euler angles
GzQuatern GzQuaternFromEuler(double roll, double pitch, double yaw)
{
  double phi, the, psi;
  GzQuatern p;

  phi = roll / 2;
  the = pitch / 2;
  psi = yaw / 2;

  // REMOVE
  /*
  c = GzQuaternSet(cos(phi), sin(phi), 0, 0);
  b = GzQuaternSet(cos(the), 0, sin(the), 0);
  a = GzQuaternSet(cos(psi), 0, 0, sin(psi));  
  q = GzQuaternMul(a, GzQuaternMul(b, c));
  */
 
  p.u = cos(phi) * cos(the) * cos(psi) + sin(phi) * sin(the) * sin(psi);
  p.x = sin(phi) * cos(the) * cos(psi) - cos(phi) * sin(the) * sin(psi);
  p.y = cos(phi) * sin(the) * cos(psi) + sin(phi) * cos(the) * sin(psi);
  //p.z = cos(phi) * cos(the) * sin(psi) - sin(phi) * sin(the) * sin(psi);
  p.z = cos(phi) * cos(the) * sin(psi) - sin(phi) * sin(the) * cos(psi);

  p = GzQuaternNormal(p);
  
  return p;
}

//////////////////////////////////////////////////////////////////////////////
// Create a quatern from Homogeneous matrix
GzQuatern GzQuaternFromHomo(GzHomo a)
{
  GzQuatern b;
  b.u = sqrt(1 + a.m[0][0] + a.m[1][1] + a.m[2][2]) / 2;
  assert(b.u >= 1e-16); // TODO: switch cases to avoid instability
  b.x = (a.m[2][1] - a.m[1][2]) / (4 * b.u);
  b.y = (a.m[0][2] - a.m[2][0]) / (4 * b.u);
  b.z = (a.m[1][0] - a.m[0][1]) / (4 * b.u);
  b = GzQuaternNormal(b);
  return b;
}


//////////////////////////////////////////////////////////////////////////////
// Convert quaternion to Euler angles
GzVector GzQuaternToEuler(GzQuatern q)
{
  double phi, the, psi;

  q = GzQuaternNormal(q);
  
  phi = atan2(2 * (q.y*q.z + q.u*q.x), (q.u*q.u - q.x*q.x - q.y*q.y + q.z*q.z));
  the = asin(-2 * (q.x*q.z - q.u * q.y));
  psi = atan2(2 * (q.x*q.y + q.u*q.z), (q.u*q.u + q.x*q.x - q.y*q.y - q.z*q.z));
  
  return GzVectorSet(phi, the, psi);
}


/* REMOVE (not correct)
//////////////////////////////////////////////////////////////////////////////
// Create a Quaternion from a 3x3 rotation matrix
GzQuatern GzQuaternFromRot( double rot[9] )
{
  int which;
  double temp;
  GzQuatern q;

  q.u = (1 + rot[0] + rot[4] + rot[8]) / 4;
  q.x = (1 + rot[0] - rot[4] - rot[8]) / 4;
  q.y = (1 - rot[0] + rot[4] - rot[8]) / 4;
  q.z = (1 - rot[0] - rot[4] + rot[8]) / 4;

  temp = q.u;
  which = 1;
  if (q.x > temp)
  {
    temp = q.x;
    which = 2;
  }
  if (q.y > temp)
  {
    temp = q.y;
    which = 3;
  }
  if (q.z > temp)
  {
    which = 4;
  }

  switch(which)
  {
    case 1:
      q.u = sqrt(q.u);
      temp = 1.0/(4.0*q.u);
      q.x = (rot[7] - rot[5]) * temp;
      q.y = (rot[2] - rot[6]) * temp;
      q.z = (rot[3] - rot[1]) * temp;
      break;
    case 2:
      q.x = sqrt(q.x);
      temp = 1.0/(4.0*q.x);
      q.u = (rot[7] - rot[5]) * temp;
      q.y = (rot[1] + rot[3]) * temp;
      q.z = (rot[2] + rot[6]) * temp;
      break;
    case 3:
      q.y = sqrt(q.y);
      temp = 1.0/(4.0*q.y);
      q.y = (rot[2] - rot[6]) * temp;
      q.x = (rot[1] + rot[3]) * temp;
      q.z = (rot[5] + rot[7]) * temp;
      break;
    case 4:
      q.z = sqrt(q.z);
      temp = 1.0/(4.0*q.z);
      q.u = (rot[3] - rot[1]) * temp;
      q.x = (rot[2] + rot[6]) * temp;
      q.y = (rot[5] + rot[7]) * temp;
      break;
  }

  return q;
}
*/


/* REMOVE
//////////////////////////////////////////////////////////////////////////////
//  Create a quaternion from a pair of points.
GzQuatern GzQuaternFromLookAt(GzVector center, GzVector up)
{
  GzQuatern a;
  GzVector x, y, z;
  double R[9];

  x = GzVectorUnit(center);
  y = GzVectorCross(GzVectorUnit(up), x);
  z = GzVectorCross(x, y);

  R[0] = x.x;
  R[3] = x.y;
  R[6] = x.z;

  R[1] = y.x;
  R[4] = y.y;
  R[7] = y.z;
  
  R[2] = z.x;
  R[5] = z.y;
  R[8] = z.z;

  a = GzQuaternFromRot(R);

  return a;
}
*/


//////////////////////////////////////////////////////////////////////////////
// Invert a quaternion
GzQuatern GzQuaternInverse(GzQuatern a)
{
  GzQuatern b;

  b.u = a.u;
  b.x = -a.x;
  b.y = -a.y;
  b.z = -a.z;
  
  return b;
}


//////////////////////////////////////////////////////////////////////////////
// Normalize a quaternion
GzQuatern GzQuaternNormal(GzQuatern a)
{
  GzQuatern b;
  double s;

  s = sqrt(a.u * a.u + a.x * a.x + a.y * a.y + a.z * a.z);

  b.u = a.u / s;
  b.x = a.x / s;
  b.y = a.y / s;
  b.z = a.z / s;

  return b;
}


//////////////////////////////////////////////////////////////////////////////
// Multiple two quaternions
GzQuatern GzQuaternMul(GzQuatern a, GzQuatern b)
{
  GzQuatern c;

  c.u = a.u * b.u - a.x * b.x - a.y * b.y - a.z * b.z;
  c.x = a.u * b.x + a.x * b.u + a.y * b.z - a.z * b.y;
  c.y = a.u * b.y - a.x * b.z + a.y * b.u + a.z * b.x;
  c.z = a.u * b.z + a.x * b.y - a.y * b.x + a.z * b.u;
  
  return c;
}


//////////////////////////////////////////////////////////////////////////////
// Scale a quaternion rotation
GzQuatern GzQuaternScale(double s, GzQuatern a)
{
  GzQuatern b;

  // Convert to axis-and-angle
  b = GzQuaternToAxis(a);

  // Scale angle
  b.u *= s;

  // Convert back again
  return GzQuaternFromAxis(b.x, b.y, b.z, b.u);
}



//////////////////////////////////////////////////////////////////////////////
// See if a quatern is finite (e.g., not nan)
bool GzQuaternFinite(GzQuatern a)
{
  return finite(a.x) && finite(a.y) && finite(a.z) && finite(a.u);
}


//////////////////////////////////////////////////////////////////////////////
// Create a pose from the given position and rotation
GzPose GzPoseSet(GzVector pos, GzQuatern rot)
{
  GzPose p;

  p.pos = pos;
  p.rot = rot;

  return p;
}


//////////////////////////////////////////////////////////////////////////////
//  Create a pose from a triplet of vectors
GzPose GzPosePointAt(GzVector pos, GzVector at, GzVector up)
{
  GzPose pose;
  GzQuatern b;
  GzVector x, y, z;
  double R[3][3];

  x = GzVectorUnit(GzVectorSub(at, pos));
  y = GzVectorUnit(GzVectorCross(GzVectorUnit(up), x));
  z = GzVectorCross(x, y);

  R[0][0] = x.x;
  R[1][0] = x.y;
  R[2][0] = x.z;

  R[0][1] = y.x;
  R[1][1] = y.y;
  R[2][1] = y.z;
  
  R[0][2] = z.x;
  R[1][2] = z.y;
  R[2][2] = z.z;

  // TODO: switch cases to avoid instability
  b.u = sqrt(1 + R[0][0] + R[1][1] + R[2][2]) / 2;
  if (b.u < 1e-16)
    PRINT_WARN("pointing singularity (gimbal lock)"); 
  b.x = (R[2][1] - R[1][2]) / (4 * b.u);
  b.y = (R[0][2] - R[2][0]) / (4 * b.u);
  b.z = (R[1][0] - R[0][1]) / (4 * b.u);
  b = GzQuaternNormal(b);

  /*
  printf("x = %f %f %f  \n", x.x, x.y, x.z);
  printf("y = %f %f %f  \n", y.x, y.y, y.z);
  printf("z = %f %f %f  \n", z.x, z.y, z.z);
  printf("%f  \n", b.u);
  */

  pose.pos = pos;
  pose.rot = b;

  return pose;
}



//////////////////////////////////////////////////////////////////////////////
// See if a pose is finite (e.g., not nan)
bool GzPoseFinite(GzPose a)
{
  return GzVectorFinite(a.pos) && GzQuaternFinite(a.rot);
}


//////////////////////////////////////////////////////////////////////////////
// Add one pose to another: c = b + a
GzPose GzCoordPoseAdd(GzPose bpose, GzPose apose)
{
  GzPose cpose;

  cpose.pos = GzCoordPositionAdd(bpose.pos, apose.pos, apose.rot);
  cpose.rot = GzCoordRotationAdd(bpose.rot, apose.rot);

  return cpose;
}


//////////////////////////////////////////////////////////////////////////////
// Subtract one pose from another: c = b - a
GzPose GzCoordPoseSub(GzPose bpose, GzPose apose)
{
  GzPose cpose;

  cpose.pos = GzCoordPositionSub(bpose.pos, apose.pos, apose.rot);
  cpose.rot = GzCoordRotationSub(bpose.rot, apose.rot);

  return cpose;
}


//////////////////////////////////////////////////////////////////////////////
// Find the inverse of a pose; i.e., if b = ba + a, given b and ba,
// find a
GzPose GzCoordPoseSolve(GzPose ba, GzPose b)
{
  GzQuatern q;
  GzPose a;

  a.rot = GzQuaternMul(GzQuaternInverse(ba.rot), b.rot);                       
  q = GzQuaternMul(a.rot, GzQuaternSetVector(ba.pos));
  q = GzQuaternMul(q, GzQuaternInverse(a.rot));
  a.pos = GzVectorSub(b.pos, GzVectorSetQuatern(q));
  
  return a;
}

  
//////////////////////////////////////////////////////////////////////////////
// Add one position to another: c = b + a
GzVector GzCoordPositionAdd(GzVector bpos, GzVector apos, GzQuatern arot)
{
  GzVector cpos;
  
  // cpos = apos + arot * bpos * arot!
  cpos = GzVectorSetQuatern(GzQuaternMul(arot, GzQuaternMul(GzQuaternSetVector(bpos), GzQuaternInverse(arot))));
  cpos = GzVectorAdd(apos, cpos);

  return cpos;
}


//////////////////////////////////////////////////////////////////////////////
// Subtract one position from another: c = b - a
GzVector GzCoordPositionSub(GzVector bpos, GzVector apos, GzQuatern arot)
{
  GzVector cpos;

  // cpos = arot! * (bpos - apos) * arot
  cpos = GzVectorSub(bpos, apos);
  cpos = GzVectorSetQuatern(
      GzQuaternMul(GzQuaternInverse(arot), 
                   GzQuaternMul(GzQuaternSetVector(cpos), arot)
                  )
      );

  return cpos;
}


//////////////////////////////////////////////////////////////////////////////
// Add one rotation to another: b = r + a
GzQuatern GzCoordRotationAdd(GzQuatern r, GzQuatern a)
{
  GzQuatern b;

  // b = a * r
  b = GzQuaternMul(a, r);

  return b;
}


//////////////////////////////////////////////////////////////////////////////
// Subtract one rotation from another: r = b - a
GzQuatern GzCoordRotationSub(GzQuatern b, GzQuatern a)
{
  GzQuatern r;

  // r = a! * b
  r = GzQuaternMul(GzQuaternInverse(a), b);

  return r;
}

//////////////////////////////////////////////////////////////////////////////
// Compute a homogeneous transform matrix from a Quatern
GzHomo GzHomoFromQuatern(GzQuatern q)
{
  GzHomo b;

  b.m[0][0] = 1 - (2*(q.y*q.y) + 2*(q.z*q.z));
  b.m[0][1] = 2 * q.x * q.y + 2 * q.z * q.u;
  b.m[0][2] = 2 * q.x * q.z - 2 * q.y * q.u;

  b.m[1][0] = 2 * q.x * q.y - 2 * q.z * q.u;
  b.m[1][1] = 1 - (2*(q.x*q.x) + 2*(q.z*q.z));
  b.m[1][2] = 2 * q.y * q.z + 2 * q.x * q.u;

  b.m[2][0] = 2 * q.x * q.z + 2 * q.y * q.u;
  b.m[2][1] = 2 * q.y * q.z - 2 * q.x * q.u;
  b.m[2][2] = 1 - (2*(q.x*q.x) + 2*(q.y*q.y));

  return b;
}


//////////////////////////////////////////////////////////////////////////////
// Compute the inverse of the given transform matrix
GzHomo GzHomoInverse(GzHomo a)
{
  double det;
  GzHomo b;

  det = ( + a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]) - a.m[0][1] * (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0]) + a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]));
  b.m[0][0] = ( + (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1])) / det;
  b.m[0][1] = ( - (a.m[0][1] * a.m[2][2] - a.m[0][2] * a.m[2][1])) / det;
  b.m[0][2] = ( + (a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1])) / det;
  b.m[1][0] = ( - (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0])) / det;
  b.m[1][1] = ( + (a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0])) / det;
  b.m[1][2] = ( - (a.m[0][0] * a.m[1][2] - a.m[0][2] * a.m[1][0])) / det;
  b.m[2][0] = ( + (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0])) / det;
  b.m[2][1] = ( - (a.m[0][0] * a.m[2][1] - a.m[0][1] * a.m[2][0])) / det;
  b.m[2][2] = ( + (a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0])) / det;
  
  return b;
}


//////////////////////////////////////////////////////////////////////////////
/// Apply a homogeneous transform
GzVector GzHomoMul(GzHomo m, GzVector a)
{
  GzVector b;

  b.x = m.m[0][0] * a.x + m.m[0][1] * a.y + m.m[0][2] * a.z;
  b.y = m.m[1][0] * a.x + m.m[1][1] * a.y + m.m[1][2] * a.z;
  b.z = m.m[2][0] * a.x + m.m[2][1] * a.y + m.m[2][2] * a.z;
  
  return b;
}

