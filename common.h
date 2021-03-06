// Copyright (c) 2022 James Cook
// common.h
//
// alldef.h
//
// 32/64-bit using macro BITS64 for 64-bit
//
// TODO: Make 32-bit mode work with 64-bit pointers.
//
// Included by pch.h

#ifndef _COMMON_H
#define _COMMON_H

// The following code makes it work in both C and C++

// I would recommend to first test the code without the DLL,
// Then use one of the following macros, for building and then using the DLL.
//
// BUILDING_DLL
// USING_DLL
//
// Windows has to also define:
//
// EWINDOWS
//
// Use "specify_platform.h" to define it.
//
// EWINDOWS /* define when not using DLL */
// BUILDING_WIN_DLL /* define when building DLL */
// USING_WIN_DLL /* define when using DLL */
//
// That is what is recommended.

#ifdef __cplusplus
#define MY_EXTERN_C extern "C"
#else
#define MY_EXTERN_C extern
#endif

#ifdef BUILDING_DLL
        #ifndef DONE_DEBUGGING
                #define DONE_DEBUGGING
        #endif
	#ifdef __GNUC__
		#define MY_DLL_SYMBOL
	#else
		#define MY_DLL_SYMBOL __declspec(dllexport)
	#endif
        #define MY_DLL_API MY_EXTERN_C MY_DLL_SYMBOL
#else
	#ifdef USING_DLL
		#ifndef DONE_DEBUGGING
			#define DONE_DEBUGGING
		#endif
		#ifdef __GNUC__
			#define MY_DLL_SYMBOL
		#else
			#define MY_DLL_SYMBOL __declspec(dllimport)
		#endif
		#define MY_DLL_API MY_EXTERN_C MY_DLL_SYMBOL
	#else
		#define MY_DLL_API
		#define MY_DLL_SYMBOL
	#endif // USING_DLL
#endif // BUILDING_DLL

// For faster code, alignment should be (2 on 16-bit machines), (4 on 32-bit machines), (8 on 64-bit machines)
typedef double eudouble32;
typedef long elong32;
typedef unsigned long eulong32;
#ifdef BITS64
typedef long double eudouble;
typedef long long elong;
typedef unsigned long long eulong;
#define REGISTER
#else
#define eudouble eudouble32
#define elong elong32
#define eulong eulong32
#define REGISTER register
#endif
#ifdef BITS64
#define ELONG_WIDTH "ll"
#else
#define ELONG_WIDTH "l"
#endif

#ifdef DONE_DEBUGGING
#undef EXTRA_CHECK
#define ERUNTIME
#define DONT_USE_RTFATAL
#define RTFatal(remove_error_messages) SimpleRTFatal("")
#else
#define RTInternal RTFatal
#endif

#define FALSE 0
#define TRUE 1

#ifdef ELINUX
#define PATH_SEPARATOR ':'
#define SLASH '/'
#else
#define PATH_SEPARATOR ';'
#define SLASH '\\'
#endif

#if defined(ELINUX) || defined(EDJGPP)
#define WORD unsigned short
#define __stdcall
#define __cdecl
#else
/* So WATCOM debugger will work better: */
#ifndef EXTRA_CHECK
//#pragma aux RTFatal aborts;
//#pragma aux CompileErr aborts;
//#pragma aux SafeErr aborts;
//#pragma aux RTInternal aborts;
//#pragma aux InternalErr aborts;
//#pragma aux SpaceMessage aborts;
//#pragma aux Cleanup aborts;
#endif
#endif

#ifdef EWINDOWS
// Use Heap functions for everything.
// Avoid using strdup() or other functions that return malloc'd blocks
// #define free(p) HeapFree((void *)default_heap, 0, p)
// #define malloc(n) HeapAlloc((void *)default_heap, 0, n)
// #define realloc(p, n) HeapReAlloc((void *)default_heap, 0, p, n)
#endif

// execute.h (from Euphoria v4.0.5)

          /* Euphoria object format v1.2 and later */

/* an object is represented as a 32-bit value as follows:

        unused  : 011xxxxx xxxxxxxx xxxxxxxx xxxxxxxx
        unused  : 010xxxxx xxxxxxxx xxxxxxxx xxxxxxxx

        TOO_BIG:  01000000 00000000 00000000 00000000   (just too big for INT)

       +ATOM-INT: 001vvvvv vvvvvvvv vvvvvvvv vvvvvvvv   (31-bit integer value)
       +ATOM-INT: 000vvvvv vvvvvvvv vvvvvvvv vvvvvvvv   (31-bit integer value)
       -ATOM-INT: 111vvvvv vvvvvvvv vvvvvvvv vvvvvvvv   (31-bit integer value)
       -ATOM-INT: 110vvvvv vvvvvvvv vvvvvvvv vvvvvvvv   (31-bit integer value)

        NO VALUE: 10111111 11111111 11111111 11111111   (undefined object)

        ATOM-DBL: 101ppppp pppppppp pppppppp pppppppp   (29-bit pointer)

        SEQUENCE: 100ppppp pppppppp pppppppp pppppppp   (29-bit pointer)

   We ensure 8-byte alignment for s1 and dbl blocks - lower 3 bits
   aren't needed - only 29 bits are stored.
*/

/* NO VALUE objects can occur only in a few well-defined places,
   so we can simplify some tests. For speed we first check for ATOM-INT
   since that's what most objects are. */

#define MAKE_UINT32(x) ((object)(( ((unsigned long)x) <= ((unsigned long)0x3FFFFFFFL)) \
                          ? (unsigned long)x : \
                            (unsigned long)NewDouble32((double)(unsigned long)x)))

/* these are obsolete */
#define INT_VAL32(x)        ((int)(x))
#define MAKE_INT32(x)       ((object)(x))

#ifdef BITS64
#define NOVALUE      ((long long)0xbfffffffffffffffLL)
#define TOO_BIG_INT  ((long long)0x4000000000000000LL)
#define HIGH_BITS    ((long long)0xC000000000000000LL)
#define IS_ATOM_INT(ob)       (((long long)(ob)) > NOVALUE)
#define IS_ATOM_INT_NV(ob)    ((long long)(ob) >= NOVALUE)

#define MAKE_UINT(x) ((object)(( ((unsigned long long)x) <= ((unsigned long long)0x3FFFFFFFFFFFFFFFLL)) \
                          ? (unsigned long long)x : \
                            (unsigned long long)NewDouble((long double)(unsigned long long)x)))

/* these are obsolete */
#define INT_VAL(x)        ((long long int)(x))
#define MAKE_INT(x)       ((object)(x))
#else
#define NOVALUE      ((long)0xbfffffffL)
#define TOO_BIG_INT  ((long)0x40000000L)
#define HIGH_BITS    ((long)0xC0000000L)
#define IS_ATOM_INT(ob)       (((long)(ob)) > NOVALUE)
#define IS_ATOM_INT_NV(ob)    ((long)(ob) >= NOVALUE)
#define MAKE_UINT(x) MAKE_UINT32(x)
#define INT_VAL(x) INT_VAL32(x)
#define MAKE_INT(x) MAKE_INT32(x)
#endif // BITS64

/* N.B. the following distinguishes DBL's from SEQUENCES -
   must eliminate the INT case first */
#ifdef BITS64
#define IS_ATOM_DBL(ob)         (((object)(ob)) >= (long long)0xA000000000000000LL)

#define IS_ATOM(ob)             (((long long)(ob)) >= (long long)0xA000000000000000LL)
#define IS_SEQUENCE(ob)         (((long long)(ob))  < (long long)0xA000000000000000LL)

#define ASEQ(s) (((unsigned long long)s & (unsigned long long)0xE000000000000000LL) == (unsigned long long)0x8000000000000000LL)

#define IS_DBL_OR_SEQUENCE(ob)  (((long long)(ob)) < NOVALUE)
#else
#define IS_ATOM_DBL(ob)         (((object)(ob)) >= (long)0xA0000000L)

#define IS_ATOM(ob)             (((long)(ob)) >= (long)0xA0000000L)
#define IS_SEQUENCE(ob)         (((long)(ob))  < (long)0xA0000000L)

