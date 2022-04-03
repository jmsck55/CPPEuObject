// Copyright (c) 2022 James Cook
// test.cpp

// NOTE: I have not tested Dbl class functions yet.

//#ifdef __GNUC__
//#define __STDC_WANT_IEC_60559_TYPES_EXT__
//#endif

// First, the standard header files, like <iostream>, then the C header files (*.h),
// Last, the C++ header files (*.hpp)

#include <iostream>
#include <cfloat> // or <float.h>

extern "C" {
    #include "pch.h"
    #include <stdint.h>
}

#include "eu.hpp"
#include "test.hpp"

// using namespace eu;
using namespace std;

#define EOBJECT Object
#define ESEQUENCE Sequence
#define EATOM Atom
#define EINTEGER Integer
#define EDBL Dbl

#ifdef DONE_DEBUGGING
#define SHOW_DEBUG(S, X)
#else
void myShowDebug(eu::base_class* x)
{
    x->ShowDebug();
    puts("[break-point]\n");
}
#define SHOW_DEBUG(S, X) printf(S ":"); myShowDebug((eu::base_class*)&X)
#endif

#ifdef BITS64
#define COUNT(n) INT64_C(n)
#define DBL80_C(c) c ## L
#else
#define COUNT(n) n
#define INT64_C(x) x
#define DBL80_C(c) c
#endif

