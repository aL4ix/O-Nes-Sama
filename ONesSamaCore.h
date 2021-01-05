#ifndef ONESSAMACORE_H
#define ONESSAMACORE_H

#include <stdint.h>
#include <stddef.h>
#include <string>
#include "Cartridge/Cartridge.hpp"
#include "CPU.h"
#include "PPU.h"
#include "RetroEmu/RetroFraction.hpp"


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
    void setPushAudioSampleCallback(void (*pushAudioSampleCallback)(short left, short right));
    int run(const int cycles);
    bool runOneFrame();
    void setControllersMatrix(bool (*input)[8]);
    //void setControllerTwo(bool a, bool b, bool select, bool start, bool up, bool down, bool left, bool right);
    void saveState();
    void loadState();
    void pause();
    void debug();
    constexpr unsigned getPPUInteralWidth()
    {
        return ppu->INTERNAL_WIDTH;
    }
    constexpr unsigned getPPUInteralHeight()
    {
        return ppu->INTERNAL_HEIGHT;
    }
    constexpr unsigned getPPUFPS()
    {
        return ppu->INTERNAL_FPS;
    }
    bool getCPUIsRunning();
    void setCPUIsRunning(bool isRunning);


    static constexpr int NTSCmasterclock = 21477272;
    //CPU Frequency in Hz
    static constexpr int cpufreq = NTSCmasterclock / 12;
    RetroFraction fractionForCPUCycles;

protected:

private:
    std::string saveStatePath;
    Cartridge* cart;
    CPU* cpu;
    PPU* ppu;
    int pendCycles;

    static std::string getBaseRomName(std::string romFileName);
};

#endif // ONESSAMACORE_H
