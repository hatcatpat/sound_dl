#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "audio_object.h"

class Env : public AudioObject
{
public:
    Env(float atk = 0.01, float rel = 0.1) : AudioObject(1)
    {
        this->atk = atk;
        this->rel = rel;
    }

    void update() override
    {
        if (active)
        {
            atk_samp = (int)floor((*atk_ptr) * SAMPLE_RATE);
            rel_samp = (int)floor((*rel_ptr) * SAMPLE_RATE);

            if (t < atk_samp)
            {
                out[0] = (float)t / atk_samp;
            }
            else if (t < atk_samp + rel_samp)
            {
                out[0] = 1 - (float)(t - atk_samp) / rel_samp;
            }
            else
            {
                active = false;
            }

            t++;
        }
    }

    void trigger()
    {
        t = 0;
        active = true;
    }

    float atk = 0.01;
    int atk_samp = (int)floor(atk * SAMPLE_RATE);
    float *atk_ptr = &atk;

    float rel = 0.01;
    int rel_samp = (int)floor(rel * SAMPLE_RATE);
    float *rel_ptr = &rel;

private:
    int t = 0;
    bool active = false;
};

#endif