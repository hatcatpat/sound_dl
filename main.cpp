#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h>
#include "rtaudio/RtAudio.h"
#include <algorithm>

#include <map>
#include "AudioFile.h"

std::map<std::string, AudioFile<double>> audio_file_dict;
auto *audio_file_dict_ptr = &audio_file_dict;
void loadFiles()
{
  AudioFile<double> file;
  file.load("samples/break.wav");
  audio_file_dict.insert(std::pair<std::string, AudioFile<double>>("break", file));
}

void defaultAudioOut(const double *in, double *out, unsigned int frames){
    /* example
  unsigned int i;

  for (i = 0; i < frames*2; i+=2)
  {
    out[i] = 0.0; // left
    out[i+1] = 0.0; // right
    t++;
  }
  */
};

auto *audioOut = &defaultAudioOut;
bool running_audioOut = false;
double *out, *in;
int c = 0;
int callback(void *outputBuffer, void *inputBuffer, unsigned int frames,
             double streamTime, RtAudioStreamStatus status, void *userData)
{

  (void)streamTime;
  (void)status;
  (void)userData;

  out = (double *)outputBuffer;
  in = (double *)inputBuffer;
  unsigned int i;

  for (i = 0; i < frames * 2; i += 2)
  {
    out[i] = 0;
    out[i + 1] = 0;
  }

  running_audioOut = true;
  audioOut(in, out, frames);
  running_audioOut = false;

  for (i = 0; i < frames * 2; i += 2)
  {
    for (c = 0; c < 2; c++)
    {
      if (out[i + c] > 1)
      {
        out[i + c] = 1;
      }
      else if (out[i + c] < -1)
      {
        out[i + c] = -1;
      }
    }
  }

  return 0;
}

void *libaudio;
auto *new_audioOut = &defaultAudioOut;
void (*new_audioOut_setup)(std::map<std::string, AudioFile<double>> *);
void load()
{
  // char file_char[file.size() + 1];
  // libaudio = dlopen(strcpy(file_char, file.c_str()), RTLD_NOW | RTLD_LOCAL);
  libaudio = dlopen("./libaudio.so", RTLD_NOW | RTLD_LOCAL);
  if (!libaudio)
  {
    printf("dlopen failed: %s\n", dlerror());
  }
  else
  {

    new_audioOut_setup = (void (*)(std::map<std::string, AudioFile<double>> *))dlsym(libaudio, "setup");
    if (new_audioOut_setup)
    {
      new_audioOut_setup(audio_file_dict_ptr);
    }

    new_audioOut =
        (void (*)(const double *, double *, unsigned int))dlsym(libaudio, "audioOut");
    if (new_audioOut)
    {
      audioOut = new_audioOut;
    }

    else
    {
      printf("dlsym for add failed: %s\n", dlerror());
    }
  }
}

void reload()
{
  while (true)
  {
    if (!running_audioOut)
    {
      audioOut = &defaultAudioOut;
      srand(time(NULL));

      dlclose(libaudio);
      load();
      std::cout << "reloaded\n";
      break;
    }
  }
}

int main()
{

  // misc

  srand(time(NULL));
  loadFiles(); // load .wav files stored in the samples folder

  // setup dynamic library

  load();

  // setup RtAudio and run

  RtAudio dac;
  if (dac.getDeviceCount() < 1)
  {
    std::cout << "\nNo audio devices found!\n";
    exit(0);
  }
  RtAudio::StreamParameters parameters;
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = 2;
  parameters.firstChannel = 0;
  unsigned int sampleRate = 44100;
  unsigned int bufferFrames = 256; // 256 sample frames

  try
  {
    dac.openStream(&parameters, NULL, RTAUDIO_FLOAT64,
                   sampleRate, &bufferFrames, &callback, NULL);
    dac.startStream();
  }
  catch (RtAudioError &e)
  {
    e.printMessage();
    exit(0);
  }

  std::cout << "\nPlaying ... type 'quit' to quit.\n";
  char line[100];

  bool loop = true;
  while (loop)
  {
    std::cout << ">> ";
    fgets(line, sizeof(line), stdin);
    // std::string s(line);

    if (strstr(line, "reload") != NULL)
    {
      // s.erase(0, 7);
      // s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
      // s = "./" + s + ".so";
      // std::cout << "reloaded\n";
      // reload(s);
      reload();
    }
    else if (strstr(line, "quit") != NULL)
    {
      std::cout << "quit!\n";
      loop = false;
    }
  }

  try
  {
    // Stop the stream
    dac.stopStream();
  }
  catch (RtAudioError &e)
  {
    e.printMessage();
  }
  if (dac.isStreamOpen())
    dac.closeStream();

  return 0;
}