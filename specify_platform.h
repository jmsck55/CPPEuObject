//
// platform.h
//
// Here you specify which platform you are using:
//
// Included by "pch.h"

#ifndef _PLATFORM_H
#define _PLATFORM_H

//#define BUILDING_DLL

// Begin Platform
// These macros are set in specify_platform.h

// If under Windows, else comment out.
//#define EWINDOWS

// Define this macro (DONE_DEBUGGING) before distributing software:
//#define DONE_DEBUGGING

// If you want to specify these macros on the command line, comment out the next line:
//#include "specify_platform.h"

// End Platform


// Standard defines:
#define USE_STDARG_H
#define USE_MATH_H
#define USE_STANDARD_LIBRARY

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
