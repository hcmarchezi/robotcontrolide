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
 * Author: Andrew Howard, Nate Koenig
 * Date: 15 Nov 2001
 * CVS info: $Id: WorldFile.cc,v 1.29 2004/11/14 07:39:04 inspectorg Exp $
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "replace.h"
#include "Error.hh"
#include "WorldFile.hh"


////////////////////////////////////////////////////////////////////////////
// Standard constructor
WorldFile::WorldFile()
{
  this->filename = NULL;
  this->root = NULL;
  return;
}


////////////////////////////////////////////////////////////////////////////
// Standard destructor
WorldFile::~WorldFile()
{
  if (this->root)
    delete this->root;
  
  if (this->filename)
    free(this->filename);

  return;
}


////////////////////////////////////////////////////////////////////////////
// Load world from file
int WorldFile::Load( const char *filename )
{
  assert( this->filename == NULL );
  this->filename = strdup( filename );

  // Enable line numbering
  xmlLineNumbersDefault( 1 );

  // Load the file
  this->xmlDoc = xmlParseFile( this->filename );
  if (xmlDoc == NULL)
  {
    PRINT_ERR1( "unable to parse [%s]", this->filename );
    return -1;
  }

  // Create wrappers for all the nodes (recursive)
  this->root = this->CreateNodes( NULL, xmlDocGetRootElement(this->xmlDoc) );
  if (this->root == NULL)
  {
    PRINT_ERR1( "empty document [%s]", this->filename );
    return -1;
  }

  return 0;
}


////////////////////////////////////////////////////////////////////////////
// Save world back into file
// Set filename to NULL to save back into the original file
int WorldFile::Save( const char *filename )
{
  if (filename == NULL)
    filename = this->filename;
    
  if (xmlSaveFile(filename, this->xmlDoc) < 0)
  {
    PRINT_ERR1("unable to save [%s]", filename);
    return -1;
  }
  
  return 0;
}


////////////////////////////////////////////////////////////////////////////
// Print a warning message with unused attributes (if any).
bool WorldFile::WarnUnused()
{
  return this->root->WarnUnused();
}


////////////////////////////////////////////////////////////////////////////
// Load world from a string
int WorldFile::LoadString( const char *str )
{
  // Enable line numbering
  xmlLineNumbersDefault( 1 );

  // Load the file
  this->xmlDoc = xmlParseDoc( (xmlChar*)(str) );
  if (xmlDoc == NULL)
  {
    PRINT_ERR1( "unable to parse [%s]", str );
    return -1;
  }

  // Create wrappers for all the nodes (recursive)
  this->root = this->CreateNodes( NULL, xmlDocGetRootElement(this->xmlDoc) );
  if (this->root == NULL)
  {
    PRINT_ERR1( "empty document [%s]", str );
    return -1;
  }

  return 0;
}


////////////////////////////////////////////////////////////////////////////
// Get the root node
WorldFileNode *WorldFile::GetRootNode() const
{
  return this->root;
}


////////////////////////////////////////////////////////////////////////////
// Create wrappers
WorldFileNode *WorldFile::CreateNodes( WorldFileNode *parent, 
                                       xmlNodePtr xmlNode )
{
  WorldFileNode *self = NULL;

  // No need to create wrappers around text and blank nodes
  if( !xmlNodeIsText( xmlNode ) && !xmlIsBlankNode( xmlNode )
      && xmlNode->type != XML_COMMENT_NODE )
  {

    // Create a new node
    self = new WorldFileNode(this, parent, xmlNode,xmlDoc);

    // Create our children
    for ( xmlNode = xmlNode->xmlChildrenNode; xmlNode != NULL; 
         xmlNode = xmlNode->next)
      this->CreateNodes( self, xmlNode );
  }

  return self;
}



////////////////////////////////////////////////////////////////////////////
// Constructor
WorldFileNode::WorldFileNode( WorldFile *file, WorldFileNode *parent, 
                              xmlNodePtr xmlNode, xmlDocPtr xmlDoc )
{
  this->file = file;

  this->parent = parent;
  this->prev = NULL;
  this->next = NULL;
  this->child_first = NULL;
  this->child_last = NULL;

  // Link ourself to our parent
  if (this->parent)
  {
    if (!this->parent->child_first)
      this->parent->child_first = this;

    this->prev = this->parent->child_last;
    this->parent->child_last = this;
  }

  // Link our self to our siblings
  if (this->prev)
    this->prev->next = this;

  this->xmlNode = xmlNode;
  this->xmlDoc = xmlDoc;

  this->used = false;

  return;
}


////////////////////////////////////////////////////////////////////////////
// Destructor
WorldFileNode::~WorldFileNode()
{
  // Delete all our children first
  if (this->child_first)
    delete this->child_first;
    
  //assert(this->child_first == NULL);
  //assert(this->child_last == NULL);

  // Unlink ourself from our siblings
  if (this->prev)
    this->prev->next = this->next;

  if (this->next)
    this->next->prev = this->prev;

  // Unlink ourself from our parent
  if (this->parent)
  {
    if (this->parent->child_first == this)
      this->parent->child_first = this->next;

    if (this->parent->child_last == this)
      this->parent->child_last = this->prev;
  }

  /*  if(this->contentStr)
      xmlFree(this->contentStr);
      this->contentStr=NULL;
      */

  return;
}


////////////////////////////////////////////////////////////////////////////
// Get the node name
const char *WorldFileNode::GetName()
{
  return (const char*) this->xmlNode->name;
}

////////////////////////////////////////////////////////////////////////////
// Get the Name Space Prefix
const char *WorldFileNode::GetNSPrefix()
{
  if( !this->xmlNode->ns )
    return NULL;

  this->used = true;
  return (const char*) this->xmlNode->ns->prefix;
}

////////////////////////////////////////////////////////////////////////////
// Get the next sibling of this node
WorldFileNode *WorldFileNode::GetNext()
{
  return this->next;
}


////////////////////////////////////////////////////////////////////////////
// Get the first child of this node
WorldFileNode *WorldFileNode::GetChild()
{
  return this->child_first;
}


////////////////////////////////////////////////////////////////////////////
// Get a child based on a name. Returns null if not found
WorldFileNode *WorldFileNode::GetChild( const char *name )
{
  WorldFileNode *tmp;
  for (tmp = this->child_first; tmp != NULL && 
      strcmp( (const char*)tmp->xmlNode->name,name)!=0; tmp = tmp->GetNext() );

  return tmp;
}

WorldFileNode *WorldFileNode::GetChildByNSPrefix( const char *prefix )
{
  WorldFileNode *tmp;

  for (tmp = this->child_first; tmp != NULL; tmp = tmp->GetNext() )
  {
    if (tmp->xmlNode->ns && strcmp( (const char*)tmp->xmlNode->ns->prefix,prefix)==0)
      break;
  }

  return tmp;
}

////////////////////////////////////////////////////////////////////////////
// Print (for debugging purposes)
void WorldFileNode::Print()
{
  WorldFileNode *node;
  //xmlChar *value;

  printf( "name = [%s]\n", (const char*) this->xmlNode->name );

  //value = xmlNodeListGetString(this->doc, this->node, 1);
  //if (value)
  //  printf("value = [%s]\n", (const char*) value);
  /*if( this->xmlNode->content != NULL)
    printf("content = [%s]\n", (const char*) this->xmlNode->content);
  */

  printf( "id = [%s]\n", xmlGetProp(this->xmlNode, (xmlChar*) "id") );

  // Recurse
  for (node = this->child_first; node != NULL; node = node->next)
    node->Print();

  return;
}


////////////////////////////////////////////////////////////////////////////
// Print a warning message with unused attributes (if any).
bool WorldFileNode::WarnUnused()
{
  bool unused;
  WorldFileNode *node;  

  unused = false;
  
  if (!this->used)
  {
    PRINT_WARN3("in %s:%d unused attribute [%s]",
                this->file->filename, (int) xmlGetLineNo(this->xmlNode), this->GetName());
    unused = true;
  }

  // Call child nodes recursively
  for (node = this->child_first; node != NULL; node = node->next)
    unused |= node->WarnUnused();
  
  return unused;
}


////////////////////////////////////////////////////////////////////////////
// Get a value associated with a node.
char *WorldFileNode::GetNodeValue( const char *key )
{
  xmlChar *value=NULL;

  // First check if the key is an attribute
  if (xmlHasProp( this->xmlNode, (xmlChar*) key ))
  {
    assert(false);
    value = xmlGetProp( this->xmlNode, (xmlChar*) key );
    this->used = true;
  }

  else if (strcmp( this->GetName(), key)==0)
  {
    value = xmlNodeGetContent(this->xmlNode);
    this->used = true;
  }

  // If not an attribute, then it should be a child node
  else
  {
    WorldFileNode *currNode;

    // Loop through children
    for (currNode = this->child_first; currNode; currNode = currNode->next)
    {
      // If the name matches, then return its value
      if (strcmp( currNode->GetName(), key ) == 0)
      {
        value = xmlNodeGetContent( currNode->xmlNode );
        currNode->used = true;
        break;
      }
    }
  }
  
  return (char*) value;
}


////////////////////////////////////////////////////////////////////////////
// Set a value associated with a node.
void WorldFileNode::SetNodeValue( const char *key, const char *value )
{
  // First check if the key is an attribute
  if (xmlHasProp( this->xmlNode, (xmlChar*) key ))
  {
    //printf("set attr : %s\n", key);
    xmlSetProp( this->xmlNode, (xmlChar*) key, (xmlChar*) value );
  }

  // If not an attribute, then it should be a child node
  else
  {
    WorldFileNode *currNode;

    // Loop through children
    for (currNode = this->child_first; currNode; currNode = currNode->next)
    {
      // If the name matches, then return its value
      if (strcmp( currNode->GetName(), key ) == 0)
      {
        //printf("set child: %s\n", key);
        xmlNodeSetContent( currNode->xmlNode, (xmlChar*) value);
        break;
      }
    }
  }
  return;
}


////////////////////////////////////////////////////////////////////////////
// Get a string value.
const char *WorldFileNode::GetString( const char *key, const char *def, int require)
{
  char *value = this->GetNodeValue( key );

  if (!value && require)
  {
    PRINT_ERR3( "in %s:%d missing attribute [%s]",
        this->file->filename, (int) xmlGetLineNo(this->xmlNode), key );
    return NULL;
  }
  else if( !value )
    return def;

  // TODO: cache the value somewhere (currently leaks)
  return (const char*) value;
}


///////////////////////////////////////////////////////////////////////////
// Set an attribute string value
void WorldFileNode::SetString( const char *key, const char *v)
{
  this->SetNodeValue(key, v);
  
  return;
}



///////////////////////////////////////////////////////////////////////////
// Get a file name.  Always returns an absolute path.  If the filename
// is entered as a relative path, we prepend the world file path.
const char *WorldFileNode::GetFilename( const char *key, const char *def, int require)
{
  const char *filename = this->GetString( key, def, require );
  if (filename == NULL)
    return NULL;

  if (filename[0] == '/' || filename[0] == '~')
    return filename;
  
  else if (this->file->filename[0] == '/' || this->file->filename[0] == '~')
  {
    // Note that dirname() modifies the contents, so
    // we need to make a copy of the filename.
    // There's no bounds-checking, but what the heck.
    char *tmp = strdup( this->file->filename );
    char *fullpath = (char*) malloc( PATH_MAX );
    memset( fullpath, 0, PATH_MAX );
    strcat( fullpath, dirname(tmp));
    strcat( fullpath, "/" ); 
    strcat( fullpath, filename );
    assert( strlen(fullpath) + 1 < PATH_MAX );
    free( tmp );
    return fullpath;
  }
  else
  {
    // Prepend the path
    // Note that dirname() modifies the contents, so
    // we need to make a copy of the filename.
    // There's no bounds-checking, but what the heck.
    char *tmp = strdup( this->file->filename );
    char *fullpath = (char*) malloc(PATH_MAX);
    getcwd( fullpath, PATH_MAX );
    strcat( fullpath, "/" ); 
    strcat( fullpath, dirname(tmp) );
    strcat( fullpath, "/" ); 
    strcat( fullpath, filename );
    assert( strlen(fullpath) + 1 < PATH_MAX );
    free(tmp);
    return fullpath;
  }
  assert(false);
  return NULL;
}


///////////////////////////////////////////////////////////////////////////
// Search for a filename across multiple paths.  
const char *WorldFileNode::SearchFilename( const char *key, const char *path, const char *def,
                                           int require)
{
  struct stat stats;
  char *fullname, *tmp, *dir;
  
  // Get the filename
  const char *filename = this->GetString( key, def, require );
  if (filename == NULL)
    return NULL;

  // Absolute filename; do nothing
  if (filename[0] == '/' || filename[0] == '~')
    return filename;

  fullname = (char*) malloc(PATH_MAX);

  // Look for file using current dir
  tmp = (char*) malloc(PATH_MAX);
  getcwd(tmp, PATH_MAX);
  snprintf(fullname, PATH_MAX, "%s/%s", tmp, filename);
  free(tmp);
  
  if (stat(filename, &stats) == 0)
    return fullname;

  // Look for file using world file dir
  tmp = strdup( this->file->filename );
  dir = dirname(tmp);
  snprintf(fullname, PATH_MAX, "%s/%s", dir, filename);
  free(tmp);
  
  if (stat(fullname, &stats) == 0)
    return fullname;

  if (path)
  {
    // Look for file using given path
    snprintf(fullname, PATH_MAX, "%s/%s", path, filename);  
    if (stat(fullname, &stats) == 0)
      return fullname;
  }

  free(fullname);

  // Couldnt find the file
  PRINT_WARN1("file not found [%s]", filename);
  
  return def;
}


