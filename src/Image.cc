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
 * CVS: $Id: Image.cc,v 1.6 2004/11/11 18:10:20 inspectorg Exp $
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Error.hh"
#include "Image.hh"

#ifdef HAVE_GDK_PIXBUF
#include <gdk-pixbuf/gdk-pixbuf.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// Constructors
GzImage::GzImage()
{
  this->width = this->height = 0;
  this->components = 0;
  this->format = 0;
  this->type = 0;
  this->size = 0;
  this->data = NULL;

  return;
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
GzImage::~GzImage()
{
  if (this->data)
    delete [] this->data;
  return;
}


/////////////////////////////////////////////////////////////////////////////
// Load an image from data
int GzImage::Set(int width, int height, const uint8_t *data)
{
  this->width = width;
  this->height = height;
  this->components = 3;
  this->format = GL_RGB;
  this->type = GL_UNSIGNED_BYTE;

  this->size = this->width * this->height * this->components;
  this->data = new uint8_t[this->size];

  memcpy(this->data, data, this->size);

  return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Load an image
int GzImage::Load(const char *filename)
{
#ifdef HAVE_GDK_PIXBUF
  GdkPixbuf* pixbuf = NULL;
  GError *error = NULL;
  guchar* pixels = NULL;
  guchar* p = NULL;
  int i,j,k;
  int rowstride, bps;

  // Initialize glib
  g_type_init();

  // Read the image
  if(!(pixbuf = gdk_pixbuf_new_from_file(filename, &error)))
  {
    PRINT_ERR1("failed to open image file %s", filename);
    return(-1);
  }

  this->width = gdk_pixbuf_get_width(pixbuf);
  this->height = gdk_pixbuf_get_height(pixbuf);
  bps = gdk_pixbuf_get_bits_per_sample(pixbuf)/8;
  rowstride = gdk_pixbuf_get_rowstride(pixbuf);
  this->components = gdk_pixbuf_get_n_channels(pixbuf);

  this->type = GL_UNSIGNED_BYTE;

  if(gdk_pixbuf_get_has_alpha(pixbuf))
  {
    this->components++;
    this->format = GL_RGBA;
  }
  else 
    this->format = GL_RGB;

  this->size = this->width * this->height * this->components;

  this->data = new uint8_t[this->size];

  // Read data
  pixels = gdk_pixbuf_get_pixels(pixbuf);
  for(j = 0; j < this->height; j++)
  {
    for (i = 0; i < this->width; i++)
    {
      p = pixels + j*rowstride + i*this->components*bps;
      for (k=0; k<3; k++)
      {
        this->data[j*this->width*this->components + i*this->components*bps+k] = (uint8_t)*(p+k);
      }
    }
  }

  gdk_pixbuf_unref(pixbuf);

  return 0;
#else
  PRINT_WARN("image loading disabled at compile time (no GDK pixbuf support)");
  return 0;
#endif
}
