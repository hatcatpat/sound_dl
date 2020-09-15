#include "../src/funcs.h"

//
//
//

unsigned long T = 0;

//

typedef double (*double_func)(double);
double_func funcs[] = {sin, saw, pulse};

Synth sy(440, 0.01, 0.2);

Synth sy2(440, 0.01, 0.25);
Osc posc(0.5, 1);
int count2 = 0;
double oct = 1;

Player player;

std::string notes[] = {"c4", "a4", "c5", "e4"};

int count = 0;
double dur = 0.125;

Delay delay(0.5, 0.5);
Osc delay_osc(0.1, 1);
Limiter limiter;

int st = 0;
double swdur = 0.125;
double swdur2 = 4;

extern "C" void
setup(std::map<std::string, AudioFile<double>> *dict)
{
    player.load(&dict->at("break"));
    bpm = 175;
    player.speed = (double)bpm / 92;

    sy.osc.wave_ptr = &saw;
    sy2.osc.wave_ptr = &pulse;
}

void events()
{
    if (st == 0)
    {
        if (every(T, dur))
        {
            sy.osc.freq = note(notes[count]);
            if (randint() < 50)
            {
                if (randint() < 50)
                {
                    sy.osc.freq *= 2;
                }
                else
                {
                    sy.osc.freq /= 2;
                }
            }
            if (randint() < 25)
            {
                sy.osc.wave_ptr = funcs[randint(0, 3)];
            }
            count++;
            count %= 4;
            dur = randdouble(0.25, 1, 0.25);
            sy.env.rel = dur / 4.0;
            sy.env.trigger();
        }

        if (every(T, swdur2) && randint() < 50)
        {
            swdur = randdouble(0.125, 1.0, 0.125);
            swdur2 = randint(2,8,2);
            st = 1;
            player.speed /= 2;
        }
    }
    else
    {
        if (every(T, swdur))
        {
            sy.pan = randdouble(-1, 1, 0.1);
            sy.env.trigger();
            player.pos = 0;
        }

        if (every(T, swdur * swdur2))
        {
            if (randint() < 50)
            {
                player.speed *= 2;
                sy.pan = 0;
                st = 0;
            }
        }
    }

    //
    //
    //

    if (every(T, 1) && randint() < 10)
    {
        sy.osc.freq = note("e6");
        sy.env.rel = 0.1;
        sy.pan = 0.5;
        sy.env.trigger();
    }

    if (every(T, 1) || (every(T,0.5) && randint() < 25 ) )
    {
        sy2.osc.freq = note(notes[count2]) * oct;
        if (count2 == 0)
        {
            switch (randint(0, 2))
            {
            case 0:
                oct = 0.5;
                break;
            case 1:
                oct = 1;
                break;
            case 2:
                oct = 2;
                break;
            }
        }
        count2++;
        count2 %= 4;
        sy2.env.trigger();
    }

    if (every(T, 4))
    {
        delay.delaytime = randdouble(0.125, 1, 0.125);
    }

    delay.left_pan = delay_osc.out[0];
    delay.right_pan = -delay_osc.out[0];

    sy2.pan = posc.out[0];
}

extern "C" void audioOut(const double *in, double *out, unsigned int frames)
{

    for (i = 0; i < frames * 2; i += 2)
    {

        for (c = 0; c < 2; c++)
        {
            out[i + c] += sy.out[c];
            if (st == 0)
                out[i + c] += sy2.out[c];
        }

        events();

        player.update();
        sy.update();
        sy2.update();
        posc.update();
        delay_osc.update();
        delay.update(out, i);

        for (c = 0; c < 2; c++)
        {
            if (st == 1)
                out[i + c] += sy2.out[c];
            out[i + c] += player.out[c] * 2;
            out[i + c] += delay.out[c];
        }

        T++;
    }

    // post processing

    limiter.update(out, frames);

    for (i = 0; i < frames * 2; i += 2)
    {
        for (c = 0; c < 2; c++)
        {
            out[i + c] *= limiter.out[c];
        }
    }
}
