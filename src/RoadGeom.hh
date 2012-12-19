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
/* Desc: Road geometry
 * Author: Andrew Howard
 * Date: 22 Oct 2004
 * CVS: $Id: RoadGeom.hh,v 1.7 2004/11/01 01:50:24 inspectorg Exp $
 */

#ifndef ROADGEOM_HH
#define ROADGEOM_HH

#include "Geom.hh"


class RoadGeom : public Geom
{
  /// @brief Constructor
  public: RoadGeom(Body *body, dSpaceID spaceId);
  
  /// @brief Destructor
  public: virtual ~RoadGeom();

  /// @brief Set the nominal spacing between track points (m)
  public: void SetTrackSpacing(double spacing) {this->trackScale = spacing;}

  /// @brief Set the nominal spacing between profile points (m)
  public: void SetProfileSpacing(double spacing) {this->profileScale = spacing;}

  /// @brief Add a point to the track
  /// @param pos Point position (relative to road).
  /// @returns Track index
  public: int AddTrackPoint(GzVector pos);

  /// @brief Add a point to the profile
  /// @param index Track index
  /// @param pz Height above or below the nominal road level.
  public: void AddProfilePoint(int index, double pz);

  /// @brief Add a sign.
  /// @param pos Sign position (relative to road).
  /// @param text Sign text (the value will be copied).
  public: void AddSign(GzVector pos, const char *text);

  /// @brief Initialize the mesh
  private: void InitMesh();

  /// @brief Update the mesh
  public: void UpdateMesh(GzVector pos);

  /// @brief Compute normal vectors on vertices
  private: void UpdateNormals();

  /// @brief Compute a Catmull-Rom spline
  private: GzVector SplineCR(double t, GzVector p0, GzVector p1,
                             GzVector p2, GzVector p3);

  /// @brief Compute a Catmull-Rom spline tangent
  private: GzVector SplineCRtan(double t, GzVector p0, GzVector p1,
                                GzVector p2, GzVector p3);

  /// @brief Render the geom (GL)
  public: virtual void Render(RenderOptions *opt);

  /// @brief Render signs
  private: void RenderSigns(RenderOptions *opt, RenderOptions listOpt);

  /// Data manager for trimesh
  private: dTriMeshDataID tridata;

  /// ODE trimesh geom
  private: dGeomID trimesh;

  /// Mesh dimensions
  private: int stepsX, stepsY;

  /// Mesh scale (m / step)
  private: double scale;

  /// Description for a single point on the road
  private: struct TrackPoint
  {
    // Position
    GzVector pos;
    
    /// Profile data
    int profilePointCount, profilePointMaxCount;
    GzVector *profilePoints;
  };
  
  /// Track data
  private: double trackScale, profileScale;
  private: int trackPointCount, trackPointMaxCount;
  private: TrackPoint *trackPoints;

  /// Description for a single sign
  private: struct SignData
  {
    /// Position
    GzVector pos;

    /// Text
    char *text;
  };

  /// Sign data
  private: int signCount, signMaxCount;
  private: SignData *signs;

  /// Vertex data
  private: int vertexCount, vertexMaxCount;
  private: dVector3 *vertices;
  private: GzVector *normals;

  /// Triangle data
  private: int indexCount, indexMaxCount;
  private: int *indices;

  /// Maximum patch size; road will be re-rendered when the camera is
  /// outside the current patch.
  private: double patchSize;
};

#endif
