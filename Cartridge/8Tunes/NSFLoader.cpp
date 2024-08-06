#include "NSFLoader.hpp"

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
    Log.info("Loading NSF");
    FILE* file = fopen(romFileName.c_str(), "rb");
    char header[6];
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0x0, SEEK_SET);
        freadAndCheckReturnedValue(header, 5);
        header[5] = 0; // null terminating the header string
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
        Log.info("isBankswitched: %d", isBankswitched);

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

    Log.info("Header: %s", header);
    Log.info("NSF Version: %d", nsfVersion);
    Log.info("Total Song: %d", totalSongs);
    Log.info("Starting Song: %d", startingSong);
    Log.info("Load Address: %x", loadAddress);
    Log.info("Init Address: %x", initAddress);
    Log.info("Play Address: %x", playAddress);
    Log.info("Name Song: %s", nameSong);
    Log.info("Artist: %s", artist);
    Log.info("Copyright: %s", copyright);
    Log.info("Play Speed NTSC: %x", playSpeedNTSC);
    Log.info("Bankswitch: %02x%02x%02x%02x%02x%02x%02x%02x", bankswitch[0], bankswitch[1],
        bankswitch[2], bankswitch[3], bankswitch[4], bankswitch[5], bankswitch[6],
        bankswitch[7]);
    Log.info("Play Speed PAL: %x", playSpeedPAL);
    Log.info("PAL Or NTSC: %x", palOrNtsc);
    Log.info("Extra Chips: %x", extraChips);
    Log.info("Prg Size: %lx", prgSize);

    if (isBankswitched) {
        for (int i = 0; i < 8; i++)
            io.switch4K(i, bankswitch[i], io.prgBuffer, io.prgSpace);
    } else {
        for (int i = 0; i < 8; i++)
            io.switch4K(i, i, io.prgBuffer, io.prgSpace);
    }

    /*
    Log.debug(LogCategory::loaderNSF, "1: %x", mapper->readCPU(0x8000));
    Log.debug(LogCategory::loaderNSF, "2: %x", mapper->readCPU(0x9001));
    Log.debug(LogCategory::loaderNSF, "3: %x", mapper->readCPU(0xa000));
    Log.debug(LogCategory::loaderNSF, "4: %x", mapper->readCPU(0xb000));
    Log.debug(LogCategory::loaderNSF, "5: %x", mapper->readCPU(0xc000));
    Log.debug(LogCategory::loaderNSF, "6: %x", mapper->readCPU(0xd000));
    Log.debug(LogCategory::loaderNSF, "7: %x", mapper->readCPU(0xe000));
    Log.debug(LogCategory::loaderNSF, "8: %x", mapper->readCPU(0xf000));
    */

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
