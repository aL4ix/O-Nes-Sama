#ifndef ONESSAMACORE_H
#define ONESSAMACORE_H

#include <stdint.h>
#include <stddef.h>
#include "RetroEmu/RetroFraction.hpp"
#include <string>
#include "Cartridge/Cartridge.hpp"
#include "CPU.h"
#include "PPU.h"


class ONesSamaCore
{
    public:
        ONesSamaCore();
        virtual ~ONesSamaCore();
        bool loadCartridge(std::string fileName);
        bool unloadCartridge();
        bool reset();
        unsigned char* getPalettedFrameBuffer();
        unsigned char* getDefaultPalette();
        void setPushAudioSampleCallback(std::function<void(unsigned short left, unsigned short right)> pushAudioSampleCallback);
        bool run(unsigned cycles);
        void setControllersMatrix(bool (*input)[8]);
        //void setControllerTwo(bool a, bool b, bool select, bool start, bool up, bool down, bool left, bool right);
        void saveState();
        void loadState();
        void pause();
        void debug();
        void testMe();

    protected:

    private:
        //Master clock of NES machine.
        int NTSCmasterclock = 21477272;
        //CPU Frequency in Hz
        int cpufreq = NTSCmasterclock / 12;
        //Synchronization freq
        int syncfreq = 60;
        RetroFraction fractionForCPUCycles;
        RetroFraction fractionForTime;
        int underrun = 0;
        unsigned frameCtr = 0;
        std::string saveStatePath;

        Cartridge* cart;
        CPU* cpu;
        PPU* ppu;

        static std::string getBaseRomName(std::string romFileName);
};

#endif // ONESSAMACORE_H
