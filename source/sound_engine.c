#include "sound_engine.h"

bool initSound(SoundContext* context)
{
    if (!context) return false;

    if (!AdlibExists())
    {
        context->soundInitialized = false;
        return false;
    }
   
    context->soundInitialized = true;
    context->soundRestTime = -1;

    FMReset();

    FMInstrument instrument = {
        0x05, 0x01, 0x4e, 0x00, 0xda, 0xf9, 0x25, 0x15, 0x00, 0x00, 0x0a
    };

    instrument.AttackDecay[0] = 0;

    // LoadSBI("MARIMBA.SBI", &instrument);
    FMSetVoice(0, &instrument);
    // FMReset();

    return true;
}

void deInitSound(SoundContext* context)
{
    if (context && context->soundInitialized)
    {
        FMReset();
    }
}

void playNote(SoundContext* context, enum SCALE note, int length)
{
    if (context && context->soundInitialized)
    {
        FMKeyOn(0, note, 4);
        context->soundRestTime = length;
    }
}

void processSound(SoundContext* context)
{
    if (context && context->soundInitialized)
    {
        if (context->soundRestTime > 0)
        {
            --context->soundRestTime;
            if (context->soundRestTime == 0)
            {
                FMKeyOff(0);
            }
        }
    }
}


