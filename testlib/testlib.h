
//testlib.h

// Assume this symbol is only defined by your DLL project, so we can either
// export or import the symbols as appropriate
#if COMPILING_MY_TEST_DLL
#define TESTLIB_EXPORT __declspec(dllexport)
#else
#define TESTLIB_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

TESTLIB_EXPORT void hello();
// ... more function declarations, marked with TESTLIB_EXPORT

#ifdef __cplusplus
}
#endif
