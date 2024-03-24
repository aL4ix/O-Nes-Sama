#include "ONesSamaCore.h"

ONesSamaCore::ONesSamaCore()
    : fractionForCPUCycles(cpufreq, 60)
    , saveStatePath("SaveState")
    , cart(nullptr)
    , cpu(nullptr)
    , ppu(nullptr)
    , pendCycles(0)
{
}

bool ONesSamaCore::loadCartridge(std::string romFileName)
{
    cart = new Cartridge(romFileName);
    saveStatePath.append(getBaseRomName(romFileName));
    saveStatePath.append(".sta");

    // printf("\nSave State path : %s", saveStatePath.c_str());

    /*Create the instances of the 6502 CPU, the Cartridge interface and the PPU*/
    cpu = new CPU(*cart->mapper);
    ppu = new PPU(cpu->io, *cart->mapper);
    cpu->setPPUPtr(ppu);

#ifdef DEBUGGER
    Debugger debuggerServer(&cpu, &ppu);
    debuggerServer.InitServer();
    // debuggerServer.InitAndWaitHandshake();
    // debuggerServer.handleRequests();
#endif // DEBUGGER

    return true;
}

bool ONesSamaCore::unloadCartridge()
{
    if (ppu) {
        delete ppu;
        ppu = nullptr;
    }
    if (cpu) {
        delete cpu;
        cpu = nullptr;
    }
    if (cart) {
        delete cart;
        cart = nullptr;
    }
    return true;
}

bool ONesSamaCore::reset()
{
    cpu->reset();
    ppu->reset();
    return true;
}

unsigned char* ONesSamaCore::getPalettedFrameBuffer()
{
    return ppu->getPalettedFrameBuffer();
}

unsigned char* ONesSamaCore::getDefaultPalette()
{
    return ppu->getDefaultPalette();
}

void ONesSamaCore::setPushAudioSampleCallback(void (*pushAudioSampleCallback)(short left, short right))
{
    cpu->apu->setPushAudioSampleCallback(pushAudioSampleCallback);
}

int ONesSamaCore::run(const int cycles)
{
    return cpu->run(cycles);
}

bool ONesSamaCore::runOneFrame()
{
    unsigned cycles = fractionForCPUCycles.getNextSlice();
    pendCycles = run(cycles + pendCycles);
    return true;
}

void ONesSamaCore::setControllersMatrix(bool (*input)[8])
{
    cpu->controller->updatePlayersInput(input);
}

/*void ONesSamaCore::setControllerTwo(bool a, bool b, bool select, bool start, bool up, bool down, bool left, bool right)
{

}*/

void ONesSamaCore::saveState()
{
    FILE* file = fopen(saveStatePath.c_str(), "wb");
    if (file != NULL) {
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
    FILE* file = fopen(saveStatePath.c_str(), "rb");
    if (file != NULL) {
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
    // cart.mapper->ppuStatus.debug = !cart.mapper->ppuStatus.debug;
}

bool ONesSamaCore::getCPUIsRunning()
{
    return cpu->isRunning;
}

void ONesSamaCore::setCPUIsRunning(bool isRunning)
{
    cpu->isRunning = isRunning;
}

ONesSamaCore::~ONesSamaCore()
{
    unloadCartridge();
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
