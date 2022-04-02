// Copyright (c) 2022 James Cook
// be_eu.h
//
// 32/64-bit using macro BITS64 for 64-bit

#ifndef _BE_EU_H
#define _BE_EU_H

#include "pch.h"
#include "common.h"

#ifndef MAX_SEQ_LEN
#if INTPTR_MAX == INT32_MAX
//#     define MAX_SEQ_LEN ((((unsigned long)0xFFFFFFFF - sizeof(struct s1)) / sizeof(object)) - 1)
#define MAX_SEQ_LEN 1073741800
#else
//#     define MAX_SEQ_LEN ((((eulong)0xFFFFFFFFFFFFFFFFLL - sizeof(struct s1)) / sizeof(object)) - 1)
#define MAX_SEQ_LEN 0x0FFFFFFFFFFFFFF8LL
#endif
#endif                          /* maximum sequence length set such that it doesn't overflow */

// from: be_runtime.c

MY_DLL_API void Cleanup(elong status);
MY_DLL_API void UserCleanup(elong status);
MY_DLL_API void SimpleRTFatal(char *msg);
#ifndef DONT_USE_RTFATAL
MY_DLL_API void RTFatal(const char *msg);
#endif
MY_DLL_API void SpaceMessage();
MY_DLL_API char *EMalloc(eulong nbytes);
MY_DLL_API char *ERealloc(char *orig, eulong newsize);
MY_DLL_API s1_ptr NewS1(elong size);
MY_DLL_API object NewString(const char *s);
MY_DLL_API s1_ptr SequenceCopy(REGISTER s1_ptr a);
MY_DLL_API object NewDouble(eudouble d);
MY_DLL_API object Dadd(d_ptr a, d_ptr b);
MY_DLL_API object Dminus(d_ptr a, d_ptr b);
MY_DLL_API object Dmultiply(d_ptr a, d_ptr b);
MY_DLL_API char doChar(object elem);
MY_DLL_API void Prepend(object_ptr target, object s1, object a);
MY_DLL_API void Append(object_ptr target, object s1, object a);
MY_DLL_API void Concat(object_ptr target, object a_obj, s1_ptr b);
MY_DLL_API void Concat_N(object_ptr target, object_ptr  source, elong n);
MY_DLL_API void Concat_Ni(object_ptr target, object_ptr *source, elong n);
MY_DLL_API void RepeatElem(elong *addr, object item, elong repcount);
MY_DLL_API object Repeat(object item, object repcount);

MY_DLL_API void de_reference(s1_ptr a);
MY_DLL_API void DeRef1(elong a);
MY_DLL_API void DeRef5(elong a, elong b, elong c, elong d, elong e);
MY_DLL_API void de_reference_i(s1_ptr a);

MY_DLL_API object DoubleToInt(object d);

MY_DLL_API object x(); /* error routine */
MY_DLL_API object add(elong a, elong b);
MY_DLL_API object minus(elong a, elong b);
MY_DLL_API object multiply(elong a, elong b);
MY_DLL_API object divide(elong a, elong b);
MY_DLL_API object Ddivide(d_ptr a, d_ptr b);
MY_DLL_API object eremainder(elong a, elong b);  // avoid conflict with "remainder" math fn
//MY_DLL_API object Dremainder(d_ptr a, d_ptr b);
MY_DLL_API void check_bits(d_ptr a, d_ptr b);
MY_DLL_API object and_bits(elong a, elong b);
MY_DLL_API object Dand_bits(d_ptr a, d_ptr b);
MY_DLL_API object or_bits(elong a, elong b);
MY_DLL_API object Dor_bits(d_ptr a, d_ptr b);
MY_DLL_API object xor_bits(elong a, elong b);
MY_DLL_API object Dxor_bits(d_ptr a, d_ptr b);
MY_DLL_API object not_bits(elong a);
MY_DLL_API object Dnot_bits(d_ptr a);
//MY_DLL_API object power(elong a, elong b);
//MY_DLL_API object Dpower(d_ptr a, d_ptr b);
MY_DLL_API object equals(elong a, elong b);
MY_DLL_API object Dequals(d_ptr a, d_ptr b);
MY_DLL_API object less(elong a, elong b);
MY_DLL_API object Dless(d_ptr a, d_ptr b);
MY_DLL_API object greater(elong a, elong b);
MY_DLL_API object Dgreater(d_ptr a, d_ptr b);
MY_DLL_API object noteq(elong a, elong b);
MY_DLL_API object Dnoteq(d_ptr a, d_ptr b);
MY_DLL_API object lesseq(elong a, elong b);
MY_DLL_API object Dlesseq(d_ptr a, d_ptr b);
MY_DLL_API object greatereq(elong a, elong b);
MY_DLL_API object Dgreatereq(d_ptr a, d_ptr b);
MY_DLL_API object Band(elong a, elong b);
MY_DLL_API object Dand(d_ptr a, d_ptr b);
MY_DLL_API object Bor(elong a, elong b);
MY_DLL_API object Dor(d_ptr a, d_ptr b);
MY_DLL_API object Bxor(elong a, elong b);
MY_DLL_API object Dxor(d_ptr a, d_ptr b);
/* --- Unary Ops --- */
MY_DLL_API object uminus(elong a);
MY_DLL_API object Duminus(d_ptr a);
MY_DLL_API object unot(elong a);
MY_DLL_API object Dnot(d_ptr a);
//MY_DLL_API object e_sqrt(elong a);
//MY_DLL_API object De_sqrt(d_ptr a);
//MY_DLL_API object e_sin(elong a);
//MY_DLL_API object De_sin(d_ptr a);
//MY_DLL_API object e_cos(elong a);
//MY_DLL_API object De_cos(d_ptr a);
//MY_DLL_API object e_tan(elong a);
//MY_DLL_API object De_tan(d_ptr a);
//MY_DLL_API object e_arctan(elong a);
//MY_DLL_API object De_arctan(d_ptr a);
//MY_DLL_API object e_log(elong a);
//MY_DLL_API object De_log(d_ptr a);
//MY_DLL_API object e_floor(elong a); // not used anymore
//MY_DLL_API object De_floor(d_ptr a);
//MY_DLL_API void setran()
//MY_DLL_API ldiv_t my_ldiv (elong numer, elong denom)
//MY_DLL_API eulong good_rand()
//MY_DLL_API /* Public Domain random number generator from USENET posting */
//MY_DLL_API object Random(elong a)
//MY_DLL_API object DRandom(d_ptr a)
//MY_DLL_API object unary_op(int fn, object a)
//MY_DLL_API object binary_op_a(int fn, object a, object b)
//MY_DLL_API object binary_op(int fn, object a, object b)
MY_DLL_API elong compare(object a, object b);
MY_DLL_API elong find(object a, s1_ptr b);
MY_DLL_API elong e_match(s1_ptr a, s1_ptr b);
#ifndef ERUNTIME
MY_DLL_API void CheckSlice(s1_ptr a, elong startval, elong endval, elong length);
#endif
MY_DLL_API void RHS_Slice(s1_ptr a, object start, object end);
MY_DLL_API void AssignSlice(object start, object end, s1_ptr val);
//MY_DLL_API object Date();
MY_DLL_API void MakeCString(char *s, object obj);
MY_DLL_API object make_atom32(unsigned long c32);
//here
MY_DLL_API elong find_from(object a, s1_ptr b, object c);
MY_DLL_API elong e_match_from(s1_ptr a, s1_ptr b, object c);
//here

// from: be_w.c

MY_DLL_API void screen_output(FILE *f, char *out_string);


#endif // _BE_EU_H
