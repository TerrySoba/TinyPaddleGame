#include <stdint.h>

#include "vga.h"
#include "draw_number.h"
#include "keyboard.h"
#include "sound_engine.h"
#include "print.h"
#include "compiler_version.h"

#define abs(x) ((x > 0)?(x):(-x))

typedef struct _Paddle
{
    int16_t x, y, w, h;
} Paddle;


void drawPaddle(const Paddle* paddle, uint8_t color)
{
    drawRect(paddle->x, paddle->y, paddle->w, paddle->h, color);
}


#define PLAYFIELD_H 177
#define PLAYFIELD_W 317

int main()
{
    SoundContext soundContext;
    initSound(&soundContext);

    registerKeyboardHandler();
    videoInit(0x13);

    int16_t dx = 4;
    int16_t dy = 3;

    Paddle leftPaddle = {10,10,5,40};
    Paddle rightPaddle = {300,10,5,40};

    int leftColor = 3;
    int rightColor = 4;

    do 
    {
        int leftScore = 0;
        int rightScore = 0;
        int leftScore_old = -1;
        int rightScore_old = -1;

        int16_t ballX = PLAYFIELD_W / 2;
        int16_t ballY = PLAYFIELD_H / 2;

        clearScreen();
        drawRect(0, 180, 320, 3, 8);

        do
        {
            processSound(&soundContext);
            waitForVSync();
            // first delete old gfx
            drawRect(ballX, ballY, 3, 3, 0);
            drawPaddle(&leftPaddle, 0);
            drawPaddle(&rightPaddle, 0);


            // handle inputs
            if (s_keyQ) leftPaddle.y -= 2;
            if (s_keyA) leftPaddle.y += 2;
            if (leftPaddle.y < 0) leftPaddle.y = 0;
            if (leftPaddle.y > PLAYFIELD_H - leftPaddle.h) leftPaddle.y = PLAYFIELD_H - leftPaddle.h;

            if (s_keyUp) rightPaddle.y -= 2;
            if (s_keyDown) rightPaddle.y += 2;
            if (rightPaddle.y < 0) rightPaddle.y = 0;
            if (rightPaddle.y > PLAYFIELD_H - rightPaddle.h) rightPaddle.y = PLAYFIELD_H - rightPaddle.h;


            // now calculate physics
            ballX += dx;
            ballY += dy;

            if (ballX >= rightPaddle.x && ballY >= rightPaddle.y && ballY <= rightPaddle.y + rightPaddle.h)
            {
                dx = -abs(dx);
                playNote(&soundContext, G4, 70);
            }
            if (ballX > PLAYFIELD_W)
            {
                dx = -abs(dx);
                ballX = PLAYFIELD_W;
                ++leftScore;
                playNote(&soundContext, D4b, 70);
            }
            if (ballX <= leftPaddle.x && ballY >= leftPaddle.y && ballY <= leftPaddle.y + leftPaddle.h)
            {
                dx = abs(dx);
                playNote(&soundContext, G4b, 70);
            }
            if (ballX < 0)
            {
                dx = abs(dx);
                ballX = 0;
                ++rightScore;
                playNote(&soundContext, D4b, 70);
            }
            if (ballY > PLAYFIELD_H)
            {
                dy = -abs(dy);
                ballY = PLAYFIELD_H;
            }
            if (ballY < 0)
            {
                dy = abs(dy);
                ballY = 0;
            }

            // now draw gfx
            
            drawRect(ballX, ballY, 3, 3, 0xf);
            drawPaddle(&leftPaddle, leftColor);
            drawPaddle(&rightPaddle, rightColor);

            // now draw score if it changed
            if (leftScore != leftScore_old || 
                rightScore != rightScore_old)
            {
                drawNumber(30, 185, 0, 1, leftScore_old);
                drawNumber(280, 185, 0, 1, rightScore_old);
                drawNumber(30, 185, leftColor, 1, leftScore);
                drawNumber(280, 185, rightColor, 1, rightScore);
            }

            leftScore_old = leftScore;
            rightScore_old = rightScore;
        } while (leftScore < 10 && rightScore < 10 && !s_keyEsc);

        int color;

        if (leftScore > rightScore) color = 3;
        else color = 4;

        drawNumber(70, 30, color, 10, 1);
        drawNumber(150, 30, color, 10, 0);

        while (!s_keyEsc && !s_keySpace);

    } while (!s_keyEsc);

    unRegisterKeyboardHandler();
    videoInit(0x3);

    deInitSound(&soundContext);

    printString("Thanks for playing!\r\nBuilt using " COMPILER_VERSION "\r\n");

    return 0;
}
