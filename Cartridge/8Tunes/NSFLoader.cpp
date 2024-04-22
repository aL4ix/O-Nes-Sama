#include "NSFLoader.h"

NSFLoader::NSFLoader(std::string fileName)
    : romFileName(fileName)
{
    loadRomFile();
}

#define freadAndCheckReturnedValue(name, size)                  \
    if (fread(name, size, 1, file) == 0) {                      \
        Log.error("There was an error reading '" #name "' \n"); \
        return false;                                           \
    }

bool NSFLoader::loadRomFile()
{
    Log.debug(LogCategory::loaderNSF, "Loading NSF");
    FILE* file = fopen(romFileName.c_str(), "rb");
    char header[6];
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0x0, SEEK_SET);
        freadAndCheckReturnedValue(header, 5);
        header[5] = 0; // null terminating the header
        freadAndCheckReturnedValue(&nsfVersion, 1);
        freadAndCheckReturnedValue(&totalSongs, 1);
        freadAndCheckReturnedValue(&startingSong, 1);
        freadAndCheckReturnedValue(&loadAddress, 2);
        freadAndCheckReturnedValue(&initAddress, 2);
        freadAndCheckReturnedValue(&playAddress, 2);
        freadAndCheckReturnedValue(nameSong, 32);
        freadAndCheckReturnedValue(artist, 32);
        freadAndCheckReturnedValue(copyright, 32);
        freadAndCheckReturnedValue(&playSpeedNTSC, 2);
        freadAndCheckReturnedValue(bankswitch, 8);
        freadAndCheckReturnedValue(&playSpeedPAL, 2);
        freadAndCheckReturnedValue(&palOrNtsc, 1);
        freadAndCheckReturnedValue(&extraChips, 1);
        freadAndCheckReturnedValue(expansion, 4);
        long int curPos = ftell(file);
        prgSize = fileSize - curPos;

        io.prgBuffer = new unsigned char[8 * 0x1000];
        // Blank PRGs
        for (int b = 0; b < 8; b++)
            for (int i = 0; i < 0x1000; i++)
                io.prgBuffer[(b * 0x1000) | i] = (unsigned char)0xFF;

        for (int i = 0; i < 8; i++)
            bankswitch[i] &= 0x7; // Mask the bankswitches so they don't overflow

        isBankswitched = (bankswitch[0] | bankswitch[1] | bankswitch[2] | bankswitch[3] | bankswitch[4] | bankswitch[5] | bankswitch[6] | bankswitch[7]) != 0;
        Log.debug(LogCategory::loaderNSF, "isBankswitched: %d", isBankswitched);

        unsigned short offset = loadAddress & 0xFFF;
        unsigned short bytesPerBank = 0x1000 - offset;
        int b = 0;
        if (!isBankswitched) {
            b = (loadAddress - 0x8000) >> 12;
        }
        for (; b < 8; b++) {
            unsigned short bytesLoaded = fread(io.prgBuffer + (b * 0x1000) + offset, 1, bytesPerBank, file);
            Log.debug(LogCategory::loaderNSF, "Bytes Loaded: %x", bytesLoaded);
            if (bytesLoaded != bytesPerBank) {
                Log.debug(LogCategory::loaderNSF, "Loaded %d banks", b);
                break;
            }
        }

        fclose(file);
    } else {
        Log.error("NO FILE!");
        return false;
    }

    // Create Mapper
    mapper = new NSFMapper(io);

    Log.debug(LogCategory::loaderNSF, "Header: %s", header);
    Log.debug(LogCategory::loaderNSF, "NSF Version: %d", nsfVersion);
    Log.debug(LogCategory::loaderNSF, "Total Song: %x", totalSongs);
    Log.debug(LogCategory::loaderNSF, "Starting Song: %x", startingSong);
    Log.debug(LogCategory::loaderNSF, "Load Address: %x", loadAddress);
    Log.debug(LogCategory::loaderNSF, "Init Address: %x", initAddress);
    Log.debug(LogCategory::loaderNSF, "Play Address: %x", playAddress);
    Log.debug(LogCategory::loaderNSF, "Name Song: %s", nameSong);
    Log.debug(LogCategory::loaderNSF, "Artist: %s", artist);
    Log.debug(LogCategory::loaderNSF, "Copyright: %s", copyright);
    Log.debug(LogCategory::loaderNSF, "Play Speed NTSC: %x", playSpeedNTSC);
    Log.debug(LogCategory::loaderNSF, "Bankswitch: %02x%02x%02x%02x%02x%02x%02x%02x", bankswitch[0], bankswitch[1],
        bankswitch[2], bankswitch[3], bankswitch[4], bankswitch[5], bankswitch[6],
        bankswitch[7]);
    Log.debug(LogCategory::loaderNSF, "Play Speed PAL: %x", playSpeedPAL);
    Log.debug(LogCategory::loaderNSF, "PAL Or NTSC: %x", palOrNtsc);
    Log.debug(LogCategory::loaderNSF, "Extra Chips: %x", extraChips);
    Log.debug(LogCategory::loaderNSF, "Prg Size: %lx", prgSize);

    if (isBankswitched) {
        for (int i = 0; i < 8; i++)
            io.switch4K(i, bankswitch[i], io.prgBuffer, io.prgSpace);
    } else {
        for (int i = 0; i < 8; i++)
            io.switch4K(i, i, io.prgBuffer, io.prgSpace);
    }
    /*
    printf("1: %x\n", mapper->readCPU(0x8000));
    printf("2: %x\n", mapper->readCPU(0x9001));
    printf("3: %x\n", mapper->readCPU(0xa000));
    printf("4: %x\n", mapper->readCPU(0xb000));
    printf("5: %x\n", mapper->readCPU(0xc000));
    printf("6: %x\n", mapper->readCPU(0xd000));
    printf("7: %x\n", mapper->readCPU(0xe000));
    printf("8: %x\n", mapper->readCPU(0xf000));*/
    return true;
}

