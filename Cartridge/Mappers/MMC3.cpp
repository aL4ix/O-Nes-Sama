#include "MMC3.hpp"
#include <stdlib.h>
#include <stdio.h>

int cyclesToIgnore = 3;

MMC3::MMC3(CartIO & ioRef) : BasicMapper (ioRef){

    oldPPUA12   = 0;
    needsMCACC  = 0;
    bankSelect  = 0;
    mirroring   = 0;
    irqLatch    = 0;
    irqReload   = 0;
    irqCounter  = 0;
    irqEnable   = 0;
    altBehavior = 0;
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
    printf ("\nNeeds MC_ACC Behavior   : %d", needsMCACC);

    io.wRam = new unsigned char [0x2000];
    io.switch8K(0, 0, io.wRam, io.wRamSpace);
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

void inline MMC3::clockPPU(){}

inline void MMC3::clockIRQCounter(){

    int ppuA12 = (io.ppuAddrBus >> 12) & 1;

    int edgeCondition = 0;

    if (needsMCACC){
        edgeCondition = oldPPUA12 && !ppuA12;
    } else {
        edgeCondition = !oldPPUA12 && ppuA12;
    }

    //printf ("\n%d", cyclesToIgnore);

    if (edgeCondition){

        if (edgeCount > cyclesToIgnore){
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

/*bool MMC3::loadState(FILE * file){
    Board::loadState(file);
    for (int i = 0; i < 8; i++){
        commandRegs[i] = tempR[i];
    }
    bankSelect = tempR[8];
    mirroring  = tempR[9];
    irqLatch   = tempR[10];
    irqReload  = tempR[11];
    irqCounter = tempR[12];
    irqEnable  = tempR[13];
    sync();
}

void MMC3::saveState(FILE * file){
    for (int i = 0; i < 8; i++){
        tempR[i] = commandRegs[i];
    }
    tempR[8]  = bankSelect;
    tempR[9]  = mirroring;
    tempR[10] = irqLatch;
    tempR[11] = irqReload;
    tempR[12] = irqCounter;
    tempR[13] = irqEnable;
    Board::saveState(file);
}


void MMC3::saveSRAM(FILE * batteryFile) {
    MapperUtils::saveSRAM(wramBuffer, batteryFile);
}

void MMC3::loadSRAM(FILE * batteryFile) {
    MapperUtils::loadSRAM(wramBuffer, batteryFile);
}
*/


MMC3::~MMC3(){}
