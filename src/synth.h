#ifndef SYNTH_H
#define SYNTH_H

#include "audio_object.h"
#include "oscillator.h"
#include "envelope.h"

class Synth : public AudioObject
{
public:
    Synth(float freq = 440, float atk = 0.01, float rel = 0.1, int chan = 2)
        : AudioObject(chan), osc(freq,chan), env(atk,rel)
    {
    }

    void update(){
        for (c = 0; c < chan; c++)
        {
            out[c] = osc.out[c] * env.out[0] * ( 1.0 - (*pan_ptr) + c*2.0*(*pan_ptr) ) / 2.0 * vol;
        }

        osc.update();
        env.update();
    }

    float pan = 0.0;
    float *pan_ptr = &pan;

    float vol = 1;
    float *vol_ptr = &vol;

    Osc osc;
    Env env;
};

#endif