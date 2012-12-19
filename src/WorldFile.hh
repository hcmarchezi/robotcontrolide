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
/*
 * Desc: A class for reading and writing the world file.
 * Author: Andrew Howard
 * Date: 15 Nov 2001
 * CVS info: $Id: WorldFile.hh,v 1.22 2004/11/14 07:39:04 inspectorg Exp $
 */

#ifndef WORLDFILE_HH
#define WORLDFILE_HH

#include <libxml/parser.h>

#include "Vector.hh"
#include "Color.hh"

// Forward declarations
class WorldFileNode;


/// @brief Class encapsulating the world file
class WorldFile
{
  /// @brief Standard constructor
  public: WorldFile();

  /// @brief Standard Destructor
  public: ~WorldFile();

  /// @brief Load world from file
  public: int Load( const char *filename );

  /// @brief Load a world file from a string
  public: int LoadString( const char *str );

  /// @brief Save world back into file
  /// @param filename Set to NULL to save back into the original file.
  public: int Save( const char *filename );

  /// @brief Print a warning message with unused nodes (if any).
  /// @returns Returns true if there are unused nodes.
  public: bool WarnUnused();
  
  /// @brief Get the root node @internal
  public: WorldFileNode *GetRootNode() const;

  // Create wrappers
  private: WorldFileNode *CreateNodes( WorldFileNode *parent, 
                                       xmlNodePtr xmlNode );

  /// @brief File name
  public: char *filename;
  
  // XML data
  private: xmlDocPtr xmlDoc;

  // The root of the tree
  private: WorldFileNode *root;
};


/// @brief Class encapsulating a single file node
class WorldFileNode
{
  /// @brief Standard constructor @internal
  public: WorldFileNode( WorldFile *file, WorldFileNode *parent, 
                         xmlNodePtr xmlNode, xmlDocPtr xmlDoc );

  /// @brief Standard destructor
  public: ~WorldFileNode();

  /// @brief Get the node name @internal
  public: const char *GetName();

  /// @brief Get the name space prefix @internal
  public: const char *GetNSPrefix();

  /// @brief Get the next sibling of this node @internal
  public: WorldFileNode *GetNext();

  /// @brief Get the first child of this node @internal
  public: WorldFileNode *GetChild();

  /// @brief Get a child based on a name. @internal
  /// @returns Returns null if not found.
  public: WorldFileNode *GetChild( const char *name );

  /// @brief Get the first child with the specified namespace prefix.  @internal
  public: WorldFileNode *GetChildByNSPrefix( const char *prefix);

  /// @brief Get a node's value.  
  private: char* GetNodeValue( const char *key );

  /// @brief Set a value associated with a node.
  private: void SetNodeValue( const char *key, const char* value );

  /// @brief Print node contents (for debugging purposes); recursive
  public: void Print();

  /// @brief Do a recursive check for unused nodes.
  public: bool WarnUnused();

  /// @brief See if a node exists
  public: bool IsDefined( const char *key ) {return GetNodeValue(key) != NULL;}

  /// @brief Get an attribute string value
  public: const char *GetString( const char *key, const char *def, int require = 0 );

  /// @brief Set an attribute string value
  public: void SetString( const char *key, const char *v);

  /// @brief Get a file name.
  ///
  /// Always returns an absolute path.  If the filename is entered as
  /// a relative path, we prepend the world file path.  @deprecated
  /// Use SearchFilename instead.
  public: const char *GetFilename( const char *key, const char *def, 
                                   int require = 0);

  /// @brief Search for a filename across multiple paths.
  /// @param key Key
  /// @param path Additional search path (may be NULL).
  /// @param def Default value
  /// @param require Set to 1 if this is a required field (i.e., no default value).
  /// For relative filenames, the search path is: current dir,
  /// worldfile dir, dir given by path argument.
  public: const char *SearchFilename( const char *key, const char *path, const char *def,
                                      int require = 0);

  /// @brief Get an integer
  public: int GetInt( const char *key, int def, int require = 0 );

  /// @brief Get a double
  public: double GetDouble( const char *key, double def, int require = 0 );

  /// @brief Get a boolean
  public: bool GetBool( const char *key, bool def, int require = 0 );

  /// @brief Get an attribute length value (return value in meters)
  public: double GetLength( const char *key, double def, int require = 0 );

  /// @brief Get an attribute angle value (return value in radians)
  public: double GetAngle( const char *key, double def, int require = 0 );

  /// @brief Get an attribute time value (return value in seconds)
  public: double GetTime( const char *key, double def, int require = 0 );

  /// @brief Read a position
  public: GzVector GetPosition( const char *key, GzVector def );

  /// @brief Write a position
  public: void SetPosition( const char *key, GzVector v );

  /// @brief Read a rotation
  public: GzQuatern GetRotation( const char *key, GzQuatern def );

  /// @brief Write a rotation
  public: void SetRotation( const char *key, GzQuatern v );

  /* REMOVE
  // @brief Read a pose
  //
  // Poses are specfied with three vectors: @htmlonly<lookat>pos_x
  // pos_y pos_z at_x at_y at_z up_x up_y up_z</lookat>@endhtmlonly.
  // The object will be positioned at @c pos, with the x-axis pointing
  // at @c at, and the z-axis pointing in the @c up direction.  The @c
  // up vector may be omitted.
  public: GzPose GetPose( const char *key, GzPose def );
  */

  /// @brief Read a color
  public: GzColor GetColor( const char *key, GzColor def );

  /// @brief Get an attribute tuple value
  public: const char *GetTupleString( const char *key, int index, 
                                      const char *def );

  /// @brief Get an attribute tuple double value
  public: double GetTupleDouble( const char *key, int index, double def );

  /// @brief Get an attribute tuple int value
  public: int GetTupleInt( const char *key, int index, int def );

  /// @brief Get an attribute tuple length value (return value in meters)
  public: double GetTupleLength( const char *key, int index, double def );

  /// @brief Get an attribute tuple angle value (return value in radians)
  public: double GetTupleAngle( const char *key, int index, double def );

  // Our document
  private: WorldFile *file;
  
  // Our parent
  private: WorldFileNode *parent;
  
  // Our siblings
  private: WorldFileNode *next, *prev;
  
  // Our children
  private: WorldFileNode *child_first, *child_last;

  // XML data
  private: xmlNodePtr xmlNode;

  // XML data
  private: xmlDocPtr xmlDoc;

  // Flag set if the node has been accessed
  private: bool used;

  // Warts
  friend class WorldFile;
};

#endif

