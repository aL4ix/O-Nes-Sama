#ifndef RETRO_AUDIO_HPP_INCLUDED
#define RETRO_AUDIO_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <queue>
#include <vector>


class RetroFraction
{
public:
    RetroFraction(unsigned total, unsigned dividedBy);
    unsigned getNextSlice();

private:
    unsigned slice;
    unsigned fracAccumulated;
    unsigned fracNumerator;
    unsigned fracDenominator;
};


class RetroAudio
{
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
    RetroFraction raf;
    bool warmingUp;



    void sendSamplesToHW(Uint16 *stream, int length);
};


void audio_callback(void*, Uint8*, int);


#endif
