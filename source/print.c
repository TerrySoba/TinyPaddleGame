#include "print.h"


#if defined(__WATCOMC__)

extern void _printString(const char* str);
#pragma aux _printString =    \
    "mov ah, 09h"      \
    "int 21h"          \
    modify [ah]        \
    parm [dx];
#endif

void printString(const char* str)
{
    if (str)
    {
        #if defined(__GNUC__)
        asm (
            "mov $0x09, %%ah\n"  // set ah to 09 == print string
            "int $0x21"          // call dos interrupt 0x21 to print string
            :
            : "d" (str)          // load string to print to dx
            : "ah"
        );
        #endif

        #if defined(__WATCOMC__)
        _printString(str);
        #endif
    }
}
