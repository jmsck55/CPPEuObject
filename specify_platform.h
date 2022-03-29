//
// platform.h
//
// Here you specify which platform you are using:
//
// Included by "pch.h"

#ifndef _PLATFORM_H
#define _PLATFORM_H

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

// Define this macro (DONE_DEBUGGING) before distributing software:
//#define DONE_DEBUGGING

#endif // _PLATFORM_H
