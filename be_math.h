//
// be_math.h
//
// Functions that use the math library: #include <math.h>

#ifndef _BE_MATH_H
#define _BE_MATH_H

#include <math.h>

#include "be_eu.h"

// be_runtime.c

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


#endif // _BE_MATH_H
