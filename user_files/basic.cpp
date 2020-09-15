#include "../src/funcs.h"

//
//
//

extern "C" void
setup(std::map<std::string, AudioFile<double>> *dict)
{
    bpm = 60;
}

unsigned long T = 0;
extern "C" void audioOut(const double *in, double *out, unsigned int frames)
{

    for (i = 0; i < frames * 2; i += 2)
    {
        for (c = 0; c < 2; c++)
        {
            if (out[i + c] > 1)
            {
                out[i + c] = 1;
            }
            if (out[i + c] < -1)
            {
                out[i + c] = -1;
            }
        }

        T++;
    }
}
