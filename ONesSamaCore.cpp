#include "ONesSamaCore.h"

ONesSamaCore::ONesSamaCore()
    : fractionForCPUCycles(cpufreq, 60)
    , saveStatePath("SaveState")
    , romLoader(nullptr)
    , cpu(nullptr)
    , ppu(nullptr)
    , pendCycles(0)
    , isNSF(false)
{
}

bool ONesSamaCore::loadCartridge(std::string romFileName)
{
    isNSF = false;
    if (case_insensitive_ends_with(romFileName, ".nsf")) {
        isNSF = true;
        romLoader = new NSFLoader(romFileName);
    } else {
        romLoader = new Cartridge(romFileName);
    }

    saveStatePath.append(getBaseRomName(romFileName));
    saveStatePath.append(".sta");

    // printf("\nSave State path : %s", saveStatePath.c_str());

    /*Create the instances of the 6502 CPU, the Cartridge interface and the PPU*/
    cpu = new CPU(*romLoader->mapper);
    ppu = new PPU(cpu->io, *romLoader->mapper);
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
    delete ppu;
    ppu = nullptr;
    delete cpu;
    cpu = nullptr;
    delete romLoader;
    romLoader = nullptr;
    return true;
}

bool ONesSamaCore::reset()
{
    if (isNSF) {
        NSFLoader* nsfLoader = (NSFLoader*)romLoader;
        nsfLoader->initializingATune(*cpu, nsfSongNumber);
    } else {
        cpu->reset();
    }
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
    int remainingCycles = cpu->run(cycles);
    if (isNSF) {
        NSFLoader* nsfLoader = (NSFLoader*)romLoader;
        nsfLoader->nfsDidARts(*cpu);
    }
    return remainingCycles;
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
        romLoader->saveState(file);
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
        romLoader->loadState(file);
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
    // romLoader.mapper->ppuStatus.debug = !romLoader.mapper->ppuStatus.debug;
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

// C++20 has ends_with that is why this method is using snake_case
bool ONesSamaCore::case_insensitive_ends_with(std::string const& fullString, std::string const& ending)
{
    std::string cloned = fullString;
    if (cloned.length() >= ending.length()) {
        return (0 == cloned.compare(cloned.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
