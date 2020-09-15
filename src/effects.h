#ifndef EFFECTS_H
#define EFFECTS_H

#include <vector>
#include "audio_object.h"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE (256)
#endif

class Delay : public AudioObject
{
public:
    Delay(double delaytime = 0.1, double feedback = 0.5, float max_delaytime = 4)
        : AudioObject(2)
    {
        this->delaytime = delaytime;
        this->feedback = feedback;
        sz = (int)floor(max_delaytime * SAMPLE_RATE);
        data.assign(sz * 2, 0); // interlaced, L R L R ...

        pos = dpos = 0;
    }

    void update(double *in, int i = 0) override
    {

        delaytime_samp = (int)fmax(floor((*delaytime_ptr) * SAMPLE_RATE), 1);

        dpos = (pos - delaytime_samp);
        dpos = ((sz * chan) + (dpos % (sz * chan))) % (sz * chan);

        for (c = 0; c < chan; c++)
        {
            out[c] = data[dpos + c] * (1 - left_pan + 2 * c * left_pan) / 2 +
                     data[dpos + c] * (1 - right_pan + 2 * c * right_pan) / 2;

            data[pos + c] = in[i + c] * (*feedback_ptr);
        }

        pos += chan;
        pos %= sz * chan;
    }

    void clear()
    {
        for (int i = 0; i < sz * 2; i++)
        {
            data[i] = 0;
        }
    }

    double delaytime = 0.1;
    double *delaytime_ptr = &delaytime;

    double feedback = 0.5;
    double *feedback_ptr = &feedback;

    float left_pan = -1;
    float right_pan = 1;

private:
    int sz, pos, dpos;
    std::vector<double> data;

    int delaytime_samp = 0;
};

//
//
//

class Limiter : public AudioObject
{
public:
    Limiter() : AudioObject(2)
    {
        for (c = 0; c < 2; c++)
        {
            out[c] = 1.0;
        }

        sz = (int)floor(0.1 * SAMPLE_RATE * 2);
        data.assign(sz, 0);
    }

    void update(double *in, int i) override
    {

        for (c = 0; c < 2; c++)
        {
            data[pos + c] = in[i + c];
        }
        pos += 2;
        pos %= (int)floor((float)sz / 2);

        for (i = 0; i < sz * 2; i += 2)
        {
            for (c = 0; c < 2; c++)
            {
                rms[c] += pow(in[i + c], 2);
            }
        }

        for (c = 0; c < 2; c++)
        {
            rms[c] /= sz;
            rms[c] = sqrt(rms[c]);
            rms[c] = 10 * log(rms[c]);
            if (rms[c] > threshhold && rms[c] != 0)
            {
                out[c] = (double)rms[c] / threshhold;
            }
        }

        for (i = 0; i < sz * 2; i += 2)
        {
            for (c = 0; c < 2; c++)
            {
                rms[c] = 0;
            }
        }
    }

private:
    // int i = 0;
    double threshhold = -6;
    double rms[2] = {0, 0};
    int pos = 0;
    int sz = 1;
    std::vector<double> data;
};

#endif