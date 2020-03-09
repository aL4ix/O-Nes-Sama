#include "NSFLoader.h"

NSFLoader::NSFLoader(const char* fileName) : mapper(NULL), romFileName(fileName)
{
    loadRomFile();
}

NSFLoader::~NSFLoader()
{
    if(mapper)
        delete mapper;
}

void NSFLoader::loadRomFile()
{
    FILE* file = fopen(romFileName, "rb");
    if(file != NULL)
    {
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0x6, SEEK_SET);
        fread(&totalSongs,    1, 1,  file);
        fread(&startingSong,  1, 1,  file);
        fread(&loadAddress,   2, 1,  file);
        fread(&initAddress,   2, 1,  file);
        fread(&playAddress,   2, 1,  file);
        fread(&nameSong,      1, 32, file);
        fread(artist,         1, 32, file);
        fread(copyright,      1, 32, file);
        fread(&playSpeedNTSC, 2, 1,  file);
        fread(bankswitch,     1, 8,  file);
        fread(&playSpeedPAL,  2, 1,  file);
        fread(&palOrNtsc,     1, 1,  file);
        fread(&extraChips,    1, 1,  file);
        fread(expansion,      1, 4,  file);
        long int curPos = ftell(file);
        prgSize = fileSize - curPos;

        io.prgBuffer = new unsigned char[8*0x1000];
        // Blank PRGs
        for(int b=0; b<8; b++)
            for(int i=0; i<0x1000; i++)
                io.prgBuffer[(b*0x1000) | i] = (unsigned char) 0xFF;

        for(int i=0; i<8; i++)
            bankswitch[i] &= 0x7; // Mask the bankswitches so they don't overflow

        isBankswitched = (bankswitch[0] | bankswitch[1] | bankswitch[2] |
           bankswitch[3] | bankswitch[4] | bankswitch[5] | bankswitch[6] |
           bankswitch[7]) != 0;
        //printf("isBankswitched: %d\n", isBankswitched);

        unsigned short offset = loadAddress & 0xFFF;
        unsigned short bytesPerBank = 0x1000 - offset;
        int b = 0;
        if(!isBankswitched)
        {
            b = (loadAddress - 0x8000) >> 12;
        }
        for(; b<8; b++)
        {
            unsigned short bytesLoaded = fread(io.prgBuffer + (b*0x1000) + offset, 1, bytesPerBank, file);
            printf("Bytes Loaded: %x\n", bytesLoaded);
            if(bytesLoaded != bytesPerBank)
            {
                printf("Loaded %d banks\n", b);
                break;
            }

        }

        fclose(file);
    }
    else
    {
        printf("NO FILE!\n");
        return;
    }

    // Create Mapper
    mapper = new NSFMapper(io);

    printf("Total Song: %x\n", totalSongs);
    printf("Starting Song: %x\n", startingSong);
    printf("Load Address: %x\n", loadAddress);
    printf("Init Address: %x\n", initAddress);
    printf("Play Address: %x\n", playAddress);
    printf("Name Song: %s\n", nameSong);
    printf("Artist: %s\n", artist);
    printf("Copyright: %s\n", copyright);
    printf("Play Speed NTSC: %x\n", playSpeedNTSC);
    printf("Bankswitch: %02x%02x%02x%02x%02x%02x%02x%02x\n", bankswitch[0], bankswitch[1],
           bankswitch[2], bankswitch[3], bankswitch[4], bankswitch[5], bankswitch[6],
           bankswitch[7]);
    printf("Play Speed PAL: %x\n", playSpeedPAL);
    printf("PAL Or NTSC: %x\n", palOrNtsc);
    printf("Extra Chips: %x\n", extraChips);
    printf("Prg Size: %lx\n", prgSize);

    if(isBankswitched)
    {
        for(int i=0; i<8; i++)
            io.switch4K(i, bankswitch[i], io.prgBuffer, io.prgSpace);
    }
    else
    {
        for(int i=0; i<8; i++)
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
}

void NSFLoader::initializingATune(CPU& cpu, unsigned short int song)
{
    mapper->setCPU(&cpu);
    cpu.reset();
    cpu.apu->powerup();
    cpu.apu->writeMem(0x4015, 0xF);
    cpu.apu->writeMem(0x4017, 0x40);
    if(song != 0)
    {
        startingSong = song;
    }
    cpu.regs.a = startingSong - 1;
    cpu.regs.x = 0; // NTSC
    for(int address=0x6000; address<0x8000; address++)
        cpu.write(address, 0);

    cpu.write(0x1FF, 0xFF);
    cpu.write(0x1FE, 0xF9);
    cpu.regs.sp = 0xFD;
    cpu.regs.pc = initAddress;
    cpu.run(30000);
}

void NSFLoader::play(CPU& cpu)
{
    //Master clock of NES machine.
    unsigned NTSCmasterclock = 21477272;
    //CPU Frequency in Hz
	unsigned cpufreq = NTSCmasterclock / 12;
	//Synchronization freq
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

    cpu.write(0x1FF, 0xFF);
    cpu.write(0x1FE, 0xF9);
    cpu.regs.sp = 0xFD;
    cpu.regs.pc = playAddress;

    for(int i=0; i<10000; i++)
    {
        const unsigned cycles = rafForCPUCycles.getNextSlice();
        const unsigned requestedCycles = cycles + pendCycles;
        sumCycles += requestedCycles;

        pendCycles = cpu.run(requestedCycles);
        //printf("slice:%u pend:%d\n", cycles, pendCycles);
        if(cpu.regs.pc == 0xfffa || cpu.regs.pc == 0xfffb) // NSF did a RTS
        {
            cpu.write(0x1FF, 0xFF);
            cpu.write(0x1FE, 0xF9);
            cpu.regs.sp = 0xFD;
            cpu.regs.pc = playAddress;
        }

        unsigned now = SDL_GetTicks();
        unsigned timeSpent = now - lastTimeTick;
        unsigned thisFrameTimeInMilis = rafForTime.getNextSlice();
        int sleeptime = thisFrameTimeInMilis - timeSpent;
        FPS++;

        if(sleeptime > 0)
        {
            SDL_Delay(sleeptime);
        }
        else
        {
            printf("NSF underrun!\n");
        }
        lastTimeTick = now + sleeptime;

        //continue;

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
    }

}
