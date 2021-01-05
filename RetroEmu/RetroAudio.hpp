#ifndef RETRO_AUDIO_HPP_INCLUDED
#define RETRO_AUDIO_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <queue>
#include <vector>
#include <stdio.h>
#include "RetroFraction.hpp"

class RetroAudio
{
    SDL_AudioDeviceID sdldev;
    const unsigned SAMPLING = 48000;
    const unsigned BUFFER_LENGTH = 2048;

    friend void audio_callback(void *, Uint8 *, int);

public:
    RetroAudio();
    ~RetroAudio();
    void loadSample(unsigned short sample);
    unsigned getQueuedCount();
    unsigned long long getOutputSamplesAndReset();
    void play();

private:

    std::queue<unsigned short> queuedSamples;
    std::vector<unsigned short> avgBuffer;
    Uint16* bufferCopy;
    FILE* fileOutput;
    bool semaphoreForBufferCopy;
    unsigned long long outputSamplesCount;
    unsigned samplesUntilNextSlice;
    RetroFraction slicesFraction;
    bool warmingUp;
    void sendSamplesToHW(Uint16 *stream, int length);
};


void audio_callback(void*, Uint8*, int);


#endif
