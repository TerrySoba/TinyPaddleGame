#include "vga.h"


void videoInit(uint8_t mode)
{
    asm volatile
       ("mov %0, %%al\n"
        "mov $0, %%ah\n"
        "int $0x10"
        : 
        : "r" (mode)
        : "ax");
}


inline void drawRectEven(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{
    w = w >> 1; // w = w/2

    uint16_t offset = 20 * y;
    asm ("mov $40960, %%ax\n" // $40960 is the address of the VGA graphics ram
        "add %2, %%ax\n"
        "mov %%ax, %%ds\n"    // set segment register to VGA
        "mov %3, %%dl\n"      // set color
        "mov %3, %%dh\n"      // set color
        "1%=:\n"
        "mov %0, %%bx\n"      // set offset to x
        "mov %1, %%cx\n"      // set length to w
        "2%=:\n"
        "mov %%dx, (%%bx)\n"  // draw pixel to vram
        "add $2, %%bx\n"      // increment pixel position
        "loop 2%=b\n"         // execute loop until cx is 0 (cx = cx - 1) 

        "add $20, %%ax\n"     // skip to next line (320 / 16 = 20)
        "mov %%ax, %%ds\n"
        "sub $1, %4\n"        // decrement h
        "jnz 1%=b\n"          // loop until h is 0
        :
        : "m" (x), "m" (w), "r" (offset), "m" (color), "r" (h)
        : "ds", "ax", "bx", "cx", "dx");
}

inline void drawRectOdd(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{
    w = w >> 1; // w = w/2

    uint16_t offset = 20 * y;
    asm ("mov $40960, %%ax\n" // $40960 is the address of the VGA graphics ram
        "add %2, %%ax\n"
        "mov %%ax, %%ds\n"    // set segment register to VGA
        "mov %3, %%dl\n"      // set color
        "mov %3, %%dh\n"      // set color
        "1%=:\n"
        "mov %0, %%bx\n"      // set offset to x
        "mov %1, %%cx\n"      // set length to w
        "2%=:\n"
        "mov %%dx, (%%bx)\n"  // draw pixel to vram
        "add $2, %%bx\n"      // increment pixel position
        "loop 2%=b\n"         // execute loop until cx is 0 (cx = cx - 1) 
        "mov %%dl, (%%bx)\n"  // draw extra pixel to vram <---- THIS IS THE DIFFERENCE!!!!

        "add $20, %%ax\n"     // skip to next line (320 / 16 = 20)
        "mov %%ax, %%ds\n"
        "sub $1, %4\n"        // decrement h
        "jnz 1%=b\n"          // loop until h is 0
        :
        : "m" (x), "m" (w), "r" (offset), "m" (color), "r" (h)
        : "ds", "ax", "bx", "cx", "dx");
}


void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{

    if (w&1) drawRectEven(x,y,w,h,color);
    else     drawRectOdd(x,y,w,h,color);

    // static char __far* screen = (char __far*)(0xA0000000L);
    // for (int i = 0; i < h; ++i)
    // {
    //     char __far *drawPos = screen + (y + i) * SCREEN_W + x;
    //     for (int n = 0; n < w; ++n)
    //     {
    //         *drawPos++ = color;
    //     }
    // }
}


void clearScreen()
{
    drawRect(0, 0, SCREEN_W, SCREEN_H, 0);
}



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

#define CGA_STATUS_REG 0x03DA
#define VERTICAL_RETRACE_BIT 0x08

void waitForVSync()
{
    while (inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT);
    while (!(inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT));
}
