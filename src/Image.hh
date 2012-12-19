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
/* Desc: Useful image class
 * Author: Andrew Howard
 * Date: 22 May 2004
 * CVS: $Id: Image.hh,v 1.3 2004/11/11 16:36:28 natepak Exp $
 */

#ifndef GZIMAGE_H_
#define GZIMAGE_H_

#include <stdint.h>
#include "GL/gl.h"

// compute linear index for given map coords
#define MAP_IDX(mf, i, j) ((mf->size_x) * (j) + (i))

// Useful image class; uses GL enums to make rendering easy
class GzImage
{
  // Constructors/destructors
  public: GzImage();
  public: virtual ~GzImage();

  /// @brief Load an image from data
  /// @param width, height Image dimensions (pixels).
  /// @param data Image data (packed RGB888).
  /// @returns Returns 0 on success.
  public: int Set(int width, int height, const uint8_t *data);

  /// @brief Load an image from a file
  public: int Load(const char *filename);

  // Image dimensions
  public: int width, height;

  // Number of color components (1, 2, 3 or 4)
  public: int components;

  // Image format (e.g., GL_RGB, GL_RGBA)
  public: int format;

  // Data type (e.g., GL_UNSIGNED_BYTE)
  public: int type;

  // Image data
  public: int size;
  public: uint8_t *data;
};



#endif

