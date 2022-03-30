// Copyright (c) 2022 James Cook
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

MY_DLL_API object Date(); // This function uses: #include <time.h>

// Random functions (on Windows, it requires the "EWINDOWS" to be defined, such as "#define EWINDOWS")

// 32-bit random number generator:

MY_DLL_API void setran32();
//static ldiv_t my_ldiv (long int numer, long int denom);
MY_DLL_API unsigned long good_rand32();
MY_DLL_API object Random32(long a);
MY_DLL_API object DRandom32(d_ptr a);
MY_DLL_API object calc_hash32(object a, object b);

// be_machine.c

MY_DLL_API long get_int32(object x);
MY_DLL_API object get_rand32();
MY_DLL_API object set_rand32(object x);


#endif // _BE_FUNCS_H
