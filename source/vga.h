#ifndef _VGA_H_INCLUDED
#define _VGA_H_INCLUDED

#include <stdint.h>

#define SCREEN_W 320
#define SCREEN_H 200

void videoInit(uint8_t mode);
void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color);
void clearScreen();
void waitForVSync();

#endif
