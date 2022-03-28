//
// be_eu.h
//
// 32/64-bit using macro BITS64 for 64-bit

#ifndef _BE_EU_H
#define _BE_EU_H

#include "common.h"

#ifndef MAX_SEQ_LEN
#if INTPTR_MAX == INT32_MAX
//#	define MAX_SEQ_LEN ((((unsigned long)0xFFFFFFFF - sizeof(struct s1)) / sizeof(object)) - 1)
#define MAX_SEQ_LEN 1073741800
#else
//#	define MAX_SEQ_LEN ((((unsigned long long)0xFFFFFFFFFFFFFFFFLL - sizeof(struct s1)) / sizeof(object)) - 1)
#define MAX_SEQ_LEN 0x0FFFFFFFFFFFFFF8LL
#endif
#endif		                /* maximum sequence length set such that it doesn't overflow */

#ifdef BITS64
// from: be_runtime.c

void Cleanup(long long int status);
void UserCleanup(long long int status);
void SimpleRTFatal(char *msg);
#ifndef DONT_USE_RTFATAL
void RTFatal(char *msg);
#endif
void SpaceMessage();
char *EMalloc(unsigned long long nbytes);
char *ERealloc(char *orig, unsigned long long newsize);
s1_ptr NewS1(long long size);
object NewString(const char *s);
s1_ptr SequenceCopy(register s1_ptr a);
object NewDouble(long double d);
object Dadd(d_ptr a, d_ptr b);
object Dminus(d_ptr a, d_ptr b);
object Dmultiply(d_ptr a, d_ptr b);
char doChar(object elem);
void Prepend(object_ptr target, object s1, object a);
void Append(object_ptr target, object s1, object a);
void Concat(object_ptr target, object a_obj, s1_ptr b);
void Concat_N(object_ptr target, object_ptr  source, long long int n);
void Concat_Ni(object_ptr target, object_ptr *source, long long int n);
void RepeatElem(long long int *addr, object item, long long int repcount);
object Repeat(object item, object repcount);

void de_reference(s1_ptr a);
void DeRef1(long long int a);
void DeRef5(long long int a, long long int b, long long int c, long long int d, long long int e);
void de_reference_i(s1_ptr a);

object DoubleToInt(object d);

object x(); /* error routine */
object add(long long a, long long b);
object minus(long long a, long long b);
object multiply(long long a, long long b);
object divide(long long a, long long b);
object Ddivide(d_ptr a, d_ptr b);
object eremainder(long long a, long long b);  // avoid conflict with "remainder" math fn
//object Dremainder(d_ptr a, d_ptr b);
void check_bits(d_ptr a, d_ptr b);
object and_bits(long long a, long long b);
object Dand_bits(d_ptr a, d_ptr b);
object or_bits(long long a, long long b);
object Dor_bits(d_ptr a, d_ptr b);
object xor_bits(long long a, long long b);
object Dxor_bits(d_ptr a, d_ptr b);
object not_bits(long long a);
object Dnot_bits(d_ptr a);
//object power(long long a, long long b);
//object Dpower(d_ptr a, d_ptr b);
object equals(long long a, long long b);
object Dequals(d_ptr a, d_ptr b);
object less(long long a, long long b);
object Dless(d_ptr a, d_ptr b);
object greater(long long a, long long b);
object Dgreater(d_ptr a, d_ptr b);
object noteq(long long a, long long b);
object Dnoteq(d_ptr a, d_ptr b);
object lesseq(long long a, long long b);
object Dlesseq(d_ptr a, d_ptr b);
object greatereq(long long a, long long b);
object Dgreatereq(d_ptr a, d_ptr b);
object Band(long long a, long long b);
object Dand(d_ptr a, d_ptr b);
object Bor(long long a, long long b);
object Dor(d_ptr a, d_ptr b);
object Bxor(long long a, long long b);
object Dxor(d_ptr a, d_ptr b);
/* --- Unary Ops --- */
object uminus(long long a);
object Duminus(d_ptr a);
object unot(long long a);
object Dnot(d_ptr a);
//object e_sqrt(long long a);
//object De_sqrt(d_ptr a);
//object e_sin(long long a);
//object De_sin(d_ptr a);
//object e_cos(long long a);
//object De_cos(d_ptr a);
//object e_tan(long long a);
//object De_tan(d_ptr a);
//object e_arctan(long long a);
//object De_arctan(d_ptr a);
//object e_log(long long a);
//object De_log(d_ptr a);
//object e_floor(long long a); // not used anymore
//object De_floor(d_ptr a);
// void setran()
// ldiv_t my_ldiv (long long int numer, long long int denom)
// unsigned long long good_rand()
// /* Public Domain random number generator from USENET posting */
// object Random(long long a)
// object DRandom(d_ptr a)
// object unary_op(int fn, object a)
// object binary_op_a(int fn, object a, object b)
// object binary_op(int fn, object a, object b)
long long int compare(object a, object b);
long long find(object a, s1_ptr b);
long long e_match(s1_ptr a, s1_ptr b);
#ifndef ERUNTIME
void CheckSlice(s1_ptr a, long long startval, long long endval, long long length);
#endif
void RHS_Slice(s1_ptr a, object start, object end);
void AssignSlice(object start, object end, s1_ptr val);
// object Date();
void MakeCString(char *s, object obj);
object make_atom32(unsigned long c32);
//here
long long find_from(object a, s1_ptr b, object c);
long long e_match_from(s1_ptr a, s1_ptr b, object c);
//here

