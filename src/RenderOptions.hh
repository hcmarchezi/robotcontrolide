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
/* Desc: Options for rendering objects (set by camera, processed by geoms)
 * Author: Andrew Howard
 * Date: 22 May 2004
 * CVS: $Id: RenderOptions.hh,v 1.12 2004/10/31 01:47:19 inspectorg Exp $
 */

#ifndef RENDEROPTIONS_HH
#define RENDEROPTIONS_HH

#include "Vector.hh"

/// @brief Options for OpenGL rendering of Geoms
///
/// Rendering options are passed to each Geom during the Render()
/// call; the options control such things as whether or not textures
/// should be rendered.
struct RenderOptions
{
  // Display bounding boxes
  bool displayBBox;

  // Display body axes
  bool displayAxes;

  // Display CoM axes
  bool displayCoM;

  // Display skins
  bool displaySkins;

  // Display sensor rays
  bool displayRays;

  // Display frustrums
  bool displayFrustrums;

  /// Display materials
  bool displayMaterials;

  /// Display textures
  bool displayTextures;
  
  /// Polygon rendering mode; e.g., GL_LINE or GL_FILL
  int polygonMode;
  
  /// Shade model; e.g. GL_SMOOTH or GL_FLAT
  int shadeModel;

  /// Camera index; filled in by camera so geoms can manage display
  /// lists
  int cameraIndex;
  
  /// Camera pose; filled in by camera so geoms can clip stuff that is
  /// far from the camera.
  GzPose cameraPose;

  /// Camera far clip distance; useful for culling.
  double farClip;
};

#endif
