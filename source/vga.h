#ifndef _VGA_H_INCLUDED
#define _VGA_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_W 320
#define SCREEN_H 200

void videoInit(uint8_t mode);
void drawRect(uint16_t x, uint8_t y, uint16_t w, uint8_t h, uint8_t color);
void drawLine(int x0, int y0, int x1, int y1, uint8_t color);
void drawHLine(int16_t x, uint8_t y, int16_t length, uint8_t color);
void drawCircle(int centerX, int centerY, int radius, uint8_t color, bool filled);
void clearScreen();
void waitForVSync();

#endif
