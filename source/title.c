#include "title.h"
#include "vga.h"

#include <stdint.h>

struct RectangleColor
{
    int16_t x,y,w,h;
    uint8_t color;
};

#include "title.inc"

void drawTitle()
{
    for (int i = 0; i < titleGfxSize; ++i)
    {
        drawRect(titleGfx[i].x, titleGfx[i].y, titleGfx[i].w, titleGfx[i].h, titleGfx[i].color);
    }
}
