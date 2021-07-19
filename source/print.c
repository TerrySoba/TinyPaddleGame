#include "print.h"


#if defined(__WATCOMC__)

extern void _printChar(char ch);
#pragma aux _printChar =    \
    "mov ah, 02h"      \
    "int 21h"          \
    modify [ah]        \
    parm [dl];
#endif

void printString(const char* str)
{
    while (*str)
    {
        #if defined(__GNUC__)
        asm (
            "mov $0x02, %%ah\n"  // set ah to 02 == print character
            "int $0x21"          // call dos interrupt 0x21 to print character
            :
            : "Rdl" (*str)       // load character to print to dl
            : "ah"
        );
        #endif

        #if defined(__WATCOMC__)
        _printChar(*str);
        #endif
        ++str;
    }
}
