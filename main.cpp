#define SDL_MAIN_HANDLED
#define BENCH 0

#include "CPU.hpp"
#include "Cartridge/Cartridge.hpp"
#include "ONesSamaCore.hpp"
#include "PPU.hpp"
#include "RetroEmu/RetroAudio.hpp"
#include "RetroEmu/RetroFraction.hpp"
#include "RetroEmu/RetroGraphics.hpp"
#include "RetroEmu/RetroInput.hpp"
#include <SDL2/SDL.h>
#include <stdio.h>


std::string getBaseRomName(std::string romFileName);
void pushAudioSample(short left, short right);
RetroAudio retroAudio;

int main(int argc, char** argv)
{
    /********************************************************/
    /*                   Timing info                        */
    /********************************************************/
    SDL_Init(0);
    { // oNesSamaCore
#if BENCH == 1
        Logger bench("Bench.txt");
#endif
        // Synchronization freq
        int syncfreq = 60;
        RetroFraction rafForTime(1000, syncfreq);
        int underrun = 0;
        unsigned frameCtr = 0;

        /********************************************************/
        /*               Modules Declaration                    */
        /********************************************************/

        /* ROM / Command line arguments*/
        std::string romFileName = "roms/rom.nes";
        unsigned char nsfSongNumber = 0;
        if (argc > 1) {
            romFileName = argv[1];
        }
        if (argc > 2) {
            nsfSongNumber = std::stoi(argv[2]);
        }

        ONesSamaCore oNesSamaCore;
        oNesSamaCore.loadCartridge(romFileName);
        oNesSamaCore.setPushAudioSampleCallback(pushAudioSample);
        oNesSamaCore.nsfSongNumber = nsfSongNumber;

        const double ZOOM = 2.0;
        RetroGraphics retroGraphics(oNesSamaCore.getPPUInteralWidth(), oNesSamaCore.getPPUInteralHeight(), ZOOM);
        unsigned char* defaultPalette = oNesSamaCore.getDefaultPalette();
        retroGraphics.loadColorPaletteFromArray(defaultPalette);
        RetroInput retroInput;
        oNesSamaCore.reset();

        unsigned lastTimeTick = SDL_GetTicks();
        unsigned FPS = 0;
#ifdef DEBUG_PRECISETIMING
        unsigned emuStartTime = lastTimeTick;
        unsigned secondsCount = 0;
        unsigned cpuCurGenCycCount = 0;
#endif // DEBUG_PRECISETIMING

        while (oNesSamaCore.getCPUIsRunning()) {
            oNesSamaCore.setControllersMatrix(retroInput.updateAndGetInputs());

            SDL_Event event;
            bool windowClosed = false;
            while (SDL_PollEvent(&event)) {
                const uint8_t* state = SDL_GetKeyboardState(NULL);

                if (event.type == SDL_QUIT) {
                    windowClosed = true;
                }
                if (state[SDL_SCANCODE_F7]) {
                    oNesSamaCore.pause();
                }
                if (state[SDL_SCANCODE_F8]) {
                    oNesSamaCore.reset();
                }
                if (state[SDL_SCANCODE_F10]) {
                    oNesSamaCore.loadState();
                }
                if (state[SDL_SCANCODE_F11]) {
                    oNesSamaCore.saveState();
                }
                if (state[SDL_SCANCODE_F2]) {
                    oNesSamaCore.debug();
                }
            }

            oNesSamaCore.runOneFrame();
            frameCtr++;

            retroGraphics.drawBegin();
            retroGraphics.drawPaletted(oNesSamaCore.getPalettedFrameBuffer(),
                oNesSamaCore.getPPUInteralWidth() * oNesSamaCore.getPPUInteralHeight());
            retroGraphics.drawEnd();

            unsigned now = SDL_GetTicks();
            unsigned timeSpent = now - lastTimeTick;
            unsigned thisFrameTimeInMilis = rafForTime.getNextSlice();
            int sleeptime = thisFrameTimeInMilis - timeSpent;
            FPS++;

            if (sleeptime > 0) {
                SDL_Delay(sleeptime);
            } else
                ++underrun;
            lastTimeTick = now + sleeptime;

#ifdef DEBUG_PRECISETIMING
            const unsigned secondsSinceStart = (now - emuStartTime) / 1000;
            if (secondsCount != secondsSinceStart) {
                secondsCount = secondsSinceStart;
                Log.debug(LogCategory::onessamaMain, "T:%u F:%u S:%llu CycMain:%u CycSpentCPU:%u A:%u B:%u\n", now, FPS,
                    retroAudio.getOutputSamplesAndReset(), sumCycles,
                    cpu.instData.generalCycleCount - cpuCurGenCycCount,
                    cpu.apu->callCyclesCount, retroAudio.getQueuedCount());
                sumCycles = 0;
                FPS = 0;
                cpu.apu->callCyclesCount = 0;
                cpuCurGenCycCount = cpu.instData.generalCycleCount;
            }
#endif // DEBUG_PRECISETIMING

#if BENCH == 1
            // Benchmark to file
            bench.LogWithPrefix("Benchmark", "%d, %6lf, %d\n", underrun, timediff * 100 / thisFrameTimeInMilis, frameCtr);
#endif
            if (windowClosed) {
                oNesSamaCore.setCPUIsRunning(false);
                break;
            }
        } // while
    } // oNesSamaCore
    SDL_Quit();
    return 0;
}

/********************************************************/
/*                 Helper Functions                     */
/********************************************************/

void pushAudioSample(short left, short right)
{
    retroAudio.loadSample(left);
}
