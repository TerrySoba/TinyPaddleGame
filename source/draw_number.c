#include "draw_number.h"

#include "vga.h"

void drawNumber(uint16_t x, int16_t y, uint8_t color, uint8_t scale, int number)
{
    /// Seven segment style number drawing routine
    /// The numbers of the segments are shown in the
    /// diagram below.
    //
    //               5
    //            ######
    //          #        #
    //        0 #        # 4
    //          #    6   #
    //            ######
    //          #        #
    //        1 #        # 3
    //          #        #
    //            ######
    //               2  

    uint16_t elementLength = 4 * scale;
    uint16_t elementThickness = 1 * scale;
    uint16_t gapSize = 1 * scale;


    // segment 0
    #define SEG_0 drawRect(x, y + gapSize, elementThickness, elementLength, color);

    // segment 1
    #define SEG_1 drawRect(x, y + gapSize + elementLength + gapSize, elementThickness, elementLength, color);

    // segment 2
    #define SEG_2 drawRect(x + gapSize, y + gapSize + elementLength + gapSize + elementLength, elementLength, elementThickness, color);

    // segment 3
    #define SEG_3 drawRect(x + gapSize + elementLength, y + gapSize + elementLength + gapSize, elementThickness, elementLength, color);

    // segment 4
    #define SEG_4 drawRect(x + gapSize + elementLength, y + gapSize, elementThickness, elementLength, color);

    // segment 5
    #define SEG_5 drawRect(x + gapSize, y, elementLength, elementThickness, color);

    // segment 6
    #define SEG_6 drawRect(x + gapSize, y + gapSize + elementLength, elementLength, elementThickness, color);

    switch (number)
    {
        case 0:
            SEG_0
            SEG_1
            SEG_2
            SEG_3
            SEG_4
            SEG_5
            break;
        case 1:
            SEG_3
            SEG_4
            break;
        case 2:
            SEG_1
            SEG_2
            SEG_4
            SEG_5
            SEG_6
            break;
        case 3:
            SEG_2
            SEG_3
            SEG_4
            SEG_5
            SEG_6
            break;
        case 4:
            SEG_0
            SEG_3
            SEG_4
            SEG_6
            break;
        case 5:
            SEG_0
            SEG_2
            SEG_3
            SEG_5
            SEG_6
            break;
        case 6:
            SEG_0
            SEG_1
            SEG_2
            SEG_3
            SEG_5
            SEG_6
            break;
        case 7:
            SEG_3
            SEG_4
            SEG_5
            break;
        case 8:
            SEG_0
            SEG_1
            SEG_2
            SEG_3
            SEG_4
            SEG_5
            SEG_6
            break;
        case 9:
            SEG_0
            SEG_3
            SEG_4
            SEG_5
            SEG_6
            break;
    }

}