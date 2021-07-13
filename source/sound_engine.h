#ifndef _SOUND_ENGINE_INCLUDED
#define _SOUND_ENGINE_INCLUDED

#include <stdbool.h>
#include "adlib.h"

typedef struct _SoundContext
{
    bool soundInitialized; // if set to false, then sound hardware was not found.
    int soundRestTime;     // contains the time units that the sound will be played until it is turned off.
} SoundContext;

/**
 * Initializes the sound system. If successfull then true is returned.
 * In error case, e.g. if no sound hardware was found, false is returned.
 */
bool initSound(SoundContext* context);

void deInitSound(SoundContext* context);

/**
 * 
 */
void playNote(SoundContext* context, enum SCALE note, int length);

/**
 * This function needs to be called about 70 times per second.
 */
void processSound(SoundContext* context);


#endif