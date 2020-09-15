#ifndef FUNCS_H
#define FUNCS_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <cstring>

#include "../AudioFile.h"

#include "../src/oscillator.h"
#include "../src/envelope.h"
#include "../src/synth.h"
#include "../src/effects.h"
#include "../src/player.h"

#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100.0)
#endif

#ifndef BLOCK_SIZE
#define BLOCK_SIZE (256)
#endif

unsigned int i;
int c;
double bpm = 60;

static double saw(double phase)
{
    return -1.0 + 2.0 * ((double)fmod(phase, 2.0 * M_PI) / (2.0 * M_PI));
}

static double pulse(double phase)
{
    return fmod(phase, 2 * M_PI) < M_PI ? 0.0 : 1.0;
}

static double map(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static int nearestint(int x = 0, int d = 10)
{
    return (int)floor((double)x / d) * d;
}

static double nearestdouble(double x = 1, double d = 0.2)
{
    return (double)(x - fmod(x, d));
}

static int randint(int lo = 0, int hi = 100, int nearest = -1)
{
    if (nearest == -1)
    {
        return (int)floor(map((double)rand() / (RAND_MAX - 1), 0, 1, lo, hi));
    }
    else
    {
        return nearestint((int)floor(map((double)rand() / (RAND_MAX - 1), 0, 1, lo, hi)), nearest);
    }
}

static double randdouble(double lo = 0, double hi = 1, double nearest = -1)
{
    if (nearest == -1)
    {
        return map((double)rand() / (RAND_MAX - 1), 0, 1, lo, hi);
    }
    else
    {
        return nearestdouble(map((double)rand() / (RAND_MAX - 1), 0, 1, lo, hi), nearest);
    }
}

static double clamp(double in, double lo = -1, double hi = 1)
{
    if (in < lo)
    {
        in = lo;
    }
    else if (in > hi)
    {
        in = hi;
    }
    return in;
}

static double midiToFreq(double note)
{
    return (double)(440 * pow(2, ((note - 69) / 12)));
}

std::map<char, int> note_map = {
    {'c', 0}, {'C', 1}, {'d', 2}, {'D', 3}, {'e', 4}, {'f', 5}, {'F', 6}, {'g', 7}, {'G', 8}, {'a', 9}, {'A', 10}, {'b', 11}};

static double note(std::string note)
{
    char n = note.c_str()[0];
    note.erase(note.begin());
    return midiToFreq( (stoi(note)+1)*12 + note_map[n] );
}

bool every(unsigned long t, double s)
{

    if (
        t % (int)fmax(1.0,
                      floor(s * SAMPLE_RATE * 60 / (bpm > 0 ? bpm : 300))) ==
        0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool leq(unsigned long t, double dur, double p)
{
    int d = (int)fmax(1.0, floor(dur * SAMPLE_RATE));
    if (
        t % d <
        (int)floor(p * d))
    {
        return true;
    }
    else
    {
        return false;
    }
}

#endif