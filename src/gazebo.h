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

/* Desc: External interfaces for Gazebo
 * Author: Andrew Howard
 * Date: 6 Apr 2003
 * CVS: $Id: gazebo.h,v 1.87 2006/11/02 17:06:31 natepak Exp $
 */

#ifndef GAZEBO_H
#define GAZEBO_H

#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup libgazebo libgazebo

See @ref libgazebo_usage for information on using libgazebo.

*/

/***************************************************************************
 * Constants, etc
 **************************************************************************/

/// @addtogroup libgazebo 
/// @{ 

/// Interface version number
#define LIBGAZEBO_VERSION 0x060

/// @}
    
/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @defgroup utility Error-handling
@{
*/
/***************************************************************************/

/** @brief @internal Initialize error logging
    @param print Set to 0 to stop messages being printed to stderr.
    @param level Debug level: 0 = important messages, 1 = useful
    messages, 2+ = all messages.
*/
void gz_error_init(int print, int level);
  
/** Retrieve the last error (as a descriptive string).  Most functions
    in will return 0 on success and non-zero value on error; a
    descriptive error message can be obtained by calling this
    function. */
const char *gz_error_str(void);

/** @}*/
/// @}
/***************************************************************************/


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup server Server object

The server object is used by the Gazebo server to establish and
maintain connections with clients.

@internal

@{
*/

/// @brief Server data
typedef struct gz_server
{
  /// The server id
  int server_id;

  /// The directory containing mmap files
  char *filename;

  /// The semphore key and id
  int sem_key, sem_cid;
    
} gz_server_t;


/// @brief Create a new server
extern gz_server_t *gz_server_alloc();

/// @brief Destroy a server
extern void gz_server_free(gz_server_t *self);

/// @brief Initialize the server
extern int gz_server_init(gz_server_t *self, int server_id, int force);

/// @brief Finalize the server
extern int gz_server_fini(gz_server_t *self);

/// @brief Tell clients that new data is available
extern int gz_server_post(gz_server_t *self);

/** @}*/
/// @}
/***************************************************************************/

  
/***************************************************************************/
/// @addtogroup libgazebo 
/// @{ 
/** @defgroup client Client object

The client object is used by Gazebo clients to establish a connection
with a running server.  See the @ref libgazebo_usage for an overview.

@{
*/

/// @brief Semaphore key used by Gazebo
#define GZ_SEM_KEY 0x135135FA
  
/// @brief Reserved client IDs.
///
/// User programs may use numbers in the range GZ_SEM_NUM_USER to
/// GZ_SEM_NUM_USER_LAST, inclusive.  All other semaphore numbers are
/// reserved.
#define GZ_CLIENT_ID_USER_FIRST 0x00
#define GZ_CLIENT_ID_USER_LAST  0x07
#define GZ_CLIENT_ID_WXGAZEBO   0x08
#define GZ_CLIENT_ID_PLAYER     0x09

  
/// Client data
typedef struct gz_client
{
  /// The server id
  int server_id;

  /// The client id
  int client_id;

  /// The directory containing mmap files
  char *filename;

  /// The semphore key and id
  int sem_key, sem_cid;

} gz_client_t;


/// Create a new client
extern gz_client_t *gz_client_alloc();

/// Destroy a client
extern void gz_client_free(gz_client_t *self);

/// Test for the presence of the server.
/// @returns The return value is 0 if the server is present; +1 if
/// the server is not present; -1 if there is an error.
extern int gz_client_query(gz_client_t *self, int server_id);

/// Connect to the server (non-blocking mode).
extern int gz_client_connect(gz_client_t *self, int server_id);

/// @brief Connect to the server (blocking mode).
/// @param self Pointer to Itself.
/// @param server_id Server ID; each server must have a unique id.
/// @param client_id Client ID; in blocking mode, each client must have a unique id.
extern int gz_client_connect_wait(gz_client_t *self, int server_id, int client_id);

/// Disconnect from the server
extern int gz_client_disconnect(gz_client_t *self);

/// @brief Wait for new data to be posted (blocking mode).
/// @returns Returns 0 on success, -1 on error.
extern int gz_client_wait(gz_client_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @internal @defgroup data Common data structures

All data structures share this common structure.

@{
*/

/// Max length of model type string 
#define GAZEBO_MAX_MODEL_TYPE 128

/// Common data header
typedef struct gz_data
{
  /// Interface version number
  int version;

  /// Allocation size
  size_t size;

  /// Type of model that owns this interface
  char model_type[GAZEBO_MAX_MODEL_TYPE];
  
  /// ID of the model that owns this interface
  int model_id;

  /// ID of the parent model
  int parent_model_id;
  
} gz_data_t;

/** @} */
/// @}



/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @internal @defgroup iface Common interface structures

All interfaces share this common structure.

@{
*/

typedef struct gz_iface
{
  // The server we are associated with
  gz_server_t *server;
  
  // The client we are associated with
  gz_client_t *client;
  
  // File descriptor for the mmap file
  int mmap_fd;

  // Pointer to the mmap'ed mem
  void *mmap;

  // The name of the file we created/opened
  char *filename;
  
} gz_iface_t;


// Create an interface
extern gz_iface_t *gz_iface_alloc();

// @internal Destroy an interface
extern void gz_iface_free(gz_iface_t *self);

// @internal Create the interface (used by Gazebo server)
extern int gz_iface_create(gz_iface_t *self, gz_server_t *server,
                           const char *type, const char *id, size_t size);

// @internal Destroy the interface (server)
extern int gz_iface_destroy(gz_iface_t *self);

// Open an existing interface
extern int gz_iface_open(gz_iface_t *self, gz_client_t *client,
                         const char *type, const char *id, size_t size);

// Close the interface
extern int gz_iface_close(gz_iface_t *self);

// Lock the interface.  Set blocking to 1 if the caller should block
// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_iface_lock(gz_iface_t *self, int blocking);

// Unlock the interface
extern void gz_iface_unlock(gz_iface_t *self);

// Tell clients that new data is available
extern int gz_iface_post(gz_iface_t *self);


/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @defgroup simulator simulator 

The simulator interface provides access to certain global properties
of the server, such as the current simulation time-step. 

@{
*/

/// Common simulator data
typedef struct gz_sim_data
{
  /// Common data structure
  gz_data_t head;
  
  /// Elapsed simulator time
  double sim_time;

  /// Accumpated pause time (this interface may be updated with the
  /// server is paused).
  double pause_time;

  // Elapsed real time since start of simulation (from system clock). 
  double real_time;

  /// Pause simulation (set by client)
  int pause;

  /// Reset simulation (set by client)
  int reset;

  /// Save current poses to world file (set by client)
  int save;

} gz_sim_data_t;


/// Simulator interface
typedef struct gz_sim
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_sim_data_t *data;

} gz_sim_t;


/// Create an interface
extern gz_sim_t *gz_sim_alloc();

/// Destroy an interface
extern void gz_sim_free(gz_sim_t *self);

/// @internal Create the interface (used by Gazebo server)
/// @param self Point to itself.
/// @param server Pointer to the server.
/// @param id Dummy id; set this to "default".
extern int gz_sim_create(gz_sim_t *self, gz_server_t *server, const char *id);

/// @internal Destroy the interface (server)
extern int gz_sim_destroy(gz_sim_t *self);

/// @brief Open an existing interface.
/// @param self Pointer to itself.
/// @param client Pointer to the client.
/// @param id Dummy id; set this to "default"
extern int gz_sim_open(gz_sim_t *self, gz_client_t *client, const char *id);

/// Close the interface
extern int gz_sim_close(gz_sim_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_sim_lock(gz_sim_t *self, int blocking);

/// Unlock the interface
extern void gz_sim_unlock(gz_sim_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @defgroup camera camera 

The camera interface allows clients to read images from a simulated
camera.  This interface gives the view of the world as the camera
would see it.

Images are in packed RGB888 format.

@{
*/

/// Maximum image pixels (width x height)
#define GAZEBO_CAMERA_MAX_IMAGE_SIZE 640 * 480 * 3

/// Camera interface data
typedef struct gz_camera_data
{
  /// Common data structure
  gz_data_t head;

  /// Data timestamp
  double time;

  /// Image dimensions (in pixels)
  unsigned int width, height;

  /// Image pixel data
  unsigned int image_size;
  unsigned char image[GAZEBO_CAMERA_MAX_IMAGE_SIZE];
  
} gz_camera_data_t;


/// The camera interface
typedef struct gz_camera
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_camera_data_t *data;
  
} gz_camera_t;


/// Create an interface
extern gz_camera_t *gz_camera_alloc();

/// Destroy an interface
extern void gz_camera_free(gz_camera_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_camera_create(gz_camera_t *self, gz_server_t *server, const char *id,
                            const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_camera_destroy(gz_camera_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_camera_open(gz_camera_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_camera_close(gz_camera_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_camera_lock(gz_camera_t *self, int blocking);

/// Unlock the interface
extern void gz_camera_unlock(gz_camera_t *self);

/// Tell clients that new data is available
extern int gz_camera_post(gz_camera_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @defgroup factory factory

The factory interface allows clients to send XML strings to a factory
in order to dynamically create models.

@{
*/

/// Factory interface
typedef struct gz_factory_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;

  /// String describing the model to be instantiated
  uint8_t string[4096];
  
} gz_factory_data_t;


/// The position interface
typedef struct gz_factory
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_factory_data_t *data;
  
} gz_factory_t;


/// Create an interface
extern gz_factory_t *gz_factory_alloc();

/// Destroy an interface
extern void gz_factory_free(gz_factory_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_factory_create(gz_factory_t *self, gz_server_t *server, const char *id,
                             const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_factory_destroy(gz_factory_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_factory_open(gz_factory_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_factory_close(gz_factory_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_factory_lock(gz_factory_t *self, int blocking);

/// Unlock the interface
extern void gz_factory_unlock(gz_factory_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @defgroup fiducial fiducial

The fiducial interface allows clients to determine the identity,
range, bearing and orientation (relative to some sensor) of objects in
the world.  For example, this interface is used by the SickLMS200
model to return data from simulated retro-reflective barcodes.

@{
*/

#define GZ_FIDUCIAL_MAX_FIDS 401

/// Data for a single fiducial
typedef struct gz_fiducial_fid
{
  /// Fiducial id
  int id;

  /// Fiducial position relative to sensor (x, y, z).
  double pos[3];

  /// Fiducial orientation relative to sensor (roll, pitch, yaw).
  double rot[3];

} gz_fiducial_fid_t;


/// Fiducial data
typedef struct gz_fiducial_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;
  
  /// Observed fiducials
  int fid_count;
  gz_fiducial_fid_t fids[GZ_FIDUCIAL_MAX_FIDS];
  
} gz_fiducial_data_t;


/// The fiducial interface
typedef struct gz_fiducial
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_fiducial_data_t *data;
  
} gz_fiducial_t;


/// Create an interface
extern gz_fiducial_t *gz_fiducial_alloc();

/// Destroy an interface
extern void gz_fiducial_free(gz_fiducial_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_fiducial_create(gz_fiducial_t *self, gz_server_t *server, const char *id,
                              const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_fiducial_destroy(gz_fiducial_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_fiducial_open(gz_fiducial_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_fiducial_close(gz_fiducial_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_fiducial_lock(gz_fiducial_t *self, int blocking);

/// Unlock the interface
extern void gz_fiducial_unlock(gz_fiducial_t *self);

/// Tell clients that new data is available
extern int gz_fiducial_post(gz_fiducial_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @defgroup guicam guicam 

The GuiCam interface provides access to user-controllable cameras
(i.e., a gods-eye view for monitoring a running simulation).  This interface
is used by monitoring GUIs such as wxgazebo.

@{
*/

/// Constants
#define GAZEBO_GUICAM_MAX_IMAGE_SIZE 1024 * 768 * 3

/// Guicam interface data
typedef struct gz_guicam_data
{
  /// Common data structure
  gz_data_t head;

  /// Data timestamp (simulator time)
  double sim_time;

  /// Accumpated pause time (this interface may be updated with the
  /// server is paused).
  double pause_time;
  
  /// True object position (x, y, x)
  double pos[3];

  /// True object rotation (roll, pitch, yaw)
  double rot[3];

  /// Spot mode (0 = translate; 1 = zoom; 2 = rotate)
  int cmd_spot_mode;
  
  /// Spot state (0 = none; 1 = moving)
  int cmd_spot_state;
  
  /// Spot transform points (image coordinates)
  double cmd_spot_a[2], cmd_spot_b[2];
  
  /// Lock the roll (so that the image y axis corresponds to world z
  /// axis)
  int roll_lock;

  /// Display bounding boxes
  int display_bbox;

  /// Display body axes
  int display_axes;

  /// Display CoM axes
  int display_com;

  /// Display skins
  int display_skins;

  /// Display sensor rays
  int display_rays;

  /// Display sensor frustrums
  int display_frustrums;

  /// Display materials
  int display_materials;

  /// Display textures
  int display_textures;

  /// Polygon mode: 0 = wireframe, 1 = filled
  int polygon_fill;

  /// Shade model: 0 = flat, 1 = smooth
  int shade_smooth;

  /// Save frames to disk
  int save_frames;

  /// Image dimensions (in pixels)
  unsigned int width, height;

  /// Image pixel data; format is packed RGB888
  unsigned char image[GAZEBO_GUICAM_MAX_IMAGE_SIZE];
  unsigned int image_size;
  
} gz_guicam_data_t;


/// The guicam interface
typedef struct gz_guicam
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_guicam_data_t *data;
  
} gz_guicam_t;


/// Create an interface
extern gz_guicam_t *gz_guicam_alloc();

/// Destroy an interface
extern void gz_guicam_free(gz_guicam_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_guicam_create(gz_guicam_t *self, gz_server_t *server, const char *id,
                            const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_guicam_destroy(gz_guicam_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_guicam_open(gz_guicam_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_guicam_close(gz_guicam_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_guicam_lock(gz_guicam_t *self, int blocking);

/// Unlock the interface
extern void gz_guicam_unlock(gz_guicam_t *self);

/// Tell clients that new data is available
extern int gz_guicam_post(gz_guicam_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @defgroup gps gps

The gps interface allows the user to receive GPS
(Latitude/Longitude/Altitude) information for the robot platform on
which the GPS device is mounted.

@{
*/

/// GPS interface
typedef struct gz_gps_data
{
  /// Common data structure
  gz_data_t head;
  
  /// Data timestamp
  double time;

  /// UTC timestamp
  double utc_time;

  /// Latitude in degrees.
  double latitude;

  /// Longitude in degrees.
  double longitude;

  /// Altitude, in meters.
  double altitude;

  /// UTM coordinates (meters)
  double utm_e, utm_n;

  /// Number of satellites in view.
  int satellites;

  /// Fix quality
  int quality;
    
  /// Horizontal dilution of position (HDOP)
  double hdop;

  /// Vertical dilution of position (VDOP)
  double vdop;

  /// Errors
  double err_horz, err_vert;
  
} gz_gps_data_t;


/// The GPS interface
typedef struct gz_gps
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_gps_data_t *data;
  
} gz_gps_t;


/// Create an interface
extern gz_gps_t *gz_gps_alloc();

/// Destroy an interface
extern void gz_gps_free(gz_gps_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_gps_create(gz_gps_t *self, gz_server_t *server, const char *id,
                         const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_gps_destroy(gz_gps_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_gps_open(gz_gps_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_gps_close(gz_gps_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_gps_lock(gz_gps_t *self, int blocking);

/// Unlock the interface
extern void gz_gps_unlock(gz_gps_t *self);

/// Tell clients that new data is available
extern int gz_gps_post(gz_gps_t *self);

/** @} */
/// @}



/***************************************************************************/
/// @addtogroup libgazebo 
/// @{
/** @defgroup gripper gripper

The gripper interface allows control of a simple 2-DOF gripper, such as
that found on the Pioneer series robots.

@{
*/

/** Gripper state: open */
#define GAZEBO_GRIPPER_STATE_OPEN 1
/** Gripper state: closed */
#define GAZEBO_GRIPPER_STATE_CLOSED 2
/** Gripper state: moving */
#define GAZEBO_GRIPPER_STATE_MOVING 3
/** Gripper state: error */
#define GAZEBO_GRIPPER_STATE_ERROR 4

/** Gripper command: open */
#define GAZEBO_GRIPPER_CMD_OPEN 1
/** Gripper command: close */
#define GAZEBO_GRIPPER_CMD_CLOSE 2
/** Gripper command: stop */
#define GAZEBO_GRIPPER_CMD_STOP 3
/** Gripper command: store */
#define GAZEBO_GRIPPER_CMD_STORE 4
/** Gripper command: retrieve */
#define GAZEBO_GRIPPER_CMD_RETRIEVE 5

/// Gripper data interface
typedef struct gz_gripper_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;
 
  /// Current command for the gripper
  int cmd;

  /// Current state of the gripper
  int state;

  int grip_limit_reach;
  int lift_limit_reach;
  int outer_beam_obstruct;
  int inner_beam_obstruct;
  int left_paddle_open;
  int right_paddle_open;

  int lift_up;
  int lift_down;

} gz_gripper_data_t;


/// The gripper interface
typedef struct gz_gripper
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_gripper_data_t *data;
  
} gz_gripper_t;


/// Create an interface
extern gz_gripper_t *gz_gripper_alloc();

/// Destroy an interface
extern void gz_gripper_free(gz_gripper_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_gripper_create(gz_gripper_t *self, gz_server_t *server, const char *id,
                             const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_gripper_destroy(gz_gripper_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_gripper_open(gz_gripper_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_gripper_close(gz_gripper_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_gripper_lock(gz_gripper_t *self, int blocking);

/// Unlock the interface
extern void gz_gripper_unlock(gz_gripper_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup laser laser

The laser interface allows clients to read data from a simulated laser
range finder (such as the SICK LMS200).  Some configuration of this
device is also allowed.

@{
*/

#define GZ_LASER_MAX_RANGES 401

/// Laser data
typedef struct gz_laser_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;
  
  /// Range scan angles
  double min_angle, max_angle;

  /// Angular resolution
  double res_angle;

  /// Max range value
  double max_range;

  /// Number of range readings
  int range_count;
  
  /// Range readings
  double ranges[GZ_LASER_MAX_RANGES];

  /// Intensity readings
  int intensity[GZ_LASER_MAX_RANGES];
  
  // New command ( 0 or 1 )
  int cmd_new_angle;
  int cmd_new_length;

  // Commanded range value
  double cmd_max_range;
  double cmd_min_angle, cmd_max_angle;
  int cmd_range_count;

} gz_laser_data_t;


/// The laser interface
typedef struct gz_laser
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_laser_data_t *data;
  
} gz_laser_t;


/// Create an interface
extern gz_laser_t *gz_laser_alloc();

/// Destroy an interface
extern void gz_laser_free(gz_laser_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_laser_create(gz_laser_t *self, gz_server_t *server, const char *id,
                           const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_laser_destroy(gz_laser_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_laser_open(gz_laser_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_laser_close(gz_laser_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_laser_lock(gz_laser_t *self, int blocking);

/// Unlock the interface
extern void gz_laser_unlock(gz_laser_t *self);

/// Tell clients that new data is available
extern int gz_laser_post(gz_laser_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup position position

The position interface allows clients to send commands to and read
odometric data from simulated mobile robot bases, such as the
Pioneer2AT or ATRV Jr.  This interface handles both 2D and 3D data.

@{
*/

/// Position interface
typedef struct gz_position_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;

  /// Pose (usually global cs); rotation is specified by euler angles
  double pos[3];
  double rot[3];

  /// Velocity; rotation is specified by euler angles
  double vel_pos[3];
  double vel_rot[3];

  /// Motor stall flag
  int stall;

  /// Enable the motors
  int cmd_enable_motors;
  
  /// Commanded robot velocities (robot cs); rotation is specified by euler angles
  double cmd_vel_pos[3];
  double cmd_vel_rot[3];
  
} gz_position_data_t;


/// The position interface
typedef struct gz_position
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_position_data_t *data;
  
} gz_position_t;


/// Create an interface
extern gz_position_t *gz_position_alloc();

/// Destroy an interface
extern void gz_position_free(gz_position_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_position_create(gz_position_t *self, gz_server_t *server, const char *id,
                              const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_position_destroy(gz_position_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_position_open(gz_position_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_position_close(gz_position_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_position_lock(gz_position_t *self, int blocking);

/// Unlock the interface
extern void gz_position_unlock(gz_position_t *self);

/// Tell clients that new data is available
extern int gz_position_post(gz_position_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup power power

The power interface allows clients to read battery levels from
simulated robots.

@{
*/

/// Power interface
typedef struct gz_power_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;

  /// Battery levels (volts)
  double levels[10];
  
} gz_power_data_t;


/// The power interface
typedef struct gz_power
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_power_data_t *data;
  
} gz_power_t;


/// Create an interface
extern gz_power_t *gz_power_alloc();

/// Destroy an interface
extern void gz_power_free(gz_power_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_power_create(gz_power_t *self, gz_server_t *server, const char *id,
                           const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_power_destroy(gz_power_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_power_open(gz_power_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_power_close(gz_power_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_power_lock(gz_power_t *self, int blocking);

/// Unlock the interface
extern void gz_power_unlock(gz_power_t *self);

/// Tell clients that new data is available
extern int gz_power_post(gz_power_t *self);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup ptz ptz

The ptz interface allows clients to control the pan, tilt and zoom
angles on a camera head such as the Sony VID30.

@{
*/

/// Ptz interface
typedef struct gz_ptz_data
{
  /// Common data structure
  gz_data_t head;
  
  /// Data timestamp
  double time;

  /// Measured orientation (radians)
  double pan, tilt;

  /// Measured field of view (radians)
  double zoom;

  /// Commanded orientation (radians)
  double cmd_pan, cmd_tilt;

  /// Commanded field of view (radians)
  double cmd_zoom;
  
} gz_ptz_data_t;


/// The ptz interface
typedef struct gz_ptz
{
  /// Common data structure
  gz_data_t head;
  
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_ptz_data_t *data;
  
} gz_ptz_t;


/// Create an interface
extern gz_ptz_t *gz_ptz_alloc();

/// Destroy an interface
extern void gz_ptz_free(gz_ptz_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_ptz_create(gz_ptz_t *self, gz_server_t *server, const char *id,
                         const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_ptz_destroy(gz_ptz_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_ptz_open(gz_ptz_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_ptz_close(gz_ptz_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_ptz_lock(gz_ptz_t *self, int blocking);

/// Unlock the interface
extern void gz_ptz_unlock(gz_ptz_t *self);

/// Tell clients that new data is available
extern int gz_ptz_post(gz_ptz_t *self);


/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup actarray actarray

The actarray interface allows clients to actarray devices like robotic manipulators


@{
*/

#define GAZEBO_ACTARRAY_NUM_ACTUATORS 16
#define GAZEBO_ACTARRAY_JOINT_POSITION_MODE 0
#define GAZEBO_ACTARRAY_JOINT_SPEED_MODE 1
#define GAZEBO_ACTARRAY_JOINT_CURRENT_MODE 2

/** Idle state code */
#define GAZEBO_ACTARRAY_ACTSTATE_IDLE     1
/** Moving state code */
#define GAZEBO_ACTARRAY_ACTSTATE_MOVING   2
/** Braked state code */
#define GAZEBO_ACTARRAY_ACTSTATE_BRAKED   3
/** Stalled state code */
#define GAZEBO_ACTARRAY_ACTSTATE_STALLED  4


/** @brief Structure containing a single actuator's information */
typedef struct gazebo_actarray_actuator
{
  /** The position of the actuator in m or rad depending on the type. */
  float position;
  /** The speed of the actuator in m/s or rad/s depending on the type. */
  float speed;
  /** The current state of the actuator. */
  uint8_t state;
} gazebo_actarray_actuator_t;

//TODO: Extend this for the complete interface
/// Actarray interface
typedef struct gz_actarray_data
{
  /// Common data structure
  gz_data_t head;
  
  /// Data timestamp
  double time;
  
  // Timestamp for commands;
  unsigned int new_cmd;

  /** The number of actuators in the array. */
  uint32_t actuators_count;
  /** The actuator data and geometry. */
  gazebo_actarray_actuator_t actuators_data[GAZEBO_ACTARRAY_NUM_ACTUATORS];
  //gazebo_actarray_actuatorgeom_t actuators_geom[GAZEBO_ACTARRAY_NUM_ACTUATORS];
  /** The position of the base of the actarray. */
  //gazebo_point_3d_t base_pos;
  /** The orientation of the base of the actarray. */
  //gazebo_orientation_3d_t base_orientation;  
   float cmd_pos[GAZEBO_ACTARRAY_NUM_ACTUATORS];
   float cmd_speed[GAZEBO_ACTARRAY_NUM_ACTUATORS];
   float cmd_current[GAZEBO_ACTARRAY_NUM_ACTUATORS];
   unsigned int joint_mode[GAZEBO_ACTARRAY_NUM_ACTUATORS];  //Position/Speed/Current mode

} gz_actarray_data_t;


/// The actarray interface
typedef struct gz_actarray
{
  /// Common data structure
  gz_data_t head;
  
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_actarray_data_t *data;
  
} gz_actarray_t;


/// Create an interface
extern gz_actarray_t *gz_actarray_alloc();

/// Destroy an interface
extern void gz_actarray_free(gz_actarray_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_actarray_create(gz_actarray_t *self, gz_server_t *server, const char *id,
                         const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_actarray_destroy(gz_actarray_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_actarray_open(gz_actarray_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_actarray_close(gz_actarray_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_actarray_lock(gz_actarray_t *self, int blocking);

/// Unlock the interface
extern void gz_actarray_unlock(gz_actarray_t *self);

/// Tell clients that new data is available
extern int gz_actarray_post(gz_actarray_t *self);


/** @} */
/// @}





/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup sonar sonar

The sonar interface allows the positions and range values for sonars
to be read.

@{
*/

#define GZ_SONAR_MAX_RANGES 48

/// Sonar data interface
typedef struct gz_sonar_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;

  /// Max range value
  double max_range;

  /// Number of sonar
  int sonar_count;

  /// Position of each sonar (x, y, z)
  double sonar_pos[GZ_SONAR_MAX_RANGES][3];

  /// Orientation of each soanr (roll, pitch, yaw)
  double sonar_rot[GZ_SONAR_MAX_RANGES][3];

  /// Range readings (m)
  double sonar_ranges[GZ_SONAR_MAX_RANGES];
  
  /// Enable the sonars
  int cmd_enable_sonar;

} gz_sonar_data_t;


/// The sonar interface
typedef struct gz_sonar
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_sonar_data_t *data;

} gz_sonar_t;


/// Create an interface
extern gz_sonar_t *gz_sonar_alloc();

/// Destroy an interface
extern void gz_sonar_free(gz_sonar_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_sonar_create(gz_sonar_t *self, gz_server_t *server, const char *id,
                           const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_sonar_destroy(gz_sonar_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_sonar_open(gz_sonar_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_sonar_close(gz_sonar_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_sonar_lock(gz_sonar_t *self, int blocking);

/// Unlock the interface
extern void gz_sonar_unlock(gz_sonar_t *self);

/** @} */
/// @}
//
/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup ir ir

The IR interface allows the positions and range values for IRs
to be read.

@{
*/

#define GZ_IR_MAX_RANGES 48

/// Ir data interface
typedef struct gz_ir_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;

  /// Max range value
  double max_range;

  /// Number of ir
  int ir_count;

  /// Position of each ir (x, y, z)
  double ir_pos[GZ_IR_MAX_RANGES][3];

  /// Orientation of each ir (roll, pitch, yaw)
  double ir_rot[GZ_IR_MAX_RANGES][3];

  /// Range readings (m)
  double ir_ranges[GZ_IR_MAX_RANGES];
  
  /// Enable the IRs
  int cmd_enable_ir;

} gz_ir_data_t;


/// The ir interface
typedef struct gz_ir
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_ir_data_t *data;

} gz_ir_t;


/// Create an interface
extern gz_ir_t *gz_ir_alloc();

/// Destroy an interface
extern void gz_ir_free(gz_ir_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_ir_create(gz_ir_t *self, gz_server_t *server, const char *id,
                           const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_ir_destroy(gz_ir_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_ir_open(gz_ir_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_ir_close(gz_ir_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_ir_lock(gz_ir_t *self, int blocking);

/// Unlock the interface
extern void gz_ir_unlock(gz_ir_t *self);

/** @} */
/// @}



/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup stereo stereo 

The stereo interface allows clients to read images from a simulated
stereo cameras.  It provides regular camera images and simulated depth
images (the latter may include occlusion and texture effects).

@{
*/

/// Constants
#define GAZEBO_STEREO_MAX_RGB_SIZE 640 * 480 * 3
#define GAZEBO_STEREO_MAX_DISPARITY_SIZE 640 * 480

/// Stereo interface data
typedef struct gz_stereo_data
{
  /// Common data structure
  gz_data_t head;

  /// Data timestamp
  double time;

  /// Image dimensions (in pixels)
  unsigned int width, height;

  /// Left image (packed RGB888)
  unsigned int left_image_size;
  unsigned char left_image[GAZEBO_STEREO_MAX_RGB_SIZE];

  /// Right image (packed RGB888)
  unsigned int right_image_size;
  unsigned char right_image[GAZEBO_STEREO_MAX_RGB_SIZE];

  /// Left disparity image (packed float)
  unsigned int left_disparity_size;
  float left_disparity[GAZEBO_STEREO_MAX_DISPARITY_SIZE];

  /// Right disparity image (packed float)
  unsigned int right_disparity_size;
  float right_disparity[GAZEBO_STEREO_MAX_DISPARITY_SIZE];

} gz_stereo_data_t;


/// The stereo interface
typedef struct gz_stereo
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_stereo_data_t *data;
  
} gz_stereo_t;


/// Create an interface
extern gz_stereo_t *gz_stereo_alloc();

/// Destroy an interface
extern void gz_stereo_free(gz_stereo_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_stereo_create(gz_stereo_t *self, gz_server_t *server, const char *id,
                            const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_stereo_destroy(gz_stereo_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_stereo_open(gz_stereo_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_stereo_close(gz_stereo_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_stereo_lock(gz_stereo_t *self, int blocking);

/// Unlock the interface
extern void gz_stereo_unlock(gz_stereo_t *self);

/// Tell clients that new data is available
extern int gz_stereo_post(gz_stereo_t *self);

/** @} */
/// @}



/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup truth truth

The truth interface is useful for getting and setting the ground-truth
pose of objects in the world; currently, it is supported only by the
TruthWidget model.

@{
*/

/// Truth data
typedef struct gz_truth_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;

  /// True object position (x, y, z)
  double pos[3];

  /// True object rotation (quaternion: u,x,y,z)
  double rot[4];

  /// New command (0 or 1)?
  int cmd_new;
  
  /// Commanded object position (x, y, z)
  double cmd_pos[3];

  /// Commanded object rotation (quaternion: u,x,y,z)
  double cmd_rot[4];
   
} gz_truth_data_t;


/// The truth interface
typedef struct gz_truth
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_truth_data_t *data;
  
} gz_truth_t;


/// Create an interface
extern gz_truth_t *gz_truth_alloc();

/// Destroy an interface
extern void gz_truth_free(gz_truth_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_truth_create(gz_truth_t *self, gz_server_t *server, const char *id,
                           const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_truth_destroy(gz_truth_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_truth_open(gz_truth_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_truth_close(gz_truth_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_truth_lock(gz_truth_t *self, int blocking);

/// Unlock the interface
extern void gz_truth_unlock(gz_truth_t *self);

/// Tell clients that new data is available
extern int gz_truth_post(gz_truth_t *self);

/// Convenience function that converts to Euler(roll,pitch,yaw) from quatern
extern void gz_truth_euler_from_quatern(gz_truth_t *self, double *e, double *q);

/// Convenience function that converts to quatern from Euler(roll,pitch,yaw)
extern void gz_truth_quatern_from_euler(gz_truth_t *self, double *q, double *e);

/** @} */
/// @}


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup wifi wifi

Data on simulated WiFi link quality.

@{
*/

#define GAZEBO_WIFI_MAX_LINKS 16

/// link quality is in dBm
#define GAZEBO_WIFI_QUAL_DBM		1

/// Individual link properties
typedef struct gz_wifi_link
{
  /// IP address of destination.
  char ip[32];

  /// Link quality, level and noise information
  /// these could be uint8_t instead, <linux/wireless.h> will only
  /// return that much.  maybe some other architecture needs larger??
  uint16_t qual, level, noise;
  
} gz_wifi_link_t;


typedef struct gz_wifi_data
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;

  /// @{
  /// A list of links
  gz_wifi_link_t links[GAZEBO_WIFI_MAX_LINKS];
  uint16_t link_count;
  /// @}

} gz_wifi_data_t;


/// The truth interface
typedef struct gz_wifi
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_wifi_data_t *data;
  
} gz_wifi_t;


/// Create an interface
extern gz_wifi_t *gz_wifi_alloc();

/// Destroy an interface
extern void gz_wifi_free(gz_wifi_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_wifi_create(gz_wifi_t *self, gz_server_t *server, const char *id,
                          const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_wifi_destroy(gz_wifi_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_wifi_open(gz_wifi_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_wifi_close(gz_wifi_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_wifi_lock(gz_wifi_t *self, int blocking);

/// Unlock the interface
extern void gz_wifi_unlock(gz_wifi_t *self);

/** @} */
//// @}


/***************************************************************************/
/// @addtogroup libgazebo
/// @{
/** @defgroup joint joint

The joint interface allows clients to send commands to and read
joint data from simulated articulated joints. This is usuful for arbitrary
joint chains. 

@{
*/

#define GAZEBO_JOINT_MAX_JOINTS 20

/// Individual joint data
typedef struct gz_joint_data
{
  /// Joint type
  int type;
 
  /// Anchor vector
  double anchor[3];

  /// First axis 
  double axis[3];

  /// Second axis, for Universal and Hinge2 joints
  double axis2[3];

  /// Angular velocity about first axis
  double angular_velocity;

  /// Angular velocity about second axis
  double angular_velocity2;

  /// Angle of first axis
  double angle;

  /// Angle of second axis
  double angle2;

  /// Commanded angle for the first axis
  double cmd_angle;

  /// Commanded angle for the second axis
  double cmd_angle2;

} gz_joint_data_t;

/// Joints interface
typedef struct gz_joints
{
  /// Common data structures
  gz_data_t head;

  /// Data timestamp
  double time;

  gz_joint_data_t joints[GAZEBO_JOINT_MAX_JOINTS];

  int joint_count;

} gz_joints_t;


/// The joint interface
typedef struct gz_joint
{
  /// General interface info
  gz_iface_t *iface;

  /// Pointer to interface data area
  gz_joints_t *data;
  
} gz_joint_t;


/// Create an interface
extern gz_joint_t *gz_joint_alloc();

/// Destroy an interface
extern void gz_joint_free(gz_joint_t *self);

/// @internal Create the interface (used by Gazebo server)
extern int gz_joint_create(gz_joint_t *self, gz_server_t *server, const char *id, const char *model_type, int model_id, int parent_model_id);

/// @internal Destroy the interface (server)
extern int gz_joint_destroy(gz_joint_t *self);

/// Open an existing interface (used by Gazebo clients)
extern int gz_joint_open(gz_joint_t *self, gz_client_t *client, const char *id);

/// Close the interface (client)
extern int gz_joint_close(gz_joint_t *self);

/// Lock the interface.  Set blocking to 1 if the caller should block
/// until the lock is acquired.  Returns 0 if the lock is acquired.
extern int gz_joint_lock(gz_joint_t *self, int blocking);

/// Unlock the interface
extern void gz_joint_unlock(gz_joint_t *self);

/// Tell clients that new data is available
extern int gz_joint_post(gz_joint_t *self);

/** @} */
/// @}




#ifdef __cplusplus
}
#endif

#endif
