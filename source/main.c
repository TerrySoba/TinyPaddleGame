#include <stdint.h>

#include "vga.h"
#include "keyboard.h"


#define abs(x) ((x > 0)?(x):(-x))


typedef struct _Paddle
{
    int16_t x, y, w, h;
} Paddle;


void drawPaddle(const Paddle* paddle, uint8_t color)
{
    drawRect(paddle->x, paddle->y, paddle->w, paddle->h, color);
}


void printString(const char* str)
{
    while (*str)
    {
        asm volatile(
            "mov %0, %%dl\n"     // load character to print to dl
            "mov $0x02, %%ah\n"  // set ah to 02 == print character
            "int $0x21"          // call dos interrupt 0x21 to print character
            :
            : "m" (*str)
            : "dx", "ax"
        );
        ++str;
    }
}

int main()
{
    registerKeyboardHandler();
    videoInit(0x13);

    clearScreen();

    int16_t ballX = 0;
    int16_t ballY = 0;

    int16_t dx = 4;
    int16_t dy = 3;


    Paddle leftPaddle = {10,10,5,40};
    Paddle rightPaddle = {300,10,5,40};

    while (!s_keyEsc)
    {
        waitForVSync();
        // first delete old gfx
        drawRect(ballX, ballY, 4, 3, 0);
        drawPaddle(&leftPaddle, 0);
        drawPaddle(&rightPaddle, 0);

        // handle inputs
        if (s_keyQ) leftPaddle.y -= 2;
        if (s_keyA) leftPaddle.y += 2;
        if (leftPaddle.y < 0) leftPaddle.y = 0;
        if (leftPaddle.y > SCREEN_H - leftPaddle.h) leftPaddle.y = SCREEN_H - leftPaddle.h;

        if (s_keyUp) rightPaddle.y -= 2;
        if (s_keyDown) rightPaddle.y += 2;
        if (rightPaddle.y < 0) rightPaddle.y = 0;
        if (rightPaddle.y > SCREEN_H - rightPaddle.h) rightPaddle.y = SCREEN_H - rightPaddle.h;


        // now calculate physics
        ballX += dx;
        ballY += dy;

        if (ballX > SCREEN_W)
        {
            dx = -abs(dx);
            ballX = SCREEN_W;
        }
        if (ballX < 0)
        {
            dx = abs(dx);
            ballX = 0;
        }
        if (ballY > SCREEN_H)
        {
            dy = -abs(dy);
            ballY = SCREEN_H;
        }
        if (ballY < 0)
        {
            dy = abs(dy);
            ballY = 0;
        }

        // now draw gfx
        drawRect(ballX, ballY, 4, 3, 0xf);
        drawPaddle(&leftPaddle, 0xf);
        drawPaddle(&rightPaddle, 0xf);
    }

    unRegisterKeyboardHandler();
    videoInit(0x3);

    
    printString("Thanks for playing!\r\n");

    return 0;
}
