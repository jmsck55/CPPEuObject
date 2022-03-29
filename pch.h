// Copyright (c) 2022 James Cook
// pch.h
//
// A "C" precompiled header.

#ifndef _PCH_H
#define _PCH_H

#include "specify_platform.h"

#ifdef EWINDOWS
#include <windows.h>
#endif

#ifdef USE_STDARG_H
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */
#endif

#include <string.h>

#ifndef EWINDOWS
#include <unistd.h>
#endif

#ifdef USE_STANDARD_LIBRARY
#include <time.h>
#endif

#include <stdlib.h>

#ifdef USE_MATH_H
#include <math.h>
#endif

#include <stdio.h>

#ifdef __GNUC__
#include <stdint.h>
// 64-bit not fully supported yet.
#if INTPTR_MAX == INT64_MAX
#define BITS64
#endif // INTPTR_MAX == INT64_MAX
#endif // __GNUC__

#endif // _PCH_H
