#define SDL_MAIN_HANDLED

#include <iostream>

#include "../CPU.h"
#include "../Cartridge/8Tunes/NSFLoader.h"
#include "../RetroEmu/RetroAudio.hpp"
#include "DummyPPU.h"

using namespace std;

void pushAudioSample(short left, short right);

int main(int argc, char** argv)
{
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
        // printf("slice:%u pend:%d\n", cycles, pendCycles);
        loader.nfsDidARts(cpu);

        unsigned now = SDL_GetTicks();
        unsigned timeSpent = now - lastTimeTick;
        unsigned thisFrameTimeInMilis = rafForTime.getNextSlice();
        int sleeptime = thisFrameTimeInMilis - timeSpent;
        FPS++;

        if (sleeptime > 0) {
            SDL_Delay(sleeptime);
        } else {
            printf("NSF underrun!\n");
        }
        lastTimeTick = now + sleeptime;

        // continue;

        const unsigned secondsSinceStart = (now - emuStartTime) / 1000;
        if (secondsCount != secondsSinceStart) {
            secondsCount = secondsSinceStart;
            /*printf("T:%u F:%u S:%llu CycMain:%u CycSpentCPU:%u A:%u B:%u\n", now, FPS,
                cpu.apu->afx.getOutputSamplesAndReset(), sumCycles,
                cpu.instData.generalCycleCount - cpuCurGenCycCount,
                cpu.apu->callCyclesCount, cpu.apu->afx.getQueuedCount());*/
            sumCycles = 0;
            FPS = 0;
            cpu.apu->callCyclesCount = 0;
            cpuCurGenCycCount = cpu.instData.generalCycleCount;
        }
    }

    return 0;
}

RetroAudio retroAudio;
void pushAudioSample(short left, short right)
{
    retroAudio.loadSample(left);
}
