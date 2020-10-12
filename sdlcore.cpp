//#define SDL_MAIN_HANDLED
#define BENCH 0
//define DEBUGGER
#include <stdio.h>
#include "Cartridge/Cartridge.hpp"
#include "CPU.h"
#include "PPU.h"
#include <SDL2/SDL.h>
#include "wxSDLPanel.hpp"

#ifdef DEBUGGER
#include "Debugger/Debugger.h"
#endif // DEBUGGER

std::string getBaseRomName(std::string romFileName);

void sdlCore(ParamsForSDL const& sdlParams)
{
/********************************************************/
/*                   Timing info                        */
/********************************************************/
    SDL_Init(0);
    {
        #if BENCH == 1
        Logger bench ("Bench.txt");
        #endif
        //Master clock of NES machine.
        int NTSCmasterclock = 21477272;
        //CPU Frequency in Hz
        int cpufreq = NTSCmasterclock / 12;
        //Synchronization freq
        int syncfreq = 60;
        RetroFraction rafForCPUCycles(cpufreq, syncfreq);
        RetroFraction rafForTime(1000, syncfreq);
        int underrun = 0;
        unsigned frameCtr = 0;

    /********************************************************/
    /*               Modules Declaration                    */
    /********************************************************/

        /* ROM / Battery / Save State path names */
        //std::string romFileName   = "games/ppu_vbl_nmi/ppu_vbl_nmi.nes";
        //std::string romFileName   = "games/instr_test-v4/official_only.nes";

        //std::string romFileName   = "games/NROM/Baseball (USA, Europe).nes";
        //std::string romFileName   = "games/NROM/Balloon Fight.nes";
        //std::string romFileName   = "games/GxROM/Super Mario Bros. + Duck Hunt (USA).nes";
        //std::string romFileName   = "games/ColorDreams/Metal Fighter (USA) (Unl).nes";


        //std::string romFileName   = "games/blargg_ppu_tests/sprite_ram.nes";
        //std::string romFileName   = "games/mmc3_test_2/rom_singles/4-scanline_timing.nes";
        //std::string romFileName   = "games/NROM/smb.nes";
        std::string romFileName   =  "games/Jurassic Park (U) [!].nes";
        //std::string romFileName   = "games/AxROM/Battletoads (USA).nes";

        //std::string romFileName   = "games/CNROM/Adventure Island.nes";
        //std::string romFileName   = "games/CNROM/Ninja Jajamaru Kun (Japan).nes";
        //std::string romFileName   = "games/CNROM/Urusei Yatsura - Lum no Wedding Bell (Japan).nes";
        //std::string romFileName   = "games/CNROM/AV Pachi Slot (Japan) (Unl).nes";
        //std::string romFileName   = "games/CNROM/Jajamaru no Daibouken (Japan).nes";
        //std::string romFileName   = "games/CNROM/Paperboy (USA).nes";

        //std::string romFileName   = "games/cpu_exec_space/test_cpu_exec_space_apu.nes";
        //std::string romFileName   = "games/cpu_exec_space/test_cpu_exec_space_ppuio.nes";

        //std::string romFileName   = "games/UxROM/Contra.nes";
        //std::string romFileName   = "games/instr_timing/instr_timing.nes";
        //std::string romFileName   = "games/MMC4/Fire Emblem Gaiden (Japan).nes";
        //std::string romFileName   = "games/MMC4/Fire Emblem (Japan).nes";
        //std::string romFileName   = "games/MMC2/Mike Tyson's Punch-Out!! (USA).nes";
        //std::string romFileName   = "games/MMC3/Tiny Toon Adventures (USA).nes";
        //std::string romFileName   = "games/MMC3/Crystalis (USA).nes";
        //std::string romFileName   = "games/blargg_ppu_tests/sprite_ram.nes";
        //std::string romFileName   = "games/MMC3/Mega Man 3 (USA).nes";
        //std::string romFileName   =  "games/branch_timing_tests/2.Backward_Branch.nes";
        //std::string romFileName   =  "games/cpu_timing_test6/cpu_timing_test.nes";

        //std::string romFileName   =  "games/nes-test-roms-master/cpu_dummy_reads/cpu_dummy_reads.nes";
        //std::string romFileName   =  "games/cpu_interrupts_v2/cpu_interrupts.nes";
        //std::string romFileName   =  "games/nes-test-roms-master/cpu_reset/ram_after_reset.nes";
        //std::string romFileName   =  "games/cpu_timing_test6/cpu_timing_test.nes";
        //std::string romFileName   =  "games/nes-test-roms-master/instr_misc/instr_misc.nes";
        //std::string romFileName   =  "games/nes-test-roms-master/nmi_sync/demo_ntsc.nes";
        //std::string romFileName   =  "games/nes-test-roms-master/scanline-a1/scanline.nes";
        //std::string romFileName   =  "games/nes-test-roms-master/window5/colorwin_ntsc.nes";
        //std::string romFileName   =  "games/MMC3/Ninja Gaiden Episode III - The Ancient Ship of Doom (USA).nes";
        //std::string romFileName   =  "games/MMC3/Super C (USA).nes";
        //std::string romFileName   =  "games/MMC3/Super Mario Bros. 3 (USA).nes";
        //std::string romFileName   =  "games/MMC3/Bart vs. the World (USA).nes";


        //std::string romFileName = "games/mmc3_test_2/rom_singles/5-MMC3.nes";

        //std::string romFileName   =  "games/Mapper225/64-in-1 (J).nes";
        //std::string romFileName   =  "games/CNROM/B-Wings (J).nes";
        //std::string romFileName   =  "games/NROM/Nim & Nom 24-11-16.nes";

        //std::string romFileName   =  "games/NROM/Kung Fu.nes";
        //std::string romFileName   =  "games/dpcmletterbox.nes";
        //std::string romFileName   =  "games/MMC3/Rollerblade Racer (USA).nes";
        //std::string romFileName   =  "games/MMC3/Startropics (USA).nes";
        //std::string romFileName   =  "games/MMC3/Mickey's Safari in Letterland (USA).nes";
        //std::string romFileName   =  "games/MMC3/Mickey's Adventure in Numberland (USA).nes";
        //std::string romFileName   =  "games/MMC3/Burai Fighter (USA).nes";
        //std::string romFileName   =  "games/MMC3/Incredible Crash Dummies, The (USA).nes";
        //std::string romFileName   =  "games/MMC3/G.I. Joe - A Real American Hero (USA).nes";
        //std::string romFileName   =  "games/MMC3/chuchu.nes";

        //std::string romFileName   = "games/MMC1/Mega Man 2 (U).nes";
        //std::string romFileName   = "games/MMC1/Bill & Ted's Excellent Video Game Adventure (USA).nes";


        //std::string romFileName   =  "games/MMC1/bart.nes";
        //std::string romFileName   =  "games/MMC1/Legend of Zelda, The (U).nes";
        //std::string romFileName   =  "games/MMC1/Metroid (USA).nes";
        //std::string romFileName   =  "games/MMC1/Dragon Warrior III (USA).nes";
        //std::string romFileName   =  "games/MMC1/Double Dragon (USA).nes";
        //std::string romFileName   =  "games/MMC1/Kid Icarus - Angel Land Story (USA, Europe).nes";
        //std::string romFileName   =  "games/MMC1/Dragon Warrior IV (USA).nes";
        //std::string romFileName   =  "games/MMC1/Genghis Khan (USA).nes";
        //std::string romFileName   =  "games/MMC1/Romance of the Three Kingdoms (USA).nes";
        //std::string romFileName   =  "games/MMC1/Final Fantasy I & II (Japan).nes";
        //std::string romFileName   =  "C:/Users/Federico Gimeno/Desktop/6502_1.2.11/Nes_Practice/nes_basics.nes";
        //std::string romFileName   =  "games/MMC5/Castlevania III - Dracula's Curse (USA).nes";
        //std::string romFileName   =  "games/Camerica/Micro Machines (USA) (Unl).nes";
        //std::string romFileName   =  "games/Camerica/Fire Hawk (USA) (Unl).nes";
        //std::string romFileName   =  "games/Camerica/MiG 29 - Soviet Fighter (Camerica) [!].nes";

        //std::string romFileName   =  "games/MMC5/Laser Invasion (U) [!].nes";
        //std::string romFileName   =  "games/MMC5/Uncharted Waters (U) [!].nes";
        //std::string romFileName   =  "games/MMC5/Bandit Kings of Ancient China (U) [!].nes";
        //std::string romFileName   =  "games/MMC5/L'Empereur (J).nes";
        //std::string romFileName   =  "games/MMC5/Just Breed (J) [!].nes";


        //std::string romFileName   =  "games/MMC5/Shin 4 Nin Uchi Mahjong - Yakuman Tengoku (J) [!].nes";


        //std::string romFileName   =  "games/allpads/allpads.nes";
        //std::string romFileName   =  "games/MMC5/Metal Slader Glory (Japan).nes";
        //std::string romFileName   =  "games/MMC5/mmc5exram.nes";
        //std::string romFileName   =  "games/Incredible Crash DummiesIncredible Crash Dummies, The (USA).nesC The (USA).nes";
        //std::string romFileName   =  "games/sprdma_and_dmc_dma.nes";
        //std::string romFileName   =  "games/Jurassic Park (U) [!].nes";
        //std::string romFileName   =  "games/Overlord (U) [!].nes";
        //std::string romFileName   =  "games/Solstice - The Quest for the Staff of Demnos (U) [!].nes";
        //std::string romFileName   =  "games/Power Blade (U) [!].nes";
        //std::string romFileName   =  "games/Skate or Die 2 - The Search for Double Trouble (U) [!].nes";
        //std::string romFileName   =  "games/apu_sweep/min period.nes";
        //std::string romFileName   =  "games/apu_sweep/sweep cutoff.nes";
        //std::string romFileName   =  "games/apu_sweep/sweep sub.nes";
        //std::string romFileName   =  "games/apu_test/apu_test.nes";
        //std::string romFileName   =  "games/Huge Insect (Sachen) [!].nes";

        std::string saveStatePath = "SaveState";
        Cartridge cart(romFileName);
        saveStatePath.append(getBaseRomName(romFileName));
        saveStatePath.append(".sta");

        //printf("\nSave State path : %s", saveStatePath.c_str());

        /*Create the instances of the 6502 CPU, the Cartridge interface and the PPU*/
        CPU cpu (*cart.mapper);
        PPU ppu (cpu.io, *cart.mapper, sdlParams.GetWindow());
        cpu.setPPUPtr(&ppu);
        cpu.reset();
        ppu.reset();

        #ifdef DEBUGGER
            Debugger debuggerServer(&cpu, &ppu);
            debuggerServer.InitServer();
            //debuggerServer.InitAndWaitHandshake();
            //debuggerServer.handleRequests();
        #endif // DEBUGGER

        int pendCycles = 0;
        unsigned lastTimeTick = SDL_GetTicks();
        unsigned FPS = 0;
        unsigned sumCycles = 0;
        #ifdef DEBUG_PRECISETIMING
        unsigned emuStartTime = lastTimeTick;
        unsigned secondsCount = 0;
        unsigned cpuCurGenCycCount = 0;
        #endif // DEBUG_PRECISETIMING

        while (cpu.isRunning && sdlParams.IsRunning()){
            #ifdef DEBUGGER
                if(!debuggerServer.connected)
                {
                    if(debuggerServer.PollForDebugger())
                    {
                        debuggerServer.handleRequests();
                    }
                }
            #endif // DEBUGGER

            //HOW CAN WE FIX THIS?
            cpu.controller->updateJoystickState();

            SDL_Event event;
            bool windowClosed = false;
            while(SDL_PollEvent(&event))
            {
                const uint8_t* state = SDL_GetKeyboardState(NULL);

                if(event.type == SDL_QUIT)
                {
                    windowClosed = true;
                }
                if (state[SDL_SCANCODE_F7]){
                    cpu.isPaused = !cpu.isPaused;
                }
                if (state[SDL_SCANCODE_F8]){
                    cpu.reset();
                    ppu.reset();
                }
                if (state[SDL_SCANCODE_F10]){
                    FILE * file = fopen (saveStatePath.c_str(), "rb");
                    if (file != NULL){
                        cart.loadState(file);
                        cpu.loadState(file);
                        ppu.loadState(file);
                        printf("\nLoaded state \"%s\"", saveStatePath.c_str());
                        fclose(file);
                    } else {
                        printf("Error loading state file '%s'", saveStatePath.c_str());
                    }
                }
                if (state[SDL_SCANCODE_F11]){
                    FILE * file = fopen (saveStatePath.c_str(), "wb");
                    if (file != NULL){
                        cart.saveState(file);
                        cpu.saveState(file);
                        ppu.saveState(file);
                        fclose(file);
                        printf("\nSaved state \"%s\"", saveStatePath.c_str());
                    } else {
                        printf("Error creating state file '%s'", saveStatePath.c_str());
                    }
                }
                if (state[SDL_SCANCODE_F2]){
                    //cart.mapper->ppuStatus.debug = !cart.mapper->ppuStatus.debug;
                }
            }

            unsigned cycles = rafForCPUCycles.getNextSlice();
            sumCycles += cycles;
            pendCycles = cpu.run(cycles + pendCycles);
            //printf("C: %d A: %d B:%d\n", cpu.instData.generalCycleCount, cpu.apu->halfCycles, cpu.apu->b.getSize());
            frameCtr ++;

            unsigned now = SDL_GetTicks();
            unsigned timeSpent = now - lastTimeTick;
            unsigned thisFrameTimeInMilis = rafForTime.getNextSlice();
            int sleeptime = thisFrameTimeInMilis - timeSpent;
            FPS++;

            if (sleeptime > 0)
            {
                SDL_Delay(sleeptime);
            }
            else
                ++underrun;
            lastTimeTick = now + sleeptime;

            #ifdef DEBUG_PRECISETIMING
            const unsigned secondsSinceStart = (now - emuStartTime) / 1000;
            if(secondsCount != secondsSinceStart)
            {
                secondsCount = secondsSinceStart;
                printf("T:%u F:%u S:%llu CycMain:%u CycSpentCPU:%u A:%u B:%u\n", now, FPS,
                       cpu.apu->afx.getOutputSamplesAndReset(), sumCycles,
                       cpu.instData.generalCycleCount-cpuCurGenCycCount,
                       cpu.apu->callCyclesCount, cpu.apu->afx.getQueuedCount());
                sumCycles = 0;
                FPS = 0;
                cpu.apu->callCyclesCount = 0;
                cpuCurGenCycCount = cpu.instData.generalCycleCount;
            }
            #endif // DEBUG_PRECISETIMING

            #if BENCH == 1
            //Benchmark to file
            bench.LogWithPrefix("Benchmark", "%d, %6lf, %d\n", underrun, timediff*100/thisFrameTimeInMilis, frameCtr);
            #endif
            if (windowClosed){
                cpu.isRunning = false;
                break;
            }
        }
    }

}

/********************************************************/
/*                 Helper Functions                     */
/********************************************************/

std::string getBaseRomName(std::string tmpRomName){
    std::string fileNameBase;
    int extIndex = tmpRomName.find_last_of(".");
    tmpRomName.replace(tmpRomName.begin() + extIndex, tmpRomName.end(), "");
    int pathSepIndex = tmpRomName.find_last_of("/");
    tmpRomName.replace(tmpRomName.begin(), tmpRomName.begin() + pathSepIndex, "");
    fileNameBase = tmpRomName;
    return fileNameBase;
}
