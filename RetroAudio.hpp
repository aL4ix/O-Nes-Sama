#ifndef RETRO_AUDIO_HPP_INCLUDED
#define RETRO_AUDIO_HPP_INCLUDED

#include <SDL2/SDL.h>
#include <queue>
#include <vector>


class RetroAccFrac
{
public:
    RetroAccFrac(unsigned total, unsigned dividedBy);
    unsigned getNextSlice();

private:
    unsigned slice;
    unsigned fracAccumulated;
    unsigned fracNumerator;
    unsigned fracDenominator;
};


const unsigned short AMPLITUDE = 30000;
const unsigned SAMPLING = 48000;
const unsigned BUFFER_LENGTH = 1024;


class RetroAudio
{
private:
    std::queue<unsigned short> beeps;
    std::vector<unsigned short> avgBuffer;
    Uint16* bufferCopy;
    FILE* fileOutput;
    bool semaphoreForBufferCopy;
    unsigned long long outputSamplesCount;
    unsigned samplesToNextSlice;
public:
    RetroAudio();
    ~RetroAudio();
    void load(unsigned short sample);
    void loadSamples(Uint16 *stream, int length);
    void wait();
    unsigned getSize();
    unsigned long long getSamplesCountAndReset();

    unsigned long long count;
    RetroAccFrac raf;
};


void audio_callback(void*, Uint8*, int);


#endif
