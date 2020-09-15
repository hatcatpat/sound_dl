#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "audio_object.h"

class Osc : public AudioObject
{
public:
    Osc(float freq = 440, int chan = 2) : AudioObject(chan)
    {
        this->freq = freq;
    }

    void update() override
    {

        o = wave_ptr(phase);

        for (c = 0; c < chan; c++)
        {
            out[c] = o;
        }
        phase += (double)(*freq_ptr) * 2.0 * M_PI / SAMPLE_RATE;
    }

    double (*wave_ptr)(double) = &sin;

    double phase = 0;

    double freq = 440.0;
    double *freq_ptr = &freq;
};

#endif