////////////////////////////////////////////////////////////////////////////
// Get an integer
int WorldFileNode::GetInt( const char *key, int def, int require )
{
  char *value = this->GetNodeValue( key );

  //printf("node [%s] [%s]\n", key, value);
  
  if (!value && require)
  {
    PRINT_ERR3( "in %s:%d missing attribute [%s]",
        this->file->filename, (int) xmlGetLineNo(this->xmlNode), key );
    return -1;
  }
  else if( !value )
    return def;

  return atoi((const char*) value);
}


////////////////////////////////////////////////////////////////////////////
// Get a double
double WorldFileNode::GetDouble( const char *key, double def, int require )
{
  char *value = this->GetNodeValue( key );

  //printf("node [%s] [%s]\n", key, value);
  
  if (!value && require)
  {
    PRINT_ERR3( "in %s:%d missing attribute [%s]",
        this->file->filename, (int) xmlGetLineNo(this->xmlNode), key );
    return -1;
  }
  else if( !value )
    return def;

  return atof((const char*) value);
}

////////////////////////////////////////////////////////////////////////////
// Get a boolean
bool WorldFileNode::GetBool( const char *key, bool def, int require )
{
  char *value = this->GetNodeValue( key );

  //printf("node [%s] [%s]\n", key, value);
  
  if (!value && require)
  {
    PRINT_ERR3( "in %s:%d missing attribute [%s]",
        this->file->filename, (int) xmlGetLineNo(this->xmlNode), key );
    return -1;
  }
  else if( !value )
    return def;

  if (strcmp((const char*) value, "true") == 0)
    return 1;
  
  return atoi((const char*) value);
}


////////////////////////////////////////////////////////////////////////////
// Get a length
double WorldFileNode::GetLength( const char *key, double def, int require )
{
  double length = this->GetDouble(key, def, require);

  // Do unit conversion here
  
  return length;
}


////////////////////////////////////////////////////////////////////////////
// Get an attribute angle value (return value in radians)
double WorldFileNode::GetAngle( const char *key, double def, int require )
{
  return this->GetDouble(key, def * 180 / M_PI, require) * M_PI / 180;
}


////////////////////////////////////////////////////////////////////////////
// Get a time
double WorldFileNode::GetTime( const char *key, double def, int require )
{
  double time = this->GetDouble(key, def, require);

  // Do unit conversion here
  
  return time;
}


////////////////////////////////////////////////////////////////////////////
// Read a position
GzVector WorldFileNode::GetPosition( const char *key, GzVector def )
{
  GzVector v;

  if (this->GetTupleString(key, 0, NULL) == NULL)
    return def;

  v.x = this->GetTupleLength( key, 0, 0.0 );
  v.y = this->GetTupleLength( key, 1, 0.0 );
  v.z = this->GetTupleLength( key, 2, 0.0 );

  return v;
}


////////////////////////////////////////////////////////////////////////////
// Write a position
void WorldFileNode::SetPosition( const char *key, GzVector v )
{
  char buffer[256];
  
  snprintf(buffer, sizeof(buffer), "%.3f %.3f %.3f", v.x, v.y, v.z);
  this->SetString(key, buffer);
  
  return;
}


////////////////////////////////////////////////////////////////////////////
// Read a rotation
GzQuatern WorldFileNode::GetRotation( const char *key, GzQuatern def )
{
  double px, py, pz;

  if (this->GetTupleString(key, 0, NULL) == NULL)
    return def;

  px = this->GetTupleAngle( key, 0, 0.0 );
  py = this->GetTupleAngle( key, 1, 0.0 );
  pz = this->GetTupleAngle( key, 2, 0.0 );
  GzQuatern q = GzQuaternFromEuler( px, py, pz );

  return q;
}


