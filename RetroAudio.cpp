#include "RetroAudio.hpp"


RetroAccFrac::RetroAccFrac(unsigned total, unsigned dividedBy)
{
    slice = total/dividedBy; // Get the size of a normal slice
    fracAccumulated = 0; // Start the count at 0 so the remaining part will accumulate
    fracNumerator = total % dividedBy; // This is the remaining part, this will accumulate in fracAccumulated
    fracDenominator = dividedBy; //If you divide by 60FPS you will keep the fraction as 60 so you can know when you got a whole frame
}

unsigned RetroAccFrac::getNextSlice()
{
    unsigned wholePartAccumulated = 0;
    fracAccumulated += fracNumerator;
    if(fracAccumulated >= fracDenominator) // We got a whole part!
    {
        fracAccumulated -= fracDenominator;
        wholePartAccumulated = 1;
    }
    return slice+wholePartAccumulated;
}


RetroAudio::RetroAudio() : outputSamplesCount(0), raf(21477272/12, SAMPLING)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		printf("SDL Audio could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
	}

    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = SAMPLING;
    desiredSpec.format = AUDIO_U16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = BUFFER_LENGTH;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;

    if(SDL_OpenAudio(&desiredSpec, &obtainedSpec) != 0)
    {
        printf("Failed to open audio: %s\n", SDL_GetError());
    }
    /*
    // For some reason this is equivalent to the one above but this doesn't work
    if(SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, SDL_AUDIO_ALLOW_ANY_CHANGE) == 0)
    {
        printf("Failed to open audio: %s\n", SDL_GetError());
    }
    */

    samplesToNextSlice = raf.getNextSlice();

    /* Debug */
    bufferCopy = new Uint16[BUFFER_LENGTH];
    fileOutput = fopen("APUout.debug", "wb");
    semaphoreForBufferCopy = false;
    printf("Started Audio. Slice:%u\n", samplesToNextSlice);
    /* End Debug */

    // start play audio
    SDL_PauseAudio(0);
}

RetroAudio::~RetroAudio()
{
    SDL_CloseAudio();
    delete [] bufferCopy;
    fclose(fileOutput);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void RetroAudio::loadSamples(Uint16 *stream, int length)
{
    int i = 0;
    while (i < length) {
        if (beeps.empty()) {
            printf("Sound Underrun! filling %d\n", length - i);
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }
        else
        {
            unsigned short c = beeps.front();
            beeps.pop();
            stream[i] = c;
            i++;
        }
    }
    if(beeps.size() >= BUFFER_LENGTH*2)
    {
        printf("Dropping sound buffer, cur size: %lu\n", beeps.size());
        //getchar();
        for(unsigned a=0; a<1024; a++)
            beeps.pop();
        //std::queue<unsigned char> empty;
        //std::swap( beeps, empty );
    }
    /* Debug */
  #ifdef RETRO_AUDIO_DEBUG
        memcpy(bufferCopy, stream, sizeof(Uint16)*BUFFER_LENGTH);
        semaphoreForBufferCopy = true;
    #endif // RETRO_AUDIO_DEBUG
    /* End Debug */
}

void RetroAudio::load(unsigned short sample)
{
    avgBuffer.push_back(sample);
    if(--samplesToNextSlice == 0)
    {
        outputSamplesCount += 1;
        samplesToNextSlice = raf.getNextSlice();

        unsigned avg = 0;
        const unsigned size = avgBuffer.size();
        for(unsigned i=0; i<size; i++)
            avg += avgBuffer[i];
        avgBuffer.clear();
        avg /= size;
        SDL_LockAudio();
        beeps.push(avg);
        SDL_UnlockAudio();
        if(semaphoreForBufferCopy)
        {
            fwrite(bufferCopy, sizeof(Uint16), BUFFER_LENGTH, fileOutput);
            semaphoreForBufferCopy = false;
        }
    }
}

void RetroAudio::wait()
{
    int size;
    do {
        SDL_Delay(20);
        SDL_LockAudio();
        size = getSize();
        SDL_UnlockAudio();
    } while (size > 0);
}

unsigned RetroAudio::getSize()
{
    return beeps.size();
}

unsigned long long RetroAudio::getSamplesCountAndReset()
{
    unsigned long long ret = outputSamplesCount;
    outputSamplesCount = 0;
    return ret;
}

void audio_callback(void *_beeper, Uint8 *_streamInBytes, int _lengthInBytes)
{
    Uint16 *stream = (Uint16*) _streamInBytes;
    int length = _lengthInBytes / 2;
    RetroAudio* beeper = (RetroAudio*) _beeper;

    beeper->loadSamples(stream, length);
}
