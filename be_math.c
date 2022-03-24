//
// be_eu.c

#include "be_math.h"

// all use: #include <math.h>

object Dremainder(d_ptr a, d_ptr b)
/* double remainder of a divided by b */
{
    if (b->dbl == 0.0)
        RTFatal("can't get remainder of a number divided by 0");
    return (object)NewDouble(fmod(a->dbl, b->dbl)); /* for now */
}

object power(long a, long b)
/* integer a to the power b */
{
    long i, p;

    if (a == 2 && b >= 0 && b <= 29) {
        /* positive power of 2 */
        return MAKE_INT(1 << b);
    }
    else if (a == 0 && b <= 0) {
        RTFatal("can't raise 0 to power <= 0");
        return 0;
    }
    else if (b == 0) {
        return ATOM_1;
    }
    else if (b >= 1 && b <= 4 && a >= -178 && a <= 178) {
        p = a;  
        for (i = 2; i <= b; i++)
            p = p * a;
        return MAKE_INT(p);
    }
    else
        return (object)NewDouble(pow((double)a, (double)b));
}

object Dpower(d_ptr a, d_ptr b)
/* double power */
{
    if (a->dbl == 0.0 && b->dbl <= 0.0)
        RTFatal("can't raise 0 to power <= 0");
    if (a->dbl < 0.0 && floor(b->dbl) != b->dbl)
        RTFatal("can't raise negative number to non-integer power");
    return (object)NewDouble(pow(a->dbl, b->dbl));
}


object e_sqrt(long a)
/* integer square_root(a) */
{
    if (a < 0)
        RTFatal("attempt to take square root of a negative number");
    return (object)NewDouble( sqrt((double)a) );
}

object De_sqrt(d_ptr a)
/* double square root(a) */
{
    if (a->dbl < 0)
        RTFatal("attempt to take square root of a negative number");
    return (object)NewDouble( sqrt(a->dbl) );
}


object e_sin(long a)
/* sin of an angle a (radians) */
{
    return (object)NewDouble( sin((double)a) );
}

object De_sin(d_ptr a)
/* double sin of a */
{
    return (object)NewDouble( sin(a->dbl) );
}

object e_cos(long a)
/* cos of an angle a (radians) */
{
    return (object)NewDouble( cos((double)a) );
}

object De_cos(d_ptr a)
/* double cos of a */
{
    return (object)NewDouble( cos(a->dbl) );
}

object e_tan(long a)
/* tan of an angle a (radians) */
{
    return (object)NewDouble( tan((double)a) );
}

object De_tan(d_ptr a)
/* double tan of a */
{
    return (object)NewDouble( tan(a->dbl) );
}

object e_arctan(long a)
/* arctan of an angle a (radians) */
{
    return (object)NewDouble( atan((double)a) );
}

object De_arctan(d_ptr a)
/* double arctan of a */
{
    return (object)NewDouble( atan(a->dbl) );
}

object e_log(long a)
/* natural log of a (integer) */
{
    if (a <= 0)
        RTFatal("may only take log of a positive number");
    return (object)NewDouble( log((double)a) );
}

object De_log(d_ptr a)
/* natural log of a (double) */
{
    if (a->dbl <= 0.0)
        RTFatal("may only take log of a positive number");
    return (object)NewDouble( log(a->dbl) );
}

object e_floor(long a)  // not used anymore
/* floor of a number - no op since a is already known to be an int */
{
    return a; 
}

object De_floor(d_ptr a)
/* floor of a number */
{
    double temp;

    temp = floor(a->dbl); 
#ifndef ERUNTIME    
    if (fabs(temp) < MAXINT_DBL)
        return MAKE_INT((long)temp);
    else 
#endif      
        return (object)NewDouble(temp);
}

