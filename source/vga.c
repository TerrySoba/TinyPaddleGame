#include "vga.h"

#if defined(__WATCOMC__)
#include <conio.h>
#include <string.h>


#define inport(px) inpw(px)
#define inportb(px) inp(px)

#endif

#define abs(val) (((val) > 0)?(val):-(val))


#if defined(__GNUC__)
void videoInit(uint8_t mode)
{
    asm("mov $0, %%ah\n"
        "int $0x10"
        : 
        : "Ral" (mode)
        : "ah"
    );
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

#if defined(__WATCOMC__)
extern void drawVLine(uint16_t x, uint8_t y, uint8_t h, uint8_t color);
#pragma aux drawVLine = \
    "mov ch, 20"        \
    "mul ch"            \
    "xor ch, ch"        \
    "add ax, 40960"     \
    "push ds"           \
    "mov ds, ax"        \
    "draw_loop:"        \
    "mov [bx], dl"      \
    "add bx, 320"       \
    "loop draw_loop"    \
    "pop ds"            \
    modify [ch ax]      \
    parm [bx] [al] [cl] [dl];
#endif

#if defined(__GNUC__)
inline void drawVLine(uint16_t x, uint8_t y, uint8_t h, uint8_t color)
{
    asm (
        "mov $20, %%ch\n"     
        "mul %%ch\n"          // multiply ax = al * ch
        "xor %%ch, %%ch\n"    // ch = 0
        "add $40960, %%ax\n"  // $40960 is the address of the VGA graphics ram
        "mov %%ax, %%ds\n"    // set segment register to VGA
        "%=:\n"
        "mov %%dl, (%%bx)\n"  // draw pixel to vram
        "add $320, %%bx\n"    // skip to next line (screen is 320 pixels wide)
        "loop %=b\n"         // loop until cx is 0 (loop decrements cx each loop)
        :
        : "b" (x), "Ral" (y), "Rdl" (color), "Rcl" (h)
        : "ds", "ah", "ch"
    );
}

inline void drawRectEven(uint16_t x, uint8_t y, uint16_t w, uint8_t h, uint8_t color)
{
    uint16_t offset = 20 * y;
    asm  (
        "add $40960, %%bx\n"  // $40960 is the address of the VGA graphics ram
        "mov %%bx, %%es\n"    // set segment register to VGA
        "mov %%al, %%ah\n"    // set color
        "cld\n"               // set DF to 0
        "shrw $1, %1\n"       // divide length by 2
        "%=:\n"
        "mov %0, %%di\n"      // set offset to x
        "mov %1, %%cx\n"      // set length to w
        "rep stosw\n"         // copy ax -> es:di++ until cx-- == 0

        "add $20, %%bx\n"     // skip to next line (320 / 16 = 20)
        "mov %%bx, %%es\n"    
        "dec %4\n"            // decrement h
        "jnz %=b\n"           // loop until h is 0
        :
        : "r" (x), "m" (w), "b" (offset), "Ral" (color), "r" (h)
        : "es", "ah", "cx", "di"
    );
}

inline void drawRectOdd(uint16_t x, uint8_t y, uint16_t w, uint8_t h, uint8_t color)
{
    uint16_t offset = 20 * y;
    asm  (
        "add $40960, %%bx\n"  // $40960 is the address of the VGA graphics ram
        "mov %%bx, %%es\n"    // set segment register to VGA
        "mov %%al, %%ah\n"    // set color
        "cld\n"               // set DF to 0
        "shrw $1, %1\n"       // divide length by 2
        "%=:\n"
        "mov %0, %%di\n"      // set offset to x
        "mov %1, %%cx\n"      // set length to w
        "rep stosw\n"         // copy ax -> es:di++ until cx-- == 0
        "stosb\n"             // store an additional byte as width is odd

        "add $20, %%bx\n"     // skip to next line (320 / 16 = 20)
        "mov %%bx, %%es\n"    
        "dec %4\n"            // decrement h
        "jnz %=b\n"           // loop until h is 0
        :
        : "r" (x), "m" (w), "b" (offset), "Ral" (color), "r" (h)
        : "es", "ah", "cx", "di"
    );
}


void drawRect(uint16_t x, uint8_t y, uint16_t w, uint8_t h, uint8_t color)
{
    if (w == 1)   drawVLine(x,y,h,color);
    else if (w&1) drawRectOdd(x,y,w,h,color);
    else          drawRectEven(x,y,w,h,color);
}

#endif

#if defined(__WATCOMC__)
void drawRect(uint16_t x, uint8_t y, uint16_t w, uint8_t h, uint8_t color)
{
    if (w == 1)
    {
        drawVLine(x,y,h,color);
    }
    else
    {
        static char far* screen = (char far*)(0xA0000000L);
        char far* pos = screen + y * SCREEN_W + x;
        for (uint8_t i = 0; i < h; ++i)
        {
            _fmemset(pos, color, w);
            pos += SCREEN_W;
        }
    }
}
#endif

void clearScreen()
{
    drawRect(0, 0, SCREEN_W, SCREEN_H, 0);
}


void drawPixel(int x, int y, uint8_t color)
{
#if defined(__WATCOMC__)
    char far* screen = (char far*)(0xA0000000L);
#elif defined(__GNUC__)
    char __far* screen = (char __far*)(0xA0000000L);
#endif
    screen[y * 320 + x] = color;
}


void drawLine(int x0, int y0, int x1, int y1, uint8_t color) {
 
  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    drawPixel(x0,y0,color);
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}


void drawHLine(int16_t x, uint8_t y, int16_t length, uint8_t color)
{
#if defined(__GNUC__)
    uint16_t offset = 20 * y;
    asm (
        "add $40960, %%bx\n"  // $40960 is the address of the VGA graphics ram
        "mov %%bx, %%es\n"    // set segment register to VGA
        "mov %%al, %%ah\n"    // set color
        "cld\n"               // set DF to 0
        "shrw $1, %1\n"       // divide length by 2
        "mov %3, %%di\n"
        "rep stosw\n"         // copy ax -> es:di++ until cx-- == 0
        "jnc %=f\n"
        "stosb\n"             // store an additional byte as width is odd
        "%=:\n"
        :
        : "b" (offset), "c" (length), "Ral" (color), "m" (x)
        : "ds", "ah", "di", "es"
    );
#elif defined(__WATCOMC__)
    char far* screen = (char far*)(0xA0000000L);
    screen += y * 320 + x;
    _fmemset(screen, color, length);
#endif

}


void drawCircle(int x0, int y0, int radius, uint8_t color, bool filled)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;
 
    drawPixel(x0, y0 + radius, color);
    drawPixel(x0, y0 - radius, color);
    drawPixel(x0 + radius, y0, color);
    drawPixel(x0 - radius, y0, color);
 
    if (filled)
    {
        drawHLine(x0 - radius, y0, 2 * radius + 1, color);
    }

    while(x < y) 
    {
        if(f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        if (filled)
        {
            drawHLine(x0 - x, y0 + y, 2 * x + 1, color);
            drawHLine(x0 - x, y0 - y, 2 * x + 1, color);
            drawHLine(x0 - y, y0 + x, 2 * y + 1, color);
            drawHLine(x0 - y, y0 - x, 2 * y + 1, color);
        }
        else // not filled
        { 
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 - x, y0 + y, color);
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 - x, y0 - y, color);
            drawPixel(x0 + y, y0 + x, color);
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 + y, y0 - x, color);
            drawPixel(x0 - y, y0 - x, color);
        }
    }
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
