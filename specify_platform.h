//
// platform.h
//
// Here you specify which platform you are using:
//
// Included by "pch.h"

#ifndef _PLATFORM_H
#define _PLATFORM_H

#ifdef BUILDING_WIN_DLL
#define BUILDING_DLL
#define EWINDOWS
#endif

#ifdef USING_WIN_DLL
#define USING_DLL
#define EWINDOWS
#endif

#ifdef __GNUC__
// CLEANUP_MOD is reserved for the future, it has not been properly coded yet.
//#define CLEANUP_MOD
#endif

// Begin Platform
// These macros are set in specify_platform.h

#ifdef __cplusplus
#define USE_STDARG_H
#endif

// Modules:

#define USE_STANDARD_LIBRARY
#define USE_MATH_H

// If under Windows, else comment out.
//#define EWINDOWS

// Define this macro (DONE_DEBUGGING) before distributing software:
//#define DONE_DEBUGGING

// If you want to specify these macros on the command line, comment out the next line:
//#include "specify_platform.h"

// End Platform


// Define if using GCC
//#define ELINUX

// Use when debugging source code:
#define EXTRA_CHECK

// If under Windows, define EWINDOWS, else comment it out.
//#define EWINDOWS

// You can use the __WATCOMC__ macro on the OpenWatcom.org compiler.
//#ifdef __WATCOMC__
//#endif // __WATCOMC__

// Define this macro (DONE_DEBUGGING) before distributing software:
//#define DONE_DEBUGGING

#endif // _PLATFORM_H
