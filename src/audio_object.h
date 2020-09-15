#ifndef AUDIO_OBJECT_H
#define AUDIO_OBJECT_H

#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100.0)
#endif

#include <math.h>

class AudioObject
{

public:
    AudioObject(){
        AudioObject(2);
    }
    AudioObject(int chan = 2)
    {
        this->chan = chan;

        out = new double[chan];
        for (c = 0; c < chan; c++)
        {
            out[c] = 0.0;
        }
    }
    ~AudioObject()
    {
        delete out;
    }

    virtual void update() {}
    virtual void update(double *, int) {}

    double *out;
    int chan = 2;

    double o = 0;
    int c = 0;
};

#endif
