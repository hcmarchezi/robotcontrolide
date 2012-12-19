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
/* Desc: Base class for all drawables
 * Author: Nate Keonig
 * Date: 04 Aug 2003
 * CVS: $Id: Geom.hh,v 1.31 2005/01/26 21:08:08 natepak Exp $
 */

#ifndef GEOM_HH
#define GEOM_HH

#include "GL/gl.h"
#include "Global.hh"
#include "Color.hh"
#include "RenderOptions.hh"
#include "BaseGeom.hh"

// Forward declarations
class GzImage;

// Forward declarations for lib3ds
typedef struct _Lib3dsNode Lib3dsNode;
typedef struct _Lib3dsFile Lib3dsFile;
typedef struct _Lib3dsMesh Lib3dsMesh;

/// @brief Base class for GL-renderable geoms
///
/// BaseGeom and Geom are the base classes for all physical geometric
/// objects; BaseGeom encapsulates the mass and collision
/// characteritics, while Geom encapsulated the visual appearance.
class Geom : public BaseGeom
{
  public: Geom(dSpaceID spaceId);
  public: virtual ~Geom();
  
  // Set material properties
  public: void SetColor( const GzColor &color );
  public: void SetAmbientColor( const GzColor &color );
  public: void SetDiffuseColor( const GzColor &color );
  public: void SetSpecularColor( const GzColor &color );
  public: void SetEmissionColor( const GzColor &color );
  public: void SetShadeModel( const char *mode );
  public: void SetPolygonMode( const char *mode );
  public: void SetTransparency( bool enable );
  public: void SetShininess( float num );
  public: void SetLaser( float num );
  public: void SetRetro( float num );
  public: void SetFiducial( int id );
  public: void SetPickId( GLuint id );

  /// @brief Set the 2D texture.
  /// @param width, height Image dimensions (pixels).
  /// @param data Image data (packed RGB888).
  /// @returns Returns 0 on success.
  public: int SetTexture2D( int width, int height, const uint8_t *data );

  /// @brief Set the 2D texture from a file
  /// @param filename Texture file name (NULL for no texture).
  /// @returns Returns 0 on success.
  public: int SetTexture2DFile( const char *filename );

  /// @brief Set size of the texture image when mapped on a geom.
  /// @param size Size (x and y, in meters).
  public: void SetTexture2DSize( GzVector size );

  /// @brief Set the visual skin
  /// @param filename 3DS file containing skin
  public: int SetSkinFile( const char *filename );

  /// @brief Set the visual skin to null
  ///
  /// This method is useful for suppressing rendering of a geom when
  /// skins are available, such as when this geom is part of a model
  /// in which the principle skin is attached to a different geom.
  public: void SetSkinNull(bool enable) {this->skinNull = enable;}

  /// @brief Set the skin pose
  /// @param pose Skin pose relative to geom
  public: void SetSkinPose( GzPose pose );

  /// @brief Set the skin scale
  /// @param scale Scale vector
  public: void SetSkinScale( GzVector scale );

  /// @brief Do we have a skin?
  public: bool HasSkin() {return this->skinFile != NULL;}

  /// @brief Do we have null a skin?
  public: bool HasSkinNull() {return this->skinNull;}

  // Get material properties
  public: GzColor GetAmbientColor() const;
  public: GzColor GetDiffuseColor() const;
  public: GzColor GetSpecularColor() const;
  public: float GetTransparency() const;
  public: float GetShininess() const;
  public: float GetLaser() const;
  public: float GetRetro() const;
  public: int GetFiducial() const;
  public: GLuint GetPickId() const;

  /// @brief Set stored list options for a particular camera
  protected: void SetList(int camera, GLuint listId, RenderOptions opt);

  /// @brief Get stored list options for a particular camera
  protected: void GetList(int camera, GLuint *listId, RenderOptions *opt);

  // TODO: pass-by-value
  // Render the geom (GL)
  public: virtual void PreRender(RenderOptions *opt);
  public: virtual void Render(RenderOptions *opt);
  public: virtual void PostRender(RenderOptions *opt);

  /// Render the skin (should be called at rendering stage)
  public: void RenderSkin(RenderOptions *opt);

  // Render a node (recursive)
  private: void RenderSkinNode(Lib3dsNode *node);
  
  // Render a mesh 
  private: void RenderSkinMesh( Lib3dsMesh *mesh );

  // Rendering order
  public: int renderOrder;
  
  // Material attributes
  protected: bool transparency;
  protected: float laser;
  protected: float retro;
  protected: int fiducial;
  protected: GLfloat colorDiffuse[4];
  protected: GLfloat colorAmbient[4];
  protected: GLfloat colorSpecular[4];
  protected: GLfloat colorEmission[4];
  protected: GLfloat shininess;

  /// Texture image (for 2D textures)
  protected: GzImage *textureImage;
  
  /// Texture scale (image will map onto a road patch with the given dimensions)
  protected: GzVector textureSize;
  
  // ??
  protected: int pick;

  /// Cache for display list info
  private: struct
  {
    GLuint listId;
    RenderOptions opt;
  } listList[256];
  
  /// 3DS skin file
  private: Lib3dsFile *skinFile;

  /// Use null skin
  private: bool skinNull;

  /// Skin geometry
  private: GzPose skinPose;
  private: GzVector skinScale;
  private: GLuint meshListId;

  /// Rengerate display list (true/false)
  protected: bool dirty;
};

#endif

