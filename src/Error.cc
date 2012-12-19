
/***************************************************************************
 * Desc: Error handling macros
 * Author: Andrew Howard
 * Date: 13 May 2002
 * CVS: $Id: Error.cc,v 1.11 2006/11/02 16:54:09 natepak Exp $
 **************************************************************************/

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <limits.h>
#include "Error.hh"

// User-selected msg level: 0 for the most important messages (always
// printed); 9 for the least important.
static int msgLevel;

// File for logging messages
static FILE *msgFile;

static char *logFileName;

// Initialize error logging
void ErrorInit(int msgLevel, const char *logFile)
{
  ::msgLevel = msgLevel;

  if (logFile==NULL)
  {
    ::logFileName = (char*)malloc(PATH_MAX);
    memset(::logFileName, 0, PATH_MAX);
    strcat(::logFileName, getenv("HOME"));
    strcat(::logFileName,"/.gazebo");
  } else
  {
    ::logFileName = strdup(logFile);
  }

  ::msgFile = fopen(::logFileName, "w");

  if (::msgFile == NULL)
  {
    ::msgFile = stdout;
    printf("Unable to create %s log file.\n",::logFileName);
    printf("Messages will be sent to stdout.\n");
  }
}


// Finalize error logging
void ErrorFini()
{
  // Flush the running totals
  printf("\n");
  PRINT_MSG1(0, "errors, warnings and messages have been appended to %s",::logFileName);
  free(::logFileName);
}

// Function for printing and logging errors.
int ErrorPrint(int msgType, int level, const char *file, int line, const char *fmt, ...)
{
  va_list err_list, file_list;
  
  va_start(err_list, fmt);
  va_start(file_list, err_list);

  if (msgType == GZ_ERR_ERR || msgType == GZ_ERR_WARN)
  {
    vfprintf(stderr, fmt, err_list);
    // Call va_start after vfprintf to reinitialize 'ap'
    //va_start(ap, fmt);

    fprintf(::msgFile, "%s:%d ", file, line);
    vfprintf(::msgFile, fmt, file_list);
  }
  else if (msgType == GZ_ERR_MSG)
  {
    if (level <= msgLevel)
    {
      vfprintf(stderr, fmt, err_list);
      // Call va_start after vfprintf to reinitialize 'ap'
      //va_start(ap, fmt);
    }

    fprintf(::msgFile, "%s:%d ", file, line);
    vfprintf(::msgFile, fmt, file_list);
  }
  
  va_end(err_list);
  va_end(file_list);
  
  return 0;
}


// Print GL error messages
int ErrorPrintGL(const char *file, int line)
{
  GLenum erc;

  while ((erc = glGetError()) != GL_NO_ERROR)
  {
    ErrorPrint(GZ_ERR_WARN, 0, file, line,
               "gl error : %s\n", gluErrorString(erc));
  }
  return 0;
}
