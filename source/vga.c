#include "vga.h"

#if defined(__WATCOMC__)
#include <conio.h>
#include <string.h>
#define inport(px) inpw(px)
#define inportb(px) inp(px)

#endif


#if defined(__GNUC__)
void videoInit(uint8_t mode)
{
    asm("mov %0, %%al\n"
        "mov $0, %%ah\n"
        "int $0x10"
        : 
        : "m" (mode)
        : "ax");
}
#endif

#if defined(__WATCOMC__)

extern void _videoInit(uint8_t mode);
#pragma aux _videoInit =    \
    "mov ah, 0"        \
    "int 10h"          \
    modify [ax]        \
    parm [al];

void videoInit(uint8_t mode)
{
    _videoInit(mode);
}
#endif


#if defined(__GNUC__)
inline void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint8_t color)
{
    uint16_t offset = 20 * y; // (320 / 16 == 20)
    asm ("mov $40960, %%ax\n" // $40960 is the address of the VGA graphics ram
        "add %1, %%ax\n"
        "mov %%ax, %%ds\n"    // set segment register to VGA
        "mov %2, %%dl\n"      // set color
        "mov %0, %%bx\n"      // set offset to x
        "mov %3, %%cx\n"      // initialize loop counter cx
        "%=:\n"
        "mov %%dl, (%%bx)\n"  // draw pixel to vram
        "add $320, %%bx\n"    // skip to next line (screen is 320 pixels wide)
        "loop %=b\n"         // loop until cx is 0 (loop decrements cx each loop)
        :
        : "m" (x), "r" (offset), "m" (color), "r" (h)
        : "ds", "ax", "bx", "cx", "dl");
}

inline void drawRectEven(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{
    uint16_t offset = 20 * y;
    asm  ("mov $40960, %%bx\n" // $40960 is the address of the VGA graphics ram
        "add %2, %%bx\n"
        "mov %%bx, %%es\n"    // set segment register to VGA
        "mov %3, %%al\n"      // set color
        "mov %3, %%ah\n"      // set color
        "cld\n"               // set DF to 0
        "%=:\n"
        "mov %0, %%di\n"      // set offset to x
        "mov %1, %%cx\n"      // set length to w
        "shrw $1, %%cx\n"     // divide length by 2
        "rep stosw\n"         // copy ax -> es:di++ until cx-- == 0

        "add $20, %%bx\n"     // skip to next line (320 / 16 = 20)
        "mov %%bx, %%es\n"    
        "sub $1, %4\n"        // decrement h
        "jnz %=b\n"          // loop until h is 0
        :
        : "m" (x), "m" (w), "r" (offset), "m" (color), "r" (h)
        : "es", "ax", "cx", "di", "bx");
}

inline void drawRectOdd(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{
    uint16_t offset = 20 * y;
    asm  ("mov $40960, %%bx\n" // $40960 is the address of the VGA graphics ram
        "add %2, %%bx\n"
        "mov %%bx, %%es\n"    // set segment register to VGA
        "mov %3, %%al\n"      // set color
        "mov %3, %%ah\n"      // set color
        "cld\n"               // set DF to 0
        "%=:\n"
        "mov %0, %%di\n"      // set offset to x
        "mov %1, %%cx\n"      // set length to w
        "shrw $1, %%cx\n"     // divide length by 2
        "rep stosw\n"         // copy ax -> es:di++ until cx-- == 0
        "stosb\n"             // store an additional byte as width is odd

        "add $20, %%bx\n"     // skip to next line (320 / 16 = 20)
        "mov %%bx, %%es\n"    
        "sub $1, %4\n"        // decrement h
        "jnz %=b\n"          // loop until h is 0
        :
        : "m" (x), "m" (w), "r" (offset), "m" (color), "r" (h)
        : "es", "ax", "cx", "di", "bx");
}


void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{
    if (w == 1)   drawVLine(x,y,h,color);
    else if (w&1) drawRectOdd(x,y,w,h,color);
    else          drawRectEven(x,y,w,h,color);
}

#endif

#if defined(__WATCOMC__)
void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{
    static char far* screen = (char far*)(0xA0000000L);
    char far* pos = screen + y * SCREEN_W + x;
    for (int i = 0; i < h; ++i)
    {
        _fmemset(pos, color, w);
        pos += SCREEN_W;
    }
}
#endif


void clearScreen()
{
    drawRect(0, 0, SCREEN_W, SCREEN_H, 0);
}


#if defined(__GNUC__)
/* Input a byte from a port */
inline unsigned char inportb(unsigned int port)
{
   unsigned char ret;
   asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
   return ret;
}


/* Output a byte to a port */
/* July 6, 2001 added space between :: to make code compatible with gpp */

inline void outportb(unsigned int port,unsigned char value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

#endif


#define CGA_STATUS_REG 0x03DA
#define VERTICAL_RETRACE_BIT 0x08

void waitForVSync()
{
    while (inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT);
    while (!(inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT));
}
