#include "MMC3.hpp"
#include <stdlib.h>
#include <stdio.h>

MMC3::MMC3(CartIO & ioRef) : BasicMapper (ioRef){

    cycleDelay = 3;
    oldPPUA12   = 0;
    needsMCACC  = 0;
    bankSelect  = 0;
    mirroring   = 0;
    irqLatch    = 0;
    irqReload   = 0;
    irqCounter  = 0;
    irqEnable   = 0;
    prgSizeMask = 0;
    chrSizeMask = 0;
    edgeCount   = 0;

    /*MC-ACC Games that are detected via CRC32*/
    for (int i=0; i < 3; i++){
        if (MC_ACC_games[i] == io.iNESHeader.romCRC32){
            needsMCACC = true;
            break;
        }
    }

    if (io.iNESHeader.romCRC32 != lowGMan){

        io.wRam = new unsigned char [0x2000];
        io.switch8K(0, 0, io.wRam, io.wRamSpace);
    }

    printf ("\nNeeds MC_ACC Behavior   : %d", needsMCACC);


    prgSizeMask = (io.iNESHeader.prgSize16k << 1) - 1;

    commandRegs[6] = 0;
    commandRegs[7] = 1;

    sync();

}

void MMC3::writeCPU(int addr, unsigned char val){

    switch (addr >> 12){
        case 0x6: case 0x7:
            io.wRamSpace[(addr - 0x6000) >> 10][addr & 0x3FF] = val;
            break;
        case 0x8: case 0x9:
            if (addr & 1)
                commandRegs[bankSelect & 7] = val;
            else
                bankSelect = val;
            break;
        case 0xA: case 0xB:
            if (addr & 1){/* put ram protect logic here*/}
            else
                mirroring = val;
            break;
        case 0xC: case 0xD:
            if (addr & 1){
                irqCounter = 0;
                irqReload = 1;
            }
            else{
                irqLatch = val;
            }
            break;
        case 0xE: case 0xF:
            irqEnable = addr & 1;
            if (!irqEnable)
                io.cpuIO->irq = 0;
            break;
    }
    sync();
}

unsigned char MMC3::readPPU(int address){
    unsigned char ret = BasicMapper::readPPU(address);
    clockIRQCounter();
    return ret;
}

void MMC3::writePPU(int address, unsigned char val){

    clockIRQCounter();
    BasicMapper::writePPU(address, val);

}

void MMC3::syncPRG(){
    bool prgMode = bankSelect & 0x40;

    io.switch8K((!prgMode) << 1  , (prgSizeMask - 1) & prgSizeMask, io.prgBuffer, io.prgSpace);
    io.switch8K(prgMode << 1     , commandRegs[6]    & prgSizeMask, io.prgBuffer, io.prgSpace);
    io.switch8K(1                , commandRegs[7]    & prgSizeMask, io.prgBuffer, io.prgSpace);
    io.switch8K(3                , prgSizeMask       & prgSizeMask, io.prgBuffer, io.prgSpace);
}

void MMC3::syncCHR(){
    bool chrMode = (bankSelect & 0x80);
    int chrMask1K = (io.iNESHeader.chrSize8k << 3) - 1;

    //printf ("\n%x", chrMask1K);

    io.switch1K((chrMode  << 2) | 0, (commandRegs[0] & chrMask1K) & 0xFE, io.chrBuffer, io.chrSpace);
    io.switch1K((chrMode  << 2) | 1, (commandRegs[0] & chrMask1K) | 1   , io.chrBuffer, io.chrSpace);
    io.switch1K((chrMode  << 2) | 2, (commandRegs[1] & chrMask1K) & 0xFE, io.chrBuffer, io.chrSpace);
    io.switch1K((chrMode  << 2) | 3, (commandRegs[1] & chrMask1K) | 1   , io.chrBuffer, io.chrSpace);

    io.switch1K((!chrMode  << 2) | 0, commandRegs[2] & chrMask1K, io.chrBuffer, io.chrSpace);
    io.switch1K((!chrMode  << 2) | 1, commandRegs[3] & chrMask1K, io.chrBuffer, io.chrSpace);
    io.switch1K((!chrMode  << 2) | 2, commandRegs[4] & chrMask1K, io.chrBuffer, io.chrSpace);
    io.switch1K((!chrMode  << 2) | 3, commandRegs[5] & chrMask1K, io.chrBuffer, io.chrSpace);

}

void MMC3::sync(){

    setNTMirroring();
    syncPRG();
    syncCHR();

}

