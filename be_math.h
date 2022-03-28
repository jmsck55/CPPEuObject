//
// be_math.h
//
// Functions that use the math library: #include <math.h>
//
// 32/64-bit using macro BITS64 for 64-bit

#ifndef _BE_MATH_H
#define _BE_MATH_H

//#include <math.h>

#define USE_MATH_H
#include "be_eu.h"

// be_runtime.c
#ifdef BITS64
// all use: #include <math.h>
object Dremainder(d_ptr a, d_ptr b);
object power(long long a, long long b);
object Dpower(d_ptr a, d_ptr b);

object e_sqrt(long long a);
object De_sqrt(d_ptr a);
object e_sin(long long a);
object De_sin(d_ptr a);
object e_cos(long long a);
object De_cos(d_ptr a);
object e_tan(long long a);
object De_tan(d_ptr a);
object e_arctan(long long a);
object De_arctan(d_ptr a);
object e_log(long long a);
object De_log(d_ptr a);
object e_floor(long long a); // not used anymore
object De_floor(d_ptr a);
#else
// all use: #include <math.h>
object Dremainder(d_ptr a, d_ptr b);
object power(long a, long b);
object Dpower(d_ptr a, d_ptr b);

object e_sqrt(long a);
object De_sqrt(d_ptr a);
object e_sin(long a);
object De_sin(d_ptr a);
object e_cos(long a);
object De_cos(d_ptr a);
object e_tan(long a);
object De_tan(d_ptr a);
object e_arctan(long a);
object De_arctan(d_ptr a);
object e_log(long a);
object De_log(d_ptr a);
object e_floor(long a); // not used anymore
object De_floor(d_ptr a);
#endif // BITS64

#endif // _BE_MATH_H
