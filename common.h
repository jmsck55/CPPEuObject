// Copyright (c) 2022 James Cook
// common.h
//
// 32/64-bit using macro BITS64 for 64-bit
//
// Included by pch.h

#ifndef _COMMON_H
#define _COMMON_H

// Begin Platform
// These macros are set in specify_platform.h

// If under Windows, else comment out.
//#define EWINDOWS

// Define this macro (DONE_DEBUGGING) before distributing software:
//#define DONE_DEBUGGING

// If you want to specify these macros on the command line, comment out the next line:
//#include "specify_platform.h"

// End Platform


// For faster code, alignment should be (2 on 16-bit machines), (4 on 32-bit machines), (8 on 64-bit machines)
#ifdef BITS64
//#pragma align(8)
#define eudouble long double
#define eulong long long
#define REGISTER
#else
//#pragma align(4)
#define eudouble double
#define eulong long
#define REGISTER register
#endif
#ifdef BITS64
#define ELONG_WIDTH "ll"
#else
#define ELONG_WIDTH "l"
#endif

#ifdef DONE_DEBUGGING
#undef EXTRA_CHECK
//#define ERUNTIME
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

#ifdef BITS64
#define NOVALUE      ((long long)0xbfffffffffffffffLL)
#define TOO_BIG_INT  ((long long)0x4000000000000000LL)
#define HIGH_BITS    ((long long)0xC000000000000000LL)
#define IS_ATOM_INT(ob)       (((long long)(ob)) > NOVALUE)
#define IS_ATOM_INT_NV(ob)    ((long long)(ob) >= NOVALUE)

#define MAKE_UINT(x) ((object)(( ((unsigned long long)x) <= ((unsigned long long)0x3FFFFFFFFFFFFFFFLL)) \
                          ? (unsigned long long)x : \
                            (unsigned long long)NewDouble((long double)(unsigned long long)x)))
#else
#define NOVALUE      ((long)0xbfffffffL)
#define TOO_BIG_INT  ((long)0x40000000L)
#define HIGH_BITS    ((long)0xC0000000L)
#define IS_ATOM_INT(ob)       (((long)(ob)) > NOVALUE)
#define IS_ATOM_INT_NV(ob)    ((long)(ob) >= NOVALUE)

#define MAKE_UINT(x) ((object)(( ((unsigned long)x) <= ((unsigned long)0x3FFFFFFFL)) \
                          ? (unsigned long)x : \
                            (unsigned long)NewDouble((double)(unsigned long)x)))
#endif
/* these are obsolete */
#ifdef BITS64
#define INT_VAL(x)        ((long long int)(x))
#else
#define INT_VAL(x)        ((int)(x))
#endif
#define MAKE_INT(x)       ((object)(x))

/* N.B. the following distinguishes DBL's from SEQUENCES -
   must eliminate the INT case first */
#ifdef BITS64
#define IS_ATOM_DBL(ob)         (((object)(ob)) >= (long long)0xA000000000000000LL)

#define IS_ATOM(ob)             (((long long)(ob)) >= (long long)0xA000000000000000LL)
#define IS_SEQUENCE(ob)         (((long long)(ob))  < (long long)0xA000000000000000LL)

#define ASEQ(s) (((unsigned long long)s & (unsigned long long)0xE000000000000000LL) == (unsigned long long)0x8000000000000000LL)

#define IS_DBL_OR_SEQUENCE(ob)  (((long long)(ob)) < NOVALUE)
#else
#define IS_ATOM_DBL(ob)         (((object)(ob)) >= (long)0xA0000000)

#define IS_ATOM(ob)             (((long)(ob)) >= (long)0xA0000000)
#define IS_SEQUENCE(ob)         (((long)(ob))  < (long)0xA0000000)

#define ASEQ(s) (((unsigned long)s & (unsigned long)0xE0000000) == (unsigned long)0x80000000)

#define IS_DBL_OR_SEQUENCE(ob)  (((long)(ob)) < NOVALUE)
#endif
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
#define INT23      (long)0x003FFFFFL
#define INT16      (long)0x00007FFFL
#define INT15      (long)0x00003FFFL
#define ATOM_M1    -1
#define ATOM_0     0
#define ATOM_1     1
#define ATOM_2     2
#else
#undef MININT
#define MININT     (long)0xC0000000
#define MAXINT     (long)0x3FFFFFFF
#define MININT_VAL MININT
#define MININT_DBL ((double)MININT_VAL)
#define MAXINT_VAL MAXINT
#define MAXINT_DBL ((double)MAXINT_VAL)
#define INT23      (long)0x003FFFFFL
#define INT16      (long)0x00007FFFL
#define INT15      (long)0x00003FFFL
#define ATOM_M1    -1
#define ATOM_0     0
#define ATOM_1     1
#define ATOM_2     2
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

struct d {                         /* a double precision number */
    long double dbl;                    /* double precision value */
    long long ref;                      /* reference count */
}; /* total 8*3=24 bytes */
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

struct d {                         /* a double precision number */
    double dbl;                    /* double precision value */
    long ref;                      /* reference count */
}; /* total 12 bytes */
#endif
#define D_SIZE (sizeof(struct d))
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
typedef struct d  *d_ptr;
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
#else
#define MAKE_DBL(x) ( (object) (((unsigned long)(x) >> 3) + (long)0xA0000000) )
#define DBL_PTR(ob) ( (d_ptr)  (((long)(ob)) << 3) )
#define MAKE_SEQ(x) ( (object) (((unsigned long)(x) >> 3) + (long)0x80000000) )
#define SEQ_PTR(ob) ( (s1_ptr) (((long)(ob)) << 3) )
#endif
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
#ifdef BITS64
#define MAX_BITWISE_DBL ((long double)(unsigned long long)0xFFFFFFFFFFFFFFFFLL)
#define MIN_BITWISE_DBL ((long double)(signed long long)  0x8000000000000000LL)

/* .dll argument & return value types */
#define C_TYPE     0x0F00000000000000LL
#define C_DOUBLE   0x0300000000000008LL
#define C_FLOAT    0x0300000000000004LL
#define C_CHAR     0x0100000000000001LL
#define C_UCHAR    0x0200000000000001LL
#define C_SHORT    0x0100000000000002LL
#define C_USHORT   0x0200000000000002LL
#define E_INTEGER  0x0600000000000004LL
#define E_ATOM     0x0700000000000004LL
#define E_SEQUENCE 0x0800000000000004LL
#define E_OBJECT   0x0900000000000004LL

#define E_DBL      0x0500000000000004LL
#define E_FLOAT128 0x0A00000000000004LL
#else
#define MAX_BITWISE_DBL ((double)(unsigned long)0xFFFFFFFF)
#define MIN_BITWISE_DBL ((double)(signed long)  0x80000000)

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

#define E_DBL      0x05000004
#define E_FLOAT128 0x0A000004
#endif
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