int main()
{
        char ch;
        eu::eudouble dbl;
        char *str;
#ifdef BITS64
        cout << "64-bit version, BITS64 enabled." << endl;
#else
        cout << "32-bit version" << endl;
#endif
        //eu::EOBJECT a; //, b, c, d;
        cout << "Size of pointer == " << sizeof(void*) << endl;
        cout << "Size of elong == " << sizeof(eu::elong) << endl;
        cout << "Size of eudouble == " << sizeof(eu::eudouble) << endl;
        cout << "Size of double == " << sizeof(double) << endl;
        cout << "Size of long double == " << sizeof(long double) << endl;
        cout << "Size of sizeof(struct d) == " << sizeof(struct eu::d) << endl;
        cout << "Size of sizeof(struct s1) == " << sizeof(struct eu::s1) << endl;
#ifdef __GNUC__
        cout << "Size of __float128 == " << sizeof(__float128) << endl;
        cout << "DBL_MANT_DIG == " << __DBL_MANT_DIG__ << endl;
        cout << "DBL_DIG == " << __DBL_DIG__ << endl;
        cout << "DBL_DECIMAL_DIG == " << __DBL_DECIMAL_DIG__ << endl;
        cout << "LDBL_MANT_DIG == " << __LDBL_MANT_DIG__ << endl;
        cout << "LDBL_DIG == " << __LDBL_DIG__ << endl;
        cout << "LDBL_DECIMAL_DIG == " << __LDBL_DECIMAL_DIG__ << endl;
        cout << "FLT128_MANT_DIG == " << __FLT128_MANT_DIG__ << endl;
        cout << "FLT128_DIG == " << __FLT128_DIG__ << endl;
        cout << "FLT128_MIN_EXP == " << __FLT128_MIN_EXP__ << endl;
        cout << "FLT128_MIN_10_EXP == " << __FLT128_MIN_10_EXP__ << endl;
        cout << "FLT128_MAX_EXP == " << __FLT128_MAX_EXP__ << endl;
        cout << "FLT128_MAX_10_EXP == " << __FLT128_MAX_10_EXP__ << endl;
        //cout << "FLT128_MAX == " << __FLT128_MAX__ << endl;
        //cout << "FLT128_EPSILON == " << __FLT128_EPSILON__ << endl;
        //cout << "FLT128_MIN == " << __FLT128_MIN__ << endl;
        cout << "FLT128_DECIMAL_DIG == " << __FLT128_DECIMAL_DIG__ << endl;
        //cout << "FLT128_TRUE_MIN == " << __FLT128_DENORM_MIN__ << endl;
        cout << endl;
#endif // __GNUC__

	if (true) {
		eu::EATOM n1(DBL80_C(1.0)), n2(DBL80_C(2.0)), n3(DBL80_C(3.0)), n4(DBL80_C(4.0));
		eu::EATOM r1, r2;
		r1 = n1 + n2;
		r1.println();
		SHOW_DEBUG("r1", r1);
		r2 = r1 - n3; // zero (0).
		r2.println();
		SHOW_DEBUG("r2", r2);
		r2 = n3 * n4;
		r2.println();
		SHOW_DEBUG("r2", r2);
		r1 = r2 / n2;
		printf("Should be equal, 6 == ");
		r1.println();
		SHOW_DEBUG("r1", r1);
		
		
	}

        if (true) {
                eu::ESEQUENCE s0, s1("Hi\n"), s2, s3, s4;
                s1.ScreenOutput(stdout);
                SHOW_DEBUG("s1", s1);
                s2 = "hi\n"; // assignment
                s2.ScreenOutput(stdout);
                SHOW_DEBUG("s2", s2);
                s3 = eu::NewString("HI\n"); // function
                s3.ScreenOutput(stdout);
                SHOW_DEBUG("s3", s3);
                s4.newString("hI\n"); // member function
                s4.ScreenOutput(stdout);
                SHOW_DEBUG("s4", s4);
                s0 = s1; // s1.GetValue(); // class assignment
                s0.ScreenOutput(stdout);
                SHOW_DEBUG("s0", s0);
                
                // here.
                s2.repeat((eu::object)' ', 1);
                s1.E_assign_to_slice(3, 3, s2); // s2 calls the "copy constructor"
                SHOW_DEBUG("s1", s1);
                s3.newString("world\n");
                eu::S_concat((eu::object_ptr)&s0, CASTING_OBJECT(eu::object, s1), CASTING_OBJECT(eu::object, s3)); // here, may be a memory leak.
                SHOW_DEBUG("s0", s0);
                SHOW_DEBUG("s1", s1);
                SHOW_DEBUG("s3", s3); // I'll work on this tomorrow.
                s0.println(2);
                // and here.
                s4.E_slice(s0, 4, 9); // "world\n"
                SHOW_DEBUG("s4", s4);
                s3.repeat(CASTING_OBJECT(eu::object, s4), 10); // here, may be a memory leak.
		SHOW_DEBUG("s3", s3);
                eu::S_concatN((eu::object_ptr)&s2, s3);
		SHOW_DEBUG("s2", s2);
                s2.ScreenOutput(stdout);

                
        }

// 2. Language Definition

        if (true) {
                eu::EINTEGER i1, i2;
                eu::EATOM a1, a2;
                puts("\n-- examples of atoms:");
                i1 = 0;
                i1.println();
                SHOW_DEBUG("i1", i1);
                i2 = 1000;
                i2.println();
                SHOW_DEBUG("i2", i2);
                a1 = DBL80_C(98.6);
                a1.println();
                SHOW_DEBUG("a1", a1);
                a2 = INT64_C(-1e6);
                a2.println();
                SHOW_DEBUG("a2", a2);
        }
        if (true) {
                eu::ESEQUENCE s0, s1, s2, s3;
                puts("\n-- examples of sequences:");
                s1 = eu::seq(COUNT(8), (eu::object)2, (eu::object)3, (eu::object)5, (eu::object)7, (eu::object)11, (eu::object)13, (eu::object)17, (eu::object)19);
                s1.println();
                SHOW_DEBUG("s1", s1);
                s1 = eu::seq(NOVALUE, INT64_C(2), INT64_C(3), INT64_C(5), INT64_C(7), INT64_C(11), INT64_C(13), INT64_C(17), INT64_C(19), NOVALUE); // use NOVALUE, "NOVALUE == ((long)0xbfffffffL)", to begin and end a variable agrument array.
                s1.println();
                SHOW_DEBUG("s1", s1);
                s2 = eu::seq(COUNT(5), INT64_C(1), INT64_C(2), eu::Repeat((eu::object)3, 3), INT64_C(4), eu::seq(COUNT(2), INT64_C(5), eu::seq(COUNT(1), INT64_C(6))));
                s2.println();
                SHOW_DEBUG("s2", s2);
                
                s1 = s2.E_at(-1);
                s1.println();
                SHOW_DEBUG("s1", s1);
                
                s3 = eu::seq(COUNT(3), eu::seq(COUNT(2), eu::NewString("jon"), eu::NewString("smith")), (eu::object)52389, eu::NewDouble(97.25));
                s3.println(2, 1);
                SHOW_DEBUG("s3", s3);
                //s0 = eu::seq(COUNT(0));
                //s0.println();
                //SHOW_DEBUG("s0", s0);
                s0 = s1.E_at(-1);
                s0.println();
                SHOW_DEBUG("s0", s0);
        }
        if (true) {
                eu::EATOM a1, a2, a3, a4;
                puts("\n-- Numbers can also be entered in hexadecimal. For example:");
                a1 = (eu::eudouble)0xFE;
                a1.println(0,0,"#%X",1);
                SHOW_DEBUG("a1", a1);
                a2 = (eu::eudouble)0xA000;
                a2.println(0,0,"#%X",1);
                SHOW_DEBUG("a2", a2);
                dbl = (eu::eudouble)68718428168;
                printf("%f %i\n", (double)dbl, IS_DOUBLE_TO_INT(dbl));
                a3 = dbl;
                a3.println();
                SHOW_DEBUG("a3", a3);
                a4 = (eu::eudouble)-0x10; // -16
                a4.println();
                SHOW_DEBUG("a4", a4);
                
                puts("\n2.2 Expressions");
                puts("\nTo be continued...\n");
                
                puts("\n-- More examples:");
                dbl = ((eu::eudouble)1/(eu::eudouble)7);
                a1.NewAtom(dbl);
                SHOW_DEBUG("a1", a1);
                a2 = a1; // Copying items of same type.
                SHOW_DEBUG("a2", a2);
        }

        if (true) {
                eu::ESEQUENCE s1, s2, s3;
                
                //s1.newString("Hi");
                s1 = "Hi";
                SHOW_DEBUG("s1", s1);
                s2 = s1.GetValue(); // copies sequence, always use "GetValue()" (to increase ref count by one), or else there will be a "memory violation" (i.e. Program Crash.)
                SHOW_DEBUG("s2", s2);
                s3 = eu::seq(COUNT(1), s1.GetValue());
                SHOW_DEBUG("s1", s1);
                s2 = eu::seq(NOVALUE, s3.GetValue(), s1.GetValue(), NOVALUE);
                SHOW_DEBUG("s3", s3);
                SHOW_DEBUG("s2", s2);
                SHOW_DEBUG("s1", s1);
                if (true)
                {
                        eu::ESEQUENCE tmp = s2.GetValue(); // must use "GetValue()" here.
                        SHOW_DEBUG("s1", s1);
                }
                SHOW_DEBUG("s1", s1);
                
                str = s1.GetCharStr();
                cout << str << endl;
                free(str);
                s1.print(2);
                printf("\n");
                s2.print(1);
                printf("\n");
                s2.print(2);
                printf("\n");
        }
        
        cout << endl << "Enter a character to exit." << endl;
        cin >> ch;
        
        return 0;
}
