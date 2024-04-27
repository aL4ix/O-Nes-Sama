#include "RetroAudio.hpp"

RetroAudio::RetroAudio()
    : outputSamplesCount(0)
    , slicesFraction(21477272 / 12, SAMPLING)
    , warmingUp(true)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        Log.error("SDL Audio could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = SAMPLING;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = BUFFER_LENGTH;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;

    /*if(SDL_OpenAudio(&desiredSpec, &obtainedSpec) != 0)
    {
        Log.error("Failed to open audio: %s\n", SDL_GetError());
    }*/

    // For some reason this is equivalent to the one above but this doesn't work
    sdldev = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);
    if (sdldev == 0) {
        Log.error("Failed to open audio: %s\n", SDL_GetError());
    }

    Log.debug(LogCategory::otherRetroAudio, "Describe audio");
    Log.debug(LogCategory::otherRetroAudio, "freq: %d %d ", desiredSpec.freq, obtainedSpec.freq);
    Log.debug(LogCategory::otherRetroAudio, "format: %x %x ", desiredSpec.format, obtainedSpec.format);
    Log.debug(LogCategory::otherRetroAudio, "isunsigned: %d %d ", SDL_AUDIO_ISUNSIGNED(desiredSpec.format), SDL_AUDIO_ISUNSIGNED(obtainedSpec.format));
    Log.debug(LogCategory::otherRetroAudio, "bitsize: %d %d ", SDL_AUDIO_MASK_BITSIZE & desiredSpec.format, SDL_AUDIO_MASK_BITSIZE & obtainedSpec.format);

    // Fill buffer so it gives warm up time
    for (unsigned i = 0; i < BUFFER_LENGTH; i++) {
        queuedSamples.push(0);
    }

    samplesUntilNextSlice = slicesFraction.getNextSlice();

    semaphoreForBufferCopy = false;
#ifdef RETRO_AUDIO_DEBUG
    bufferCopy = new Uint16[BUFFER_LENGTH];
    fileOutput = fopen("APUout.debug", "wb");
    Log.debug(LogCategory::otherRetroAudio, "Started Audio. Slice:%u", samplesUntilNextSlice);
#endif // RETRO_AUDIO_DEBUG
}

RetroAudio::~RetroAudio()
{
    // SDL_CloseAudio();
#ifdef RETRO_AUDIO_DEBUG
    delete[] bufferCopy;
    fclose(fileOutput);
#endif // RETRO_AUDIO_DEBUG
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void RetroAudio::sendSamplesToHW(Uint16* stream, int length)
{
    int i = 0;
    while (i < length) {
        if (queuedSamples.empty()) {
            Log.error("RetroAudio: Sound Underrun! filling %d", length - i);
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        } else {
            unsigned short c = queuedSamples.front();
            queuedSamples.pop();
            stream[i] = c;
            i++;
        }
    }
    if (queuedSamples.size() >= BUFFER_LENGTH * 2) {
        Log.error("Dropping sound buffer, cur size: %lu", queuedSamples.size());
        for (unsigned a = 0; a < 1024; a++)
            queuedSamples.pop();
    }
#ifdef RETRO_AUDIO_DEBUG
    memcpy(bufferCopy, stream, sizeof(Uint16) * BUFFER_LENGTH);
    semaphoreForBufferCopy = true;
#endif // RETRO_AUDIO_DEBUG
}

void RetroAudio::loadSample(unsigned short sample)
{
    avgBuffer.push_back(sample);
    if (--samplesUntilNextSlice == 0) {
        outputSamplesCount += 1;
        samplesUntilNextSlice = slicesFraction.getNextSlice();

        unsigned avg = 0;
        const unsigned size = avgBuffer.size();
        for (unsigned i = 0; i < size; i++)
            avg += avgBuffer[i];
        avgBuffer.clear();
        avg /= size;
        SDL_LockAudioDevice(sdldev);
        queuedSamples.push(avg);
        SDL_UnlockAudioDevice(sdldev);
#ifdef RETRO_AUDIO_DEBUG
        if (semaphoreForBufferCopy) {
            fwrite(bufferCopy, sizeof(Uint16), BUFFER_LENGTH, fileOutput);
            semaphoreForBufferCopy = false;
        }
#endif // RETRO_AUDIO_DEBUG
        if (warmingUp) {
            play();
            warmingUp = false;
        }
    }
}

unsigned RetroAudio::getQueuedCount()
{
    return queuedSamples.size();
}

unsigned long long RetroAudio::getOutputSamplesAndReset()
{
    unsigned long long ret = outputSamplesCount;
    outputSamplesCount = 0;
    return ret;
}

void RetroAudio::play()
{
    // start play audio
    SDL_PauseAudioDevice(sdldev, 0);
}

void audio_callback(void* _beeper, Uint8* _streamInBytes, int _lengthInBytes)
{
    Uint16* stream = (Uint16*)_streamInBytes;
    int length = _lengthInBytes / 2;
    RetroAudio* beeper = (RetroAudio*)_beeper;

    beeper->sendSamplesToHW(stream, length);
}