// from: be_w.c

void screen_output(FILE *f, char *out_string);


#else
// from: be_runtime.c

void Cleanup(long int status);
void UserCleanup(long int status);
void SimpleRTFatal(char *msg);
#ifndef DONT_USE_RTFATAL
void RTFatal(char *msg);
#endif
void SpaceMessage();
char *EMalloc(unsigned long nbytes);
char *ERealloc(char *orig, unsigned long newsize);
s1_ptr NewS1(long size);
object NewString(const char *s);
s1_ptr SequenceCopy(register s1_ptr a);
object NewDouble(double d);
object Dadd(d_ptr a, d_ptr b);
object Dminus(d_ptr a, d_ptr b);
object Dmultiply(d_ptr a, d_ptr b);
char doChar(object elem);
void Prepend(object_ptr target, object s1, object a);
void Append(object_ptr target, object s1, object a);
void Concat(object_ptr target, object a_obj, s1_ptr b);
void Concat_N(object_ptr target, object_ptr  source, long int n);
void Concat_Ni(object_ptr target, object_ptr *source, long int n);
void RepeatElem(long int *addr, object item, long int repcount);
object Repeat(object item, object repcount);

void de_reference(s1_ptr a);
void DeRef1(long int a);
void DeRef5(long int a, long int b, long int c, long int d, long int e);
void de_reference_i(s1_ptr a);

object DoubleToInt(object d);

object x(); /* error routine */
object add(long a, long b);
object minus(long a, long b);
object multiply(long a, long b);
object divide(long a, long b);
object Ddivide(d_ptr a, d_ptr b);
object eremainder(long a, long b);  // avoid conflict with "remainder" math fn
//object Dremainder(d_ptr a, d_ptr b);
void check_bits(d_ptr a, d_ptr b);
object and_bits(long a, long b);
object Dand_bits(d_ptr a, d_ptr b);
object or_bits(long a, long b);
object Dor_bits(d_ptr a, d_ptr b);
object xor_bits(long a, long b);
object Dxor_bits(d_ptr a, d_ptr b);
object not_bits(long a);
object Dnot_bits(d_ptr a);
//object power(long a, long b);
//object Dpower(d_ptr a, d_ptr b);
object equals(long a, long b);
object Dequals(d_ptr a, d_ptr b);
object less(long a, long b);
object Dless(d_ptr a, d_ptr b);
object greater(long a, long b);
object Dgreater(d_ptr a, d_ptr b);
object noteq(long a, long b);
object Dnoteq(d_ptr a, d_ptr b);
object lesseq(long a, long b);
object Dlesseq(d_ptr a, d_ptr b);
object greatereq(long a, long b);
object Dgreatereq(d_ptr a, d_ptr b);
object Band(long a, long b);
object Dand(d_ptr a, d_ptr b);
object Bor(long a, long b);
object Dor(d_ptr a, d_ptr b);
object Bxor(long a, long b);
object Dxor(d_ptr a, d_ptr b);
/* --- Unary Ops --- */
object uminus(long a);
object Duminus(d_ptr a);
object unot(long a);
object Dnot(d_ptr a);
//object e_sqrt(long a);
//object De_sqrt(d_ptr a);
//object e_sin(long a);
//object De_sin(d_ptr a);
//object e_cos(long a);
//object De_cos(d_ptr a);
//object e_tan(long a);
//object De_tan(d_ptr a);
//object e_arctan(long a);
//object De_arctan(d_ptr a);
//object e_log(long a);
//object De_log(d_ptr a);
//object e_floor(long a); // not used anymore
//object De_floor(d_ptr a);
// void setran()
// ldiv_t my_ldiv (long int numer, long int denom)
// unsigned long good_rand()
// /* Public Domain random number generator from USENET posting */
// object Random(long a)
// object DRandom(d_ptr a)
// object unary_op(long int fn, object a)
// object binary_op_a(long int fn, object a, object b)
// object binary_op(long int fn, object a, object b)
long int compare(object a, object b);
long find(object a, s1_ptr b);
long e_match(s1_ptr a, s1_ptr b);
#ifndef ERUNTIME
void CheckSlice(s1_ptr a, long startval, long endval, long length);
#endif
void RHS_Slice(s1_ptr a, object start, object end);
void AssignSlice(object start, object end, s1_ptr val);
// object Date();
void MakeCString(char *s, object obj);
object make_atom32(unsigned long c32);
//here
long find_from(object a, s1_ptr b, object c);
long e_match_from(s1_ptr a, s1_ptr b, object c);
//here

// from: be_w.c

void screen_output(FILE *f, char *out_string);


#endif // BITS64


#endif // _BE_EU_H