////////////////////////////////////////////////////////////////////////////
// Write a rotation
void WorldFileNode::SetRotation( const char *key, GzQuatern v )
{
  char buffer[256];
  GzVector e;

  e = GzQuaternToEuler(v);
  e.x *= 180 / M_PI;
  e.y *= 180 / M_PI;
  e.z *= 180 / M_PI;
  
  snprintf(buffer, sizeof(buffer), "%.0f %.0f %.0f", e.x, e.y, e.z);
  this->SetString(key, buffer);
  
  return;
}


/* REMOVE
////////////////////////////////////////////////////////////////////////////
// Read a pose
GzPose WorldFileNode::GetPose( const char *key, GzPose def )
{
  GzVector pos, at, up;
  GzPose p;
  
  if (this->GetTupleString(key, 0, NULL) == NULL)
    return def;

  // Determine pose using GLU lookat method
  pos = GzVectorSet(this->GetTupleLength(key, 0, 0.0),
                    this->GetTupleLength(key, 1, 0.0),
                    this->GetTupleLength(key, 2, 0.0));
  at = GzVectorSet(this->GetTupleLength(key, 3, 0.0),
                   this->GetTupleLength(key, 4, 0.0),
                   this->GetTupleLength(key, 5, 0.0));
  up = GzVectorSet(this->GetTupleLength(key, 6, 0.0),
                   this->GetTupleLength(key, 7, 0.0),
                   this->GetTupleLength(key, 8, 1.0));

  p = GzPosePointAt(pos, at, up);
  
  return p;
}
*/


////////////////////////////////////////////////////////////////////////////
// Read a color (returns color as a 3-vector)
GzColor WorldFileNode::GetColor( const char *key, GzColor def )
{
  GzColor c;

  if (this->GetString( key, NULL ) == NULL)
    return def;
  
  c.r = this->GetTupleDouble( key, 0, 0.0 );
  c.g = this->GetTupleDouble( key, 1, 0.0 );
  c.b = this->GetTupleDouble( key, 2, 0.0 );
  c.a = this->GetTupleDouble( key, 3, 1.0 );
  
  return c;
}


////////////////////////////////////////////////////////////////////////////
// Get a tuple string value.
const char *WorldFileNode::GetTupleString( const char *key, int index, 
                                           const char *def)
{
  char *value;
  char *nvalue = NULL;
  int i, a, b, state, count;

  value = this->GetNodeValue( key );

  if (value == NULL)
    return def;

  state = 0;
  count = 0;
  a = b = 0;

  for (i = 0; i < (int) strlen((const char*) value); i++)
  {
    // Look for start of element
    if (state == 0)
    {
      if (!isspace( value[i] ))
      {
        a = i;
        state = 1;
      }
    }

    // Look for end of element
    else if (state == 1)
    {
      if (isspace( value[i] ))
      {
        state = 0;
        b = i - 1;
        count++;
        if (count > index)
          break;
      }
    }
  }

  if (state == 1)
  {
    b = i - 1;
    count++;
  }

  if (count == index + 1)
    nvalue = strndup( (const char*) value + a, b - a + 2 );
  else
    nvalue = NULL;

  //printf( "tuple [%s] [%d] [%s] [%s] [%d %d]\n", key, index, value, nvalue, a, b );

  xmlFree( value );

  return nvalue;
}


////////////////////////////////////////////////////////////////////////////
// Get an attribute tuple double value
double WorldFileNode::GetTupleDouble( const char *key, int index, double def )
{
  const char *svalue;

  svalue = this->GetTupleString( key, index, NULL );
  if (svalue == NULL)
    return def;

  return atof(svalue);
}

////////////////////////////////////////////////////////////////////////////
// Get an attribute tuple int value
int WorldFileNode::GetTupleInt( const char *key, int index, int def )
{
  const char *svalue;

  svalue = this->GetTupleString( key, index, NULL );
  if (svalue == NULL)
    return def;

  return atoi(svalue);
}

////////////////////////////////////////////////////////////////////////////
// Get an tuple length value (return value in meters)
double WorldFileNode::GetTupleLength( const char *key, int index, double def )
{
  const char *svalue;

  svalue = this->GetTupleString( key, index, NULL );
  if (svalue == NULL)
    return def;

  // TODO: unit conversion here
  
  return atof(svalue);
}


////////////////////////////////////////////////////////////////////////////
// Get an tuple angle value (return value in radians)
double WorldFileNode::GetTupleAngle( const char *key, int index, double def )
{
  const char *svalue;

  svalue = this->GetTupleString( key, index, NULL );
  if (svalue == NULL)
    return def;

  return atof(svalue) * M_PI / 180;
}



