#include "ONesSamaCore.h"

ONesSamaCore::ONesSamaCore() :
    fractionForCPUCycles(cpufreq, syncfreq),
    fractionForTime(1000, syncfreq),
    saveStatePath("SaveState"),
    cart(nullptr),
    cpu(nullptr),
    ppu(nullptr)
{
}

bool ONesSamaCore::loadCartridge(std::string romFileName)
{
    Cartridge cart(romFileName);
    saveStatePath.append(getBaseRomName(romFileName));
    saveStatePath.append(".sta");

    //printf("\nSave State path : %s", saveStatePath.c_str());

    /*Create the instances of the 6502 CPU, the Cartridge interface and the PPU*/
    CPU cpu (*cart.mapper);
    PPU ppu (cpu.io, *cart.mapper);
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
}

bool ONesSamaCore::reset()
{
    cpu->reset();
    ppu->reset();
}

void ONesSamaCore::getFramebuffer()
{

}

void ONesSamaCore::getAudiobuffer()
{

}

bool ONesSamaCore::run(unsigned cycles)
{

}

void ONesSamaCore::setControllerOne(bool a, bool b, bool select, bool start, bool up, bool down, bool left, bool right)
{

}

void ONesSamaCore::setControllerTwo(bool a, bool b, bool select, bool start, bool up, bool down, bool left, bool right)
{

}

void ONesSamaCore::saveState()
{
    FILE * file = fopen (saveStatePath.c_str(), "wb");
    if (file != NULL){
        cart->saveState(file);
        cpu->saveState(file);
        ppu->saveState(file);
        fclose(file);
        printf("\nSaved state \"%s\"", saveStatePath.c_str());
    } else {
        printf("Error creating state file '%s'", saveStatePath.c_str());
    }
}

void ONesSamaCore::loadState()
{
    FILE * file = fopen (saveStatePath.c_str(), "rb");
    if (file != NULL){
        cart->loadState(file);
        cpu->loadState(file);
        ppu->loadState(file);
        printf("\nLoaded state \"%s\"", saveStatePath.c_str());
        fclose(file);
    } else {
        printf("Error loading state file '%s'", saveStatePath.c_str());
    }
}

void ONesSamaCore::pause()
{
    cpu->isPaused = !cpu->isPaused;
}

void ONesSamaCore::debug()
{
    //cart.mapper->ppuStatus.debug = !cart.mapper->ppuStatus.debug;
}


ONesSamaCore::~ONesSamaCore()
{
    if(ppu)
    {
        delete ppu;
    }
    if(cpu)
    {
        delete cpu;
    }
    if(cart)
    {
        delete cart;
    }
}

/********************************************************/
/*                 Helper Functions                     */
/********************************************************/

string ONesSamaCore::getBaseRomName(std::string tmpRomName)
{
    std::string fileNameBase;
    int extIndex = tmpRomName.find_last_of(".");
    tmpRomName.replace(tmpRomName.begin() + extIndex, tmpRomName.end(), "");
    int pathSepIndex = tmpRomName.find_last_of("/");
    tmpRomName.replace(tmpRomName.begin(), tmpRomName.begin() + pathSepIndex, "");
    fileNameBase = tmpRomName;
    return fileNameBase;
}
