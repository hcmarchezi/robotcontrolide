
/***************************************************************************
 * Desc: Error handling macros
 * Author: Andrew Howard
 * Date: 13 May 2002
 * CVS: $Id: Error.hh,v 1.12 2005/03/19 21:10:30 natepak Exp $
 **************************************************************************/

#ifndef ERROR_HH
#define ERROR_HH

#include <stdio.h>
#include <errno.h>

/// @brief Initialize error logging
void ErrorInit(int msgLevel, const char *logFile=NULL);

/// @brief Finalize error logging
void ErrorFini();

/// @brief Function for print and logging errors.
///
/// Do not call this function directly; use the macros below.
/// @internal 
int ErrorPrint(int msgType, int level, const char *file, int line, const char *fmt, ...);

/// @brief Print GL error messages
///
/// Do not call this function directly; use the macros below.
/// @internal 
int ErrorPrintGL(const char *file, int line);


/// @internal Message types (internal use only; code should use the macros)
#define GZ_ERR_ERR 0
#define GZ_ERR_WARN 1
#define GZ_ERR_MSG 2
#define GZ_ERR_DBG 2
           
/// Error message macros
#define PRINT_ERR(msg)         ErrorPrint(GZ_ERR_ERR, 0, __FILE__, __LINE__, "error   : " msg "\n")
#define PRINT_ERR1(msg, a)     ErrorPrint(GZ_ERR_ERR, 0, __FILE__, __LINE__, "error   : " msg "\n", a)
#define PRINT_ERR2(msg, a, b)  ErrorPrint(GZ_ERR_ERR, 0, __FILE__, __LINE__, "error   : " msg "\n", a, b)
#define PRINT_ERR3(msg, a, b, c)  ErrorPrint(GZ_ERR_ERR, 0, __FILE__, __LINE__,  "error   : " msg "\n", a, b, c)

/// Warning message macros
#define PRINT_WARN(msg)        ErrorPrint(GZ_ERR_WARN, 0, __FILE__, __LINE__, "warning : " msg "\n")
#define PRINT_WARN1(msg, a)    ErrorPrint(GZ_ERR_WARN, 0, __FILE__, __LINE__, "warning : " msg "\n", a)
#define PRINT_WARN2(msg, a, b) ErrorPrint(GZ_ERR_WARN, 0, __FILE__, __LINE__, "warning : " msg "\n", a, b)
#define PRINT_WARN3(msg, a, b, c) ErrorPrint(GZ_ERR_WARN, 0, __FILE__, __LINE__, "warning : " msg "\n", a, b, c)

/// General messages.  Use level to indicate the message importance
///  - 0 : important
///  - 1 : informative
///  - 2+ : diagnostic
/// All messages are recorded in the log file, but only the more important
/// messages are printed on the console.  Use the command line option to
/// dictate which messages will be printed.
#define PRINT_MSG0(level, msg) ErrorPrint(GZ_ERR_MSG, level, __FILE__, __LINE__, "" msg "\n") 
#define PRINT_MSG1(level, msg, a) ErrorPrint(GZ_ERR_MSG, level, __FILE__, __LINE__, "" msg "\n", a) 
#define PRINT_MSG2(level, msg, a, b)  ErrorPrint(GZ_ERR_MSG, level, __FILE__, __LINE__, "" msg "\n", a, b)
#define PRINT_MSG3(level, msg, a, b, c) ErrorPrint(GZ_ERR_MSG, level, __FILE__, __LINE__, "" msg "\n", a, b, c)
#define PRINT_MSG4(level, msg, a, b, c, d) ErrorPrint(GZ_ERR_MSG, level, __FILE__, __LINE__, "" msg "\n", a, b, c, d)
#define PRINT_MSG5(level, msg, a, b, c, d, e) ErrorPrint(GZ_ERR_MSG, level, __FILE__, __LINE__, "" msg "\n", a, b, c, d, e)
#define PRINT_MSG6(level, msg, a, b, c, d, e, f) ErrorPrint(GZ_ERR_MSG, level, __FILE__, __LINE__, "" msg "\n", a, b, c, d, e, f)
#define PRINT_MSG7(level, msg, a, b, c, d, e, f, g) ErrorPrint(GZ_ERR_MSG, level, __FILE__, __LINE__, "" msg "\n", a, b, c, d, e, f, g)


// Print GL error messages (check if error is set)
#define PRINT_GL_ERR() ErrorPrintGL(__FILE__, __LINE__)

#endif
