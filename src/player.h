#ifndef PLAYER_H
#define PLAYER_H

#include "../AudioFile.h"
#include "audio_object.h"

class Player : public AudioObject
{
public:
    Player(int chan = 2) : AudioObject(chan)
    {
    }

    void load(AudioFile<double> *audio_file)
    {
        this->audio_file = audio_file;
        num_samples = audio_file->getNumSamplesPerChannel();
        num_channels = audio_file->getNumChannels();
    }

    void update() override
    {
        for (c = 0; c < chan; c++)
        {
            out[c] = audio_file->samples[c % num_channels][(int)floor(pos)];
        }
        pos += speed;
        pos = fmod(pos, num_samples);
        if (pos < 0)
        {
            pos += num_samples;
        }
    }

    void set_pos(double p)
    {
        pos = (int)floor((double)num_samples * p);
    }

    double speed = 1;
    double pos = 0;

    AudioFile<double> *audio_file;

private:
    int num_samples = 1;
    int num_channels = 2;
};

#endif