#define ASEQ(s) (((unsigned long)s & (unsigned long)0xE0000000L) == (unsigned long)0x80000000L)

#define IS_DBL_OR_SEQUENCE(ob)  (((long)(ob)) < NOVALUE)
#endif // BITS64
#define MININT32     (long)0xC0000000L
#define MAXINT32     (long)0x3FFFFFFFL
#define MININT_VAL32 MININT
#define MININT_DBL32 ((double)MININT_VAL)
#define MAXINT_VAL32 MAXINT
#define MAXINT_DBL32 ((double)MAXINT_VAL)
#define INT23      (long)0x003FFFFFL
#define INT16      (long)0x00007FFFL
#define INT15      (long)0x00003FFFL
#define ATOM_M1    -1
#define ATOM_0     0
#define ATOM_1     1
#define ATOM_2     2
#ifdef BITS64
#undef MININT
#define MININT     (long long)0xC000000000000000LL
#define MAXINT     (long long)0x3FFFFFFFFFFFFFFFLL
#define MININT_VAL MININT
#define MININT_DBL ((long double)MININT_VAL)
#define MAXINT_VAL MAXINT
#define MAXINT_DBL ((long double)MAXINT_VAL)
#define INT55      (long long)0x003FFFFFFFFFFFFFLL
#define INT47      (long long)0x00003FFFFFFFFFFFLL
#define INT31      (long long)0x000000003FFFFFFFLL
#else
#undef MININT
#define MININT MININT32
#define MAXINT MAXINT32
#define MININT_VAL MININT_VAL32
#define MININT_DBL MININT_DBL32
#define MAXINT_VAL MAXINT_VAL32
#define MAXINT_DBL MAXINT_DBL32
#endif
#ifdef BITS64
typedef long long int integer;
typedef long long object;
typedef object *object_ptr;

struct s1 {                        /* a sequence header block */
    object_ptr base;               /* pointer to (non-existent) 0th element */
    long long length;                   /* number of elements */
    long long ref;                      /* reference count */
    long long postfill;                 /* number of post-fill objects */
}; /* total 8*4=32 bytes */
#else
typedef int integer;
typedef long object;
typedef object *object_ptr;

struct s1 {                        /* a sequence header block */
    object_ptr base;               /* pointer to (non-existent) 0th element */
    long length;                   /* number of elements */
    long ref;                      /* reference count */
    long postfill;                 /* number of post-fill objects */
}; /* total 16 bytes */
#endif // BITS64
#ifdef CLEANUP_MOD
typedef void (*TypeCleanupFunc)(void *ptr, elong type, object cleanup);
#endif // CLEANUP_MOD
#ifdef USE_QUADMATH_H
#ifdef __GNUC__
#define equadmath __float128
#else
#define equadmath _Quad
#endif // __GNUC__
#endif // USE_QUADMATH_H
#define CLEANUP_DEFAULT 0
#define CLEANUP_SIGNED_TYPE 1
#define CLEANUP_UNSIGNED_TYPE 2
#define CLEANUP_DOUBLE 3
// Data is handled the same as String (non-null terminated string of length 'type')
// CStrings should still terminate their string with a null character value.
// When there are no null characters in a string, it should be null-terminated.
#define CLEANUP_DATA -1
#define CLEANUP_STRING -1
// Non MSVC compilers:
#define CLEANUP_FLOAT128 -2
#define CLEANUP_LONGDOUBLE -3
#define USER_CLEANUP_START 32
// Any negative number in 'cleanup' types means that it gets freed by the program.
#ifdef BITS64
// BEGIN 64-bit:
struct d {                         /* a long double precision number */
    union {
#ifdef USE_QUADMATH_H
	// USE_QUADMATH_H requires CLEANUP_MOD on 64-bit
	equadmath quad;
#endif
	long double dbl;                    /* long double precision value, float80 or float128 */
	struct {
	    union {
		double a_dbl; // an actual double on 64-bit platforms, i.e. MSVC compiler.
		void * ptr; // could be a pointer to an array of structures, customize in "cleanup's flags"
	    };
	    unsigned long long type; // could be length, perhaps high bits could be flags.
	};
    };
    long long ref;                      /* reference count */
// CLEANUP_MOD is required on 64-bit, because of 16 byte (128-bit) boundaries.
//#ifdef CLEANUP_MOD
// Both "struct s1" and "struct d" take up 32-bytes on 64-bit platforms.
    object cleanup; // should be set to zero (0), type of data, and pointer for the cleanup routine.
	// IS_ATOM_INT means it is a dbl==0 or "signed type"=1 "unsigned type"==2 or "double a_dbl"==3 or "data of length type"==4 or "float128"==5 (ptr on 32-bit) (other data types can be implimented in the future.)
	// IS_ATOM_DBL means it is a pointer ("ptr") of "type" with pointer to "cleanup" routine.
	// IS_SEQUENCE means it is an array ("ptr") of length "type" (without postfill variable) with pointer to "cleanup" routine.
//#endif // CLEANUP_MOD
}; /* total 8*3=24 bytes, or 8*4=32 bytes on newer GCC 64-bit */
#define D_SIZE (sizeof(struct d))
#else
// BEGIN 32-bit:
struct d {                         /* a double precision number */
    union {
	double dbl;                    /* double precision value */
	double a_dbl; // duplicate.
	struct {
	    union {
#ifdef USE_QUADMATH_H
		equadmath *pquad; // pointer to a equadmath in 32-bit
#endif // USE_QUADMATH_H
		long double *pldbl;
		void * ptr;
	    };
	    unsigned long type;
	};
    };
    long ref;                      /* reference count */
#ifdef CLEANUP_MOD
    object cleanup; // should be set to zero (0), type of data, and pointer for the cleanup routine.
	// IS_ATOM_INT means it is a dbl==0 or "signed type"=1 "unsigned type"==2 or "double a_dbl"==3 or "data of length type"==4 or "float128"==5 (ptr on 32-bit) (other data types can be implimented in the future.)
	// IS_ATOM_DBL means it is a pointer ("ptr") of "type" with pointer to "cleanup" routine.
	// IS_SEQUENCE means it is an array ("ptr") of length "type" (without postfill variable) with pointer to "cleanup" routine.
#endif // CLEANUP_MOD
}; /* total 16 bytes */
#define D_SIZE (sizeof(struct d))
#endif // BITS64
#if 0
struct free_block {                /* a free storage block */
    struct free_block *next;       /* pointer to next free block */
    long filler;
    long ref;                      /* reference count */
}; /* 12 bytes */

struct symtab_entry;

struct routine_list {   // sync with euphoria\include\euphoria.h
    char *name;
    int (*addr)();
    int seq_num;
    int file_num;
    short int num_args;
    short int convention;
};

struct ns_list {
    char *name;
    int ns_num;
    int seq_num;
    int file_num;
};
#endif // if 0
typedef struct d *d_ptr;
typedef struct s1 *s1_ptr;
#if 0
typedef struct free_block *free_block_ptr;

struct sline {      /* source line table entry */
    char *src;               /* text of line,
                                first 4 bytes used for count when profiling */
    unsigned short line;     /* line number within file */
    unsigned char file_no;   /* file number */
    unsigned char options;   /* options in effect: */
#define OP_TRACE   0x01      /* statement trace */
#define OP_PROFILE_STATEMENT 0x04  /* statement profile */
#define OP_PROFILE_TIME 0x02       /* time profile */
}; /* 8 bytes */

struct op_info {
    object (*intfn)();
    object (*dblfn)();
};

#ifdef INT_CODES
typedef int opcode_type;
#define opcode(x) (x)
#else
typedef int *opcode_type;
#define opcode(x) jumptab[x-1]
#endif
#endif // if 0
#define UNKNOWN -1

#define EXPR_SIZE 200  /* initial size of call stack */

#ifdef EBSD
#define MAX_CACHED_SIZE 0        /* don't use storage cache at all */
#else
#define MAX_CACHED_SIZE 1024     /* this size (in bytes) or less are cached
                                    Note: other vars must change if this does */
#endif
/* MACROS */
#ifdef BITS64
#define MAKE_DBL(x) ( (object) (((unsigned long long)(x) >> 3) + (long long)0xA000000000000000LL) )
#define DBL_PTR(ob) ( (d_ptr)  (((long long)(ob)) << 3) )
#define MAKE_SEQ(x) ( (object) (((unsigned long long)(x) >> 3) + (long long)0x8000000000000000LL) )
#define SEQ_PTR(ob) ( (s1_ptr) (((long long)(ob)) << 3) )
/* ref a double or a sequence (both need same 3 bit shift) */
#define RefDS(a) ++(DBL_PTR(a)->ref)

/* ref a general object */
#define Ref(a) if (IS_DBL_OR_SEQUENCE(a)) { RefDS(a); }

/* de-ref a double or a sequence */
#define DeRefDS(a) if (--(DBL_PTR(a)->ref) == 0 ) { de_reference((s1_ptr)(a)); }
/* de-ref a double or a sequence in x.c and set tpc (for time-profile) */
#define DeRefDSx(a) if (--(DBL_PTR(a)->ref) == 0 ) {tpc=pc; de_reference((s1_ptr)(a)); }

/* de_ref a sequence already in pointer form */
#define DeRefSP(a) if (--((s1_ptr)(a))->ref == 0 ) { de_reference((s1_ptr)MAKE_SEQ(a)); }

/* de-ref a general object */
#define DeRef(a) if (IS_DBL_OR_SEQUENCE(a)) { DeRefDS(a); }
/* de-ref a general object in x.c and set tpc (for time-profile) */
#define DeRefx(a) if (IS_DBL_OR_SEQUENCE(a)) { DeRefDSx(a); }

#define UNIQUE(seq) (((s1_ptr)(seq))->ref == 1)
#else
#define MAKE_DBL(x) ( (object) (((unsigned long)(x) >> 3) + (long)0xA0000000L) )
#define DBL_PTR(ob) ( (d_ptr)  (((long)(ob)) << 3) )
#define MAKE_SEQ(x) ( (object) (((unsigned long)(x) >> 3) + (long)0x80000000L) )
#define SEQ_PTR(ob) ( (s1_ptr) (((long)(ob)) << 3) )
/* ref a double or a sequence (both need same 3 bit shift) */
#define RefDS(a) ++(DBL_PTR(a)->ref)

/* ref a general object */
#define Ref(a) if (IS_DBL_OR_SEQUENCE(a)) { RefDS(a); }

/* de-ref a double or a sequence */
#define DeRefDS(a) if (--(DBL_PTR(a)->ref) == 0 ) { de_reference((s1_ptr)(a)); }
/* de-ref a double or a sequence in x.c and set tpc (for time-profile) */
#define DeRefDSx(a) if (--(DBL_PTR(a)->ref) == 0 ) {tpc=pc; de_reference((s1_ptr)(a)); }

/* de_ref a sequence already in pointer form */
#define DeRefSP(a) if (--((s1_ptr)(a))->ref == 0 ) { de_reference((s1_ptr)MAKE_SEQ(a)); }

/* de-ref a general object */
#define DeRef(a) if (IS_DBL_OR_SEQUENCE(a)) { DeRefDS(a); }
/* de-ref a general object in x.c and set tpc (for time-profile) */
#define DeRefx(a) if (IS_DBL_OR_SEQUENCE(a)) { DeRefDSx(a); }

#define UNIQUE(seq) (((s1_ptr)(seq))->ref == 1)
#endif // BITS64

/* file modes */
#define EF_CLOSED 0
#define EF_READ   1
#define EF_WRITE  2
#define EF_APPEND 4
#if 0
struct file_info {
    FILE *fptr;  // C FILE pointer
    int mode;    // file mode
};

struct arg_info {
    int (*address)();     // pointer to C function
    s1_ptr name;          // name of routine (for diagnostics)
    s1_ptr arg_size;      // s1_ptr of sequence of argument sizes
    object return_size;   // atom or sequence for return value size
    int convention;       // calling convention
};

struct IL {
    struct symtab_entry *st;
    struct sline *sl;
    int *misc;
    char *lit;
};

// Task Control Block - sync with euphoria\include\euphoria.h
struct tcb {
    int rid;         // routine id
    double tid;      // external task id
    int type;        // type of task: T_REAL_TIME or T_TIME_SHARED
    int status;      // status: ST_ACTIVE, ST_SUSPENDED, ST_DEAD
    double start;    // start time of current run
    double min_inc;  // time increment for min
    double max_inc;  // time increment for max
    double min_time; // minimum activation time
                     // or number of executions remaining before sharing
    double max_time; // maximum activation time (determines task order)
    int runs_left;   // number of executions left in this burst
    int runs_max;    // maximum number of executions in one burst
    int next;        // index of next task of the same kind
    object args;     // args to call task procedure with at startup
    int *pc;         // program counter for this task
    object_ptr expr_stack; // call stack for this task
    object_ptr expr_max;   // current top limit of stack
    object_ptr expr_limit; // don't start a new routine above this
    object_ptr expr_top;   // stack pointer
    int stack_size;        // current size of stack
};

// saved private blocks
struct private_block {
   int task_number;            // internal task number
   struct private_block *next; // pointer to next block
   object block[1];            // variable-length array of saved private data
};
#endif // if 0
#ifdef ELINUX
#define MAX_LINES 100
#define MAX_COLS 200
struct char_cell {
    char ascii;
    char fg_color;
    char bg_color;
};
#endif

#define MAX_FILE_NAME 255

#define CONTROL_C 3

#define COLOR_DISPLAY   (config.monitor != _MONO &&         \
                         config.monitor != _ANALOGMONO &&   \
                         config.mode != 7 && config.numcolors >= 16)

#define TEXT_MODE (config.numxpixels <= 80 || config.numypixels <= 80)

#define TEMP_SIZE 1040  // size of TempBuff - big enough for 1024 image

#define EXTRA_EXPAND(x) (4 + (x) + ((x) >> 2))

#define DEFAULT_SAMPLE_SIZE 25000

#define MAX_BITWISE_DBL32 ((double)(unsigned long)0xFFFFFFFFL)
#define MIN_BITWISE_DBL32 ((double)(signed long)  0x80000000L)
#ifdef BITS64
#define MAX_BITWISE_DBL ((long double)(unsigned long long)0xFFFFFFFFFFFFFFFFLL)
#define MIN_BITWISE_DBL ((long double)(signed long long)  0x8000000000000000LL)
#else
#define MAX_BITWISE_DBL MAX_BITWISE_DBL32
#define MIN_BITWISE_DBL MIN_BITWISE_DBL32
#endif // BITS64

/* .dll argument & return value types */
#define C_TYPE     0x0F000000
#define C_DOUBLE   0x03000008
#define C_FLOAT    0x03000004
#define C_CHAR     0x01000001
#define C_UCHAR    0x02000001
#define C_SHORT    0x01000002
#define C_USHORT   0x02000002
#define E_INTEGER  0x06000004
#define E_ATOM     0x07000004
#define E_SEQUENCE 0x08000004
#define E_OBJECT   0x09000004
#define C_INT      0x01000004
#define C_UINT     0x02000004
#define C_LONG     0x01000008
#define C_ULONG    0x02000008
#define C_POINTER  0x03000001
#define C_LONGLONG 0x03000002

#define C_FLOAT80  0x0300000A
#define C_FLOAT128 0x03000010
#define E_FLOAT128 0x0A000010

#define C_STDCALL 0
#define C_CDECL 1

// multitasking
#define ST_ACTIVE 0
#define ST_SUSPENDED 1
#define ST_DEAD 2

#define LOW_MEMORY_MAX ((unsigned)0x0010FFEF)
// It used to be ((unsigned)0x000FFFFF)
// but I found putsxy accessing ROM font above here
// successfully on Millennium. It got the address from a DOS interrupt.

#define DOING_SPRINTF -9999999 // indicates sprintf rather than printf

// alloc.h
#define FreeD(p) free(p)

// be_alloc.c
#define EFree(p) free(p)

#endif // _COMMON_H
