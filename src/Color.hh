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
/* Desc: Color classes and functions
 * Author: Andrew Howard
 * Date: 4 Aug 2003
 * CVS: $Id: Color.hh,v 1.2 2004/10/23 06:32:58 inspectorg Exp $
 */

#ifndef GZCOLOR_H_
#define GZCOLOR_H_

#include <math.h>
#include <GL/gl.h>
#include "Global.hh"

// Generic color description
class GzColor
{

  // Default Constructor
  public: GzColor()
    {
      this->r = this->g = this->b = 0.0;
      this->a = 1.0;
    };
 
  // Constructor
  public: GzColor(double r, double g, double b, double a=1.0)
    {
      this->r = r;
      this->g = g;
      this->b = b;
      this->a = a;
    }

  // Return pointer to be used by GL
  public: operator const GLfloat *() {return &this->r;}
 
  // Red, Green, Blue, Alpha color components
  public: GLfloat r, g, b, a;
};


// Copy color structure to GL format
#define GZ_COLOR_COPY(x, y) x[0] = y.r; x[1] = y.g; x[2] = y.b; x[3] = y.a; 


#endif
