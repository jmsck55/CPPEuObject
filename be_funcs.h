//
// be_funcs.h
//

#ifndef _BE_FUNCS_H
#define _BE_FUNCS_H

//#include <time.h>

#define USE_STANDARD_LIBRARY
#include "be_eu.h"

// Uses files from Euphoria v4.0.5

// be_runtime.c

object Date(); // This function uses: #include <time.h>

// Random functions (on Windows, it requires the "EWINDOWS" to be defined, such as "#define EWINDOWS")

void setran();
//static ldiv_t my_ldiv (long int numer, long int denom);
unsigned long good_rand();
object Random(long a);
object DRandom(d_ptr a);
object calc_hash(object a, object b);

// be_machine.c

object get_rand();
object set_rand(object x);


#endif // _BE_FUNCS_H
