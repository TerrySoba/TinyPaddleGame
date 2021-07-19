#ifndef _INCLUDE_COMPILER_VERSION_H_
#define _INCLUDE_COMPILER_VERSION_H_

#define xstr(s) str(s)
#define str(s) #s

#if defined(__GNUC__)
#define COMPILER_VERSION "GCC " xstr(__GNUC__) "." xstr(__GNUC_MINOR__) "." xstr(__GNUC_PATCHLEVEL__)
#endif

#if defined(__WATCOMC__)
#define COMPILER_VERSION "OpenWatcom C " xstr(__WATCOMC__)
#endif

#endif
