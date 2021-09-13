#include "MMC3.hpp"
#include <stdlib.h>
#include <stdio.h>

int edges_m = 0;
int mcacc_cnt = -1;
int edge_latch = 0;
int edge_latch_l = 0;
bool preesc_l, preesc = false;


MMC3::MMC3(CartIO & ioRef) : BasicMapper (ioRef){

    cycleDelay  = 4;
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
    mcacc_cnt = 0;

    /*MC-ACC Games that are detected via CRC32*/
    for (int i=0; i < 3; i++){
        if (MC_ACC_games[i] == io.iNESHeader.romCRC32){
            needsMCACC = true;
            break;
        }
    }

    if (io.iNESHeader.romCRC32 != lowGMan){

        io.wRam = new unsigned char [0x2000];
        io.swapPRGRAM(0, 1);
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
            io.wRamSpace[addr & 0x1FFF] = val;
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
                edges_m=0;
                //mcacc_cnt = 0;

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

void MMC3::syncPRG(){
    bool prgMode = bankSelect & 0x40;

    io.swapPRGROM(8, (!prgMode) << 1  , (prgSizeMask - 1) & prgSizeMask, io.prgBuffer, 0);
    io.swapPRGROM(8, prgMode << 1     , commandRegs[6]    & prgSizeMask, io.prgBuffer, 0);
    io.swapPRGROM(8, 1                , commandRegs[7]    & prgSizeMask, io.prgBuffer, 0);
    io.swapPRGROM(8, 3                , prgSizeMask       & prgSizeMask, io.prgBuffer, 0);
}

void MMC3::syncCHR(){
    bool chrMode = (bankSelect & 0x80);
    int chrMask1K = (io.iNESHeader.chrSize8k << 3) - 1;

    io.swapCHR(1, (chrMode  << 2) | 0, (commandRegs[0] & chrMask1K) & 0xFE, io.chrBuffer);
    io.swapCHR(1, (chrMode  << 2) | 1, (commandRegs[0] & chrMask1K) | 1   , io.chrBuffer);
    io.swapCHR(1, (chrMode  << 2) | 2, (commandRegs[1] & chrMask1K) & 0xFE, io.chrBuffer);
    io.swapCHR(1, (chrMode  << 2) | 3, (commandRegs[1] & chrMask1K) | 1   , io.chrBuffer);

    io.swapCHR(1, (!chrMode  << 2) | 0, commandRegs[2] & chrMask1K, io.chrBuffer);
    io.swapCHR(1, (!chrMode  << 2) | 1, commandRegs[3] & chrMask1K, io.chrBuffer);
    io.swapCHR(1, (!chrMode  << 2) | 2, commandRegs[4] & chrMask1K, io.chrBuffer);
    io.swapCHR(1, (!chrMode  << 2) | 3, commandRegs[5] & chrMask1K, io.chrBuffer);

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


    if (!ppuA12)
        edgeCount++;
    if (edgeCount == cycleDelay){
        edge_latch_l = edge_latch;
        edge_latch = 0;
    }
}

void inline MMC3::clockPPU(){

    oldPPUA12 = ppuA12;
    ppuA12 = (*io.ppuAddrBus >> 12) & 1;

    if (needsMCACC){

        if (oldPPUA12 && !ppuA12) {

            mcacc_cnt = edges_m % 8;
            edges_m ++;

            if (mcacc_cnt == 0){
                edge_latch_l = edge_latch;
                edge_latch = 0;
            } else {
                edge_latch_l = edge_latch;
                edge_latch = 1;
            }

            if (!edge_latch_l && edge_latch) {
                clockIRQCounter();
            }
        }

    }
    else {

        if (!oldPPUA12 && ppuA12) {
            edgeCount = 0;
            edge_latch_l = edge_latch;
            edge_latch = 1;

            if (edge_latch_l == 0 && edge_latch == 1){
                //printf ("\nClocked MAPPER: %03d %03d %03d 0x%04x", edgeCount, *io.dbg.sl, *io.dbg.tick, *io.ppuAddrBus);
                clockIRQCounter();
            }
        }
    }
}

inline void MMC3::clockIRQCounter(){


    //New Behavior
   /*if ((irqReload) || (irqCounter == 0)){
        irqCounter = irqLatch;
        irqReload = 0;
        //edges_m=0;
    } else {
        irqCounter --;
    }

    if (irqCounter == 0){

        if (irqEnable){
            io.cpuIO->irq = 1;
        }
    }*/


    //Old behavior
    int lst_cnt = 0;

    if (irqReload && irqLatch == 0){
            io.cpuIO->irq = 1;
    }

    if ((irqReload) || (irqCounter == 0)){
        irqCounter = irqLatch;
        irqReload = 0;

    } else {
        lst_cnt = irqCounter;
        irqCounter --;
    }

    if (lst_cnt == 1 && irqCounter == 0){
        if (irqEnable){
            io.cpuIO->irq = 1;
        }
    }

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
