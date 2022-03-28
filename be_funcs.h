//
// be_funcs.h
//
// 32/64-bit using macro BITS64 for 64-bit

#ifndef _BE_FUNCS_H
#define _BE_FUNCS_H

//#include <time.h>

#define USE_STANDARD_LIBRARY
#include "be_eu.h"

// Uses files from Euphoria v4.0.5

// be_runtime.c

object Date(); // This function uses: #include <time.h>

// Random functions (on Windows, it requires the "EWINDOWS" to be defined, such as "#define EWINDOWS")

// 32-bit random number generator:

void setran32();
//static ldiv_t my_ldiv (long int numer, long int denom);
unsigned long good_rand32();
object Random32(long a);
object DRandom32(d_ptr a);
object calc_hash32(object a, object b);

// be_machine.c

long get_int32(object x);
object get_rand32();
object set_rand32(object x);


#endif // _BE_FUNCS_H
