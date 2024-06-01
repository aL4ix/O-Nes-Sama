#ifndef RETRO_AUDIO_HPP_INCLUDED
#define RETRO_AUDIO_HPP_INCLUDED

#include "../Logging/Logger.h"
#include "RetroFraction.hpp"
#include <SDL2/SDL.h>
#include <queue>
#include <stdio.h>
#include <vector>

class RetroAudio {
    SDL_AudioDeviceID sdldev;
    const unsigned SAMPLING = 48000;
    const unsigned BUFFER_LENGTH = 2048;

    friend void audio_callback(void*, Uint8*, int);

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
    void sendSamplesToHW(Uint16* stream, int length);
};

void audio_callback(void* retroAudio, Uint8* streamInBytes, int lengthInBytes);

#endif
