#define SDL_MAIN_HANDLED

#include <iostream>

#include "../CPU.h"
#include "../Cartridge/8Tunes/NSFLoader.h"
#include "../RetroEmu/RetroAudio.hpp"
#include "DummyPPU.h"

using namespace std;

void pushAudioSample(short left, short right);
RetroAudio retroAudio;

int main(int argc, char** argv)
{
    Log.enableDebugCategory(LogCategory::loaderNSF);

    NSFLoader loader("../games/music.nsf");
    if (!loader.mapper)
        return -1;

    CPU cpu(*loader.mapper);
    PPU ppu;
    cpu.setPPUPtr(&ppu);
    cpu.apu->setPushAudioSampleCallback(pushAudioSample);
    loader.initializingATune(cpu, 2); // song number

    //
    // Play()
    //

    // Master clock of NES machine.
    unsigned NTSCmasterclock = 21477272;
    // CPU Frequency in Hz
    unsigned cpufreq = NTSCmasterclock / 12;
    // Synchronization freq
    unsigned syncfreq = 60;

    RetroFraction rafForCPUCycles(cpufreq, syncfreq);
    RetroFraction rafForTime(1000, syncfreq);

    int pendCycles = 0;
    unsigned lastTimeTick = SDL_GetTicks();
    unsigned emuStartTime = lastTimeTick;
    unsigned secondsCount = 0;
    unsigned FPS = 0;
    unsigned sumCycles = 0;
    unsigned cpuCurGenCycCount = 0;

    for (int i = 0; i < 10000; i++) {
        const unsigned cycles = rafForCPUCycles.getNextSlice();
        const unsigned requestedCycles = cycles + pendCycles;
        sumCycles += requestedCycles;

        pendCycles = cpu.run(requestedCycles);
        Log.debug(LogCategory::onessama8Tunes, "slice:%u pend:%d", cycles, pendCycles);
        loader.nfsDidARts(cpu);

        unsigned now = SDL_GetTicks();
        unsigned timeSpent = now - lastTimeTick;
        unsigned thisFrameTimeInMilis = rafForTime.getNextSlice();
        int sleeptime = thisFrameTimeInMilis - timeSpent;
        FPS++;

        if (sleeptime > 0) {
            SDL_Delay(sleeptime);
        } else {
            Log.debug(LogCategory::onessama8Tunes, "NSF underrun!");
        }
        lastTimeTick = now + sleeptime;

        // continue;

        const unsigned secondsSinceStart = (now - emuStartTime) / 1000;
        if (secondsCount != secondsSinceStart) {
            secondsCount = secondsSinceStart;
            Log.debug(LogCategory::onessama8Tunes, "T:%u F:%u S:%llu CycMain:%u CycSpentCPU:%u A:%u B:%u", now, FPS,
                retroAudio.getOutputSamplesAndReset(), sumCycles,
                cpu.instData.generalCycleCount - cpuCurGenCycCount,
                cpu.apu->callCyclesCount, retroAudio.getQueuedCount());
            sumCycles = 0;
            FPS = 0;
            cpu.apu->callCyclesCount = 0;
            cpuCurGenCycCount = cpu.instData.generalCycleCount;
        }
    }

    return 0;
}

void pushAudioSample(short left, short right)
{
    retroAudio.loadSample(left);
}
