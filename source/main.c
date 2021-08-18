#include <stdint.h>

#include "vga.h"
#include "draw_number.h"
#include "keyboard.h"
#include "sound_engine.h"
#include "print.h"
#include "compiler_version.h"
#include "title.h"

#define abs(x) ((x > 0)?(x):(-x))
#define TO_SUBPIXELS(val) ((val) << 4)
#define FROM_SUBPIXELS(val) ((val) >> 4)

typedef struct _Rectangle
{
    int16_t x, y, w, h;
} Rectangle;


void drawRectangle(const Rectangle* rectangle, uint8_t color)
{
    drawRect(
        FROM_SUBPIXELS(rectangle->x),
        FROM_SUBPIXELS(rectangle->y),
        FROM_SUBPIXELS(rectangle->w),
        FROM_SUBPIXELS(rectangle->h),
        color);
}



#define PLAYFIELD_H 180
#define PLAYFIELD_W 320

bool calculateCollision(const Rectangle* r1, const Rectangle* r2)
{
    if ( r1->x < r2->x + r2->w &&
         r1->x + r1->w > r2->x &&
         r1->y < r2->y + r2->h &&
         r1->y + r1->h > r2->y)
    {
        return true;
    }
    return false;    
}


int main()
{
    SoundContext soundContext;
    initSound(&soundContext);

    registerKeyboardHandler();
    videoInit(0x13);

    

    
    Rectangle leftPaddle = {TO_SUBPIXELS(10),TO_SUBPIXELS(10),TO_SUBPIXELS(5),TO_SUBPIXELS(40)};
    Rectangle rightPaddle = {TO_SUBPIXELS(300),TO_SUBPIXELS(10),TO_SUBPIXELS(5),TO_SUBPIXELS(40)};

    int leftColor = 3;
    int rightColor = 4;

    int leftPaddleColor = leftColor;
    int rightPaddleColor = rightColor;

    clearScreen();

    drawTitle();

    while (!s_keyEsc && !s_keySpace);

    do 
    {
        int leftScore = 0;
        int rightScore = 0;
        int leftScore_old = -1;
        int rightScore_old = -1;
        int16_t dx = TO_SUBPIXELS(1);
        int16_t dy = TO_SUBPIXELS(2);

        Rectangle ball = {TO_SUBPIXELS(PLAYFIELD_W) / 2, TO_SUBPIXELS(PLAYFIELD_H) / 2, TO_SUBPIXELS(3), TO_SUBPIXELS(3)};

        clearScreen();
        
        drawRect(0, 180, 320, 3, 8);

        do
        {
            processSound(&soundContext);
            waitForVSync();
            // first delete old gfx
            drawRectangle(&ball, 0);
            drawRectangle(&leftPaddle, 0);
            drawRectangle(&rightPaddle, 0);


            // handle inputs
            if (s_keyQ) leftPaddle.y -= TO_SUBPIXELS(2);
            if (s_keyA) leftPaddle.y += TO_SUBPIXELS(2);
            if (leftPaddle.y < 0) leftPaddle.y = 0;
            if (leftPaddle.y > TO_SUBPIXELS(PLAYFIELD_H) - leftPaddle.h) leftPaddle.y = TO_SUBPIXELS(PLAYFIELD_H) - leftPaddle.h;

            if (s_keyUp) rightPaddle.y -= TO_SUBPIXELS(2);
            if (s_keyDown) rightPaddle.y += TO_SUBPIXELS(2);
            if (rightPaddle.y < 0) rightPaddle.y = 0;
            if (rightPaddle.y > TO_SUBPIXELS(PLAYFIELD_H) - rightPaddle.h) rightPaddle.y = TO_SUBPIXELS(PLAYFIELD_H) - rightPaddle.h;


            // now calculate physics
            ball.x += dx;
            ball.y += dy;

            if (calculateCollision(&ball, &rightPaddle))
            {
                dx = -abs(dx) - 5;
                rightPaddleColor += 10;
                playNote(&soundContext, G4, 30);
            }
            if (ball.x + ball.w > TO_SUBPIXELS(PLAYFIELD_W))
            {
                dx = -abs(dx);
                ball.x = TO_SUBPIXELS(PLAYFIELD_W) - ball.w;
                ++leftScore;
                playNote(&soundContext, D4b, 30);
            }
            if (calculateCollision(&ball, &leftPaddle))
            {
                dx = abs(dx) + 5;
                leftPaddleColor += 10;
                playNote(&soundContext, G4b, 30);
            }
            if (ball.x < 0)
            {
                dx = abs(dx);
                ball.x = 0;
                ++rightScore;
                playNote(&soundContext, D4b, 30);
            }
            if (ball.y + ball.h > TO_SUBPIXELS(PLAYFIELD_H))
            {
                dy = -abs(dy);
                ball.y = TO_SUBPIXELS(PLAYFIELD_H) - ball.h;
            }
            if (ball.y < 0)
            {
                dy = abs(dy);
                ball.y = 0;
            }

            if (rightPaddleColor > rightColor) --rightPaddleColor;
            if (leftPaddleColor > leftColor) --leftPaddleColor;


            // now draw gfx
            drawRectangle(&ball, 0xf);
            drawRectangle(&leftPaddle, leftPaddleColor);
            drawRectangle(&rightPaddle, rightPaddleColor);

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