void NSFLoader::initializingATune(CPU& cpu, unsigned short int song)
{
    NSFMapper* nsfMapper = (NSFMapper*)mapper; // We created this mapper at construction time so we know it's this one
    nsfMapper->setCPU(&cpu);
    cpu.reset();
    cpu.apu->powerup();
    for (int address = 0x0000; address < 0x0800; address++)
        cpu.write(address, 0);
    for (int address = 0x6000; address < 0x8000; address++)
        cpu.write(address, 0);
    for (int address = 0x4000; address <= 0x4013; address++)
        cpu.apu->writeMem(address, 0);
    cpu.apu->writeMem(0x4015, 0x0);
    cpu.apu->writeMem(0x4015, 0xF);
    cpu.apu->writeMem(0x4017, 0x40);
    if (song != 0) {
        startingSong = song;
    }
    cpu.regs.a = startingSong - 1;
    cpu.regs.x = 0; // NTSC

    cpu.write(0x1FF, 0xFF);
    cpu.write(0x1FE, 0xF9);
    cpu.regs.sp = 0xFD;
    cpu.regs.pc = initAddress;
    cpu.run(30000);

    // From play()
    cpu.write(0x1FF, 0xFF);
    cpu.write(0x1FE, 0xF9);
    cpu.regs.sp = 0xFD;
    cpu.regs.pc = playAddress;
}

void NSFLoader::nfsDidARts(CPU& cpu)
{
    if (cpu.regs.pc == 0xfffa || cpu.regs.pc == 0xfffb) // NSF did a RTS
    {
        cpu.write(0x1FF, 0xFF);
        cpu.write(0x1FE, 0xF9);
        cpu.regs.sp = 0xFD;
        cpu.regs.pc = playAddress;
    }
}

void NSFLoader::saveState(FILE* file) { }
bool NSFLoader::loadState(FILE* file) { return true; }