void MMC3::setNTMirroring(){
    if(mirroring & 1)
        io.switchHorMirroring();
    else
        io.switchVerMirroring();
}

void inline MMC3::clockCPU(){

    int ppuA12 = (io.ppuAddrBus >> 12) & 1;

    if (!ppuA12)
        edgeCount++;

    /*if (io.wRam[0] != 0x80){
        if ((io.wRam[0] != 0x80) && (io.wRam[1] == 0xDE) && (io.wRam[2] == 0xB0) && (io.wRam[3] == 0x61)){
            printf ("\n");
            for (int i = 0; i < 60; i++){
                printf ("%c", io.wRam[i]);
            }
            printf ("\n");
            exit(1);
        }
    }*/

}

void inline MMC3::clockPPU(){
    clockIRQCounter();
}

inline void MMC3::clockIRQCounter(){
    if (io.ppuAddrBus > 0x2FFF)
        io.ppuAddrBus &= 0x2FFF;

    int ppuA12 = (io.ppuAddrBus >> 12) & 1;

    int edgeCondition = 0;

    if (needsMCACC){
        edgeCondition = oldPPUA12 && !ppuA12;
    } else {
        edgeCondition = !oldPPUA12 && ppuA12;
    }

    //printf ("\n%d", cyclesToIgnore);

    if (edgeCondition){

        if (edgeCount > cycleDelay){
            if (irqReload){
                irqCounter = irqLatch;
                irqReload = 0;
            }

            else if (irqCounter == 0){
                irqCounter = irqLatch;
            }

            else{
                irqCounter--;
            }

            if (!irqCounter && irqEnable){
                io.cpuIO->irq = 1;
            }
        }
        edgeCount = 0;

    }
    oldPPUA12 = ppuA12;
}

void MMC3::saveSRAM(FILE * batteryFile){
    fwrite(io.wRam, 0x2000, 1, batteryFile);
}

void MMC3::loadSRAM(FILE * batteryFile){
    fread(io.wRam, 0x2000, 1, batteryFile);
}

void MMC3::loadState(FILE * file){

    fread(&oldPPUA12 , sizeof(int *), 1, file);
    fread(&needsMCACC, sizeof(int *), 1, file);
    fread(&edgeCount , sizeof(int *), 1, file);
    fread(&cycleDelay, sizeof(int *), 1, file);
    fread(commandRegs, sizeof (unsigned char), 8, file);
    fread(&bankSelect , sizeof (unsigned char *), 1, file);
    fread(&mirroring  , sizeof (unsigned char *), 1, file);
    fread(&irqLatch   , sizeof (unsigned char *), 1, file);
    fread(&irqReload  , sizeof (unsigned char *), 1, file);
    fread(&irqCounter , sizeof (unsigned char *), 1, file);
    fread(&irqEnable  , sizeof (unsigned char *), 1, file);

    if (io.wRam)
        fread(io.wRam, sizeof (unsigned char), 0x2000, file);
    if (io.iNESHeader.chrSize8k == 0)
        fread(io.chrBuffer, sizeof (unsigned char), 0x2000, file);
    fread(io.ntSystemRam, sizeof (unsigned char), 0x800, file);

    sync();

}

void MMC3::saveState(FILE * file){

    fwrite(&oldPPUA12 , sizeof(int *), 1, file);
    fwrite(&needsMCACC, sizeof(int *), 1, file);
    fwrite(&edgeCount , sizeof(int *), 1, file);
    fwrite(&cycleDelay, sizeof(int *), 1, file);
    fwrite(commandRegs, sizeof (unsigned char), 8, file);
    fwrite(&bankSelect , sizeof (unsigned char *), 1, file);
    fwrite(&mirroring  , sizeof (unsigned char *), 1, file);
    fwrite(&irqLatch   , sizeof (unsigned char *), 1, file);
    fwrite(&irqReload  , sizeof (unsigned char *), 1, file);
    fwrite(&irqCounter , sizeof (unsigned char *), 1, file);
    fwrite(&irqEnable  , sizeof (unsigned char *), 1, file);

    if (io.wRam)
        fwrite(io.wRam, sizeof (unsigned char), 0x2000, file);
    if (io.iNESHeader.chrSize8k == 0)
        fwrite(io.chrBuffer, sizeof (unsigned char), 0x2000, file);
    fwrite(io.ntSystemRam, sizeof (unsigned char), 0x800, file);

}

MMC3::~MMC3(){}
