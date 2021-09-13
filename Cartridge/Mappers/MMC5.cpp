#include "MMC5.hpp"
#include <stdlib.h>

int mask1K;
int mask2K;
int mask4K;
int mask8K;
int ppuRdCount = 0;
int idleCount = 0;
int lastAddress = 0;
bool ppuIsReading = false;
int bgfc = 0;

MMC5::MMC5(CartIO & ioRef) : BasicMapper (ioRef){

    mask1K = (io.iNESHeader.chrSize8k << 3) - 1;
    mask2K = (io.iNESHeader.chrSize8k << 2) - 1;
    mask4K = (io.iNESHeader.chrSize8k << 1) - 1;
    mask8K = io.iNESHeader.chrSize8k - 1;

    ntAddr = -1;
    curTile = -1;
    tilebank = 0;
    is8x16 = 0;
    prgMode = 3;
    chrMode = 0;
    wRamProt1 = 0;
    wRamProt2 = 0;
    exRamMode = 0;
    ntMapping = 0;
    fillModeTile = 0;
    fillModeColor = 0;
    prgRamBank = 0;
    chrUpperBank = 0;
    vSplitMode = 0;
    vSplitScroll = 0;
    vSplitBank = 0;
    irqScanline = 0;
    irqEnabled = 0;
    irqStatus = 0;
    multiplier[0] = 0;
    multiplier[1] = 0;
    prgBanks[0] = 0;
    prgBanks[1] = 0;
    prgBanks[2] = 0;
    prgBanks[3] = 0xFF;
    curChrSet = 0;
    io.wRam = new unsigned char [0x10000]; //64 K
    sync();

}

unsigned char MMC5::readCPU(int address){
    unsigned char ret = BasicMapper::readCPU(address);
    switch (address){
        case 0x5204:
            {
                ret = irqStatus;
                irqStatus &= ~0x80;
                io.cpuIO->irq = 0;
                break;
            }
        case 0x5205:
            ret = (multiplier[0] * multiplier[1]);
            break;
        case 0x5206:
            ret = (multiplier[0] * multiplier[1]) >> 8;
            break;
    }

    if ((address >= 0x5C00) && (address <= 0x5FFF)){
        int exRamAddr = address & 0x3FF;

        switch (exRamMode){
            case 0: case 1:
                ret = io.cpuIO->dataBus;
                break;
            case 2: case 3:
                ret = exRam[exRamAddr];
                break;
        }
    }

    return ret;
}

void MMC5::writeCPU(int address, unsigned char val){

    switch (address){
        case 0x5100:
            prgMode = val & 3;
            //printf ("\nPRG Mode %X", prgMode);
            break;
        case 0x5101:
            chrMode = val & 3;
            //printf ("\nCHR Mode %X", chrMode);
            break;
        case 0x5102:
            wRamProt1 = val & 3;
            break;
        case 0x5103:
            wRamProt2 = val & 3;
            break;
        case 0x5104:
            exRamMode = val & 3;
            //printf ("\nExram Mode: %x", val);
            break;
        case 0x5105:
            ntMapping = val;
            //printf ("\nNT Mapping: %x", val);
            break;
        case 0x5106: //Fill Mode Tile
            fillModeTile = val;
            for (int i = 0; i < 0x3C0; i++){
                fillMode[i] = fillModeTile;
            }
            break;
        case 0x5107: //Fill Mode Color
            fillModeColor = val & 3;
            for (int i = 0x3C0; i < 0x400; i++){
                fillMode[i] = fillModeColor | (fillModeColor << 2) | (fillModeColor << 4) | (fillModeColor << 6);
            }
            break;
        case 0x5113:
            prgRamBank = val & 7;
            //printf ("PRG RAM Bank %d", prgRamBank);
            io.swapPRGRAM(prgRamBank, 1);
            break;
        case 0x5114:
            prgBanks[0] = val;
            break;
        case 0x5115:
            prgBanks[1] = val;
            break;
        case 0x5116:
            prgBanks[2] = val;
            break;
        case 0x5117:
            prgBanks[3] = val & 0x7F;
            break;
        case 0x5120: case 0x5121: case 0x5122: case 0x5123: case 0x5124: case 0x5125:
        case 0x5126: case 0x5127:
            chrSelA[address & 0x7] = val;
            curChrSet = 0;
            break;
        case 0x5128: case 0x5129: case 0x512A: case 0x512B:
            chrSelB[address & 0x3] = val;
            curChrSet = 1;
            break;
        case 0x5130:
            chrUpperBank = val & 7;
            break;
        case 0x5200:
            vSplitMode = val;
            break;
        case 0x5201:
            vSplitScroll = val;
            break;
        case 0x5202:
            vSplitBank = val;
            break;
        case 0x5203:
            irqScanline = val;
            break;
        case 0x5204:
            irqEnabled = val >> 7;
            //if (irqEnabled && (irqStatus & 0x80)){
            //    io.cpuIO->irq = 1;
            //} else {
            //     io.cpuIO->irq = 0;
            //}
            break;
        case 0x5205:
            multiplier[0] = val;
            break;
        case 0x5206:
            multiplier[1] = val;
            break;
    }

    //Set the PPU vRAM handlers
    if ((vSplitMode & 0x80) && (exRamMode < 2)){
        if (exRamMode == 1){
            //Split & ExAttributes
        } else {
            ppuReadFunc = &MMC5::readPPUVSplit;
        }
    } else if (exRamMode == 1) {
        ppuReadFunc = &MMC5::readPPUExAttr;
        //ppuReadFunc = &MMC5::basicReadPPU;
    }
    else {
        ppuReadFunc = &MMC5::basicReadPPU;
    }

    if ((address >= 0x5C00) && (address <= 0x5FFF)){
        int exRamAddr = address & 0x3FF;

        if (exRamMode != 3){
            if ((irqStatus & 0x40) || (exRamMode == 2)){
                    exRam[exRamAddr] = val;
            } else {
                exRam[exRamAddr] = 0;
            }
        }
    }

    switch (address >> 12){
        case 8: case 9: case 10: case 11:
        case 12: case 13: case 14:case 15:
            io.prgSpace[(address >> 10) & 0x1F][address & 0x3FF] = val;
            break;
    }

    BasicMapper::writeCPU(address, val);
    sync();

}

unsigned char MMC5::readPPU(int address){

    address &= 0x3fff;
    //if (address >= 0x3000) address -= 0x1000;


    //Checks if 3 consecutive nametable/attribute fetches have occurred from the same address
    if ((address >= 0x2000) && (address <= 0x2FFF)){
        //Watch the BG fetches from nt range


        //IRQ counter logic
        /*if (*io.dbg.tick == 337){
            printf ("\n%YAAAAAH %x %x %d %d", lastAddress, address, *io.dbg.sl, *io.dbg.tick);
        }*/
        if (lastAddress == address){

            ppuRdCount++;
            //printf ("\n%x %x %d %d", lastAddress, address, *io.dbg.sl, *io.dbg.tick);
            if (ppuRdCount == 2){
                //printf ("\n%x %x %d %d", lastAddress, address, *io.dbg.sl, *io.dbg.tick);
                if (irqStatus & 0x40){
                    irqCounter++;
                    clockIRQ();
                }
                else {
                    irqStatus |= 0x40;
                    irqCounter=0;
                }
            }

        }


    } else {
        ppuRdCount = 0;
    }

    lastAddress = address;
    ppuIsReading = true;
    sync();
    return (this->*ppuReadFunc)(address);
}

unsigned char MMC5::basicReadPPU(int address){
    return BasicMapper::readPPU(address);
}

unsigned char MMC5::readPPUExAttr(int address){

    if (isFetchingSpr){
        return BasicMapper::readPPU(address);
    }

    if (address >= 0x3000)
        address &= 0x2FFF;

    int div400 = address >> 10;
    *io.ppuAddrBus = address;

    switch (div400){
        case 0: case 1: case 2: case 3: case 4:
        case 5: case 6: case 7:


            break;
        case 8: case 9: case 10: case 11:
            if ((address & 0x3FF) < 0x3C0){
                ntAddr = address & 0x3FF;
                tilebank = (exRam[ntAddr] & 0x3F) | (chrUpperBank << 6);
                io.swapCHR(4, 0, tilebank & mask4K, io.chrBuffer);
                io.swapCHR(4, 1, tilebank & mask4K, io.chrBuffer);
                return BasicMapper::readPPU(address);
            }
            else {
                unsigned char color = exRam[ntAddr] >> 6;

                return color | (color << 2) | (color << 4) | (color << 6);
            }
    }

    return BasicMapper::readPPU(address);
}

unsigned char MMC5::readPPUVSplit(int address){

    if (isFetchingSpr){
        return BasicMapper::readPPU(address);
    }

    if (address >= 0x3000)
        address &= 0x2FFF;

    int div400 = address >> 10;
    *io.ppuAddrBus = address;


    switch (div400){
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            if (isSplitArea){
                io.swapCHR(4, 0, vSplitBank, io.chrBuffer);
                io.swapCHR(4, 1, vSplitBank, io.chrBuffer);
                //return io.chrSpace[address >> 10][(address & 0x3F8) | (vScroll & 7)];
            }
            break;
        case 8: case 9: case 10: case 11:
            if ((address & 0x3FF) < 0x3C0){
                //curTile = address & 0x1F;


                if (isSplitArea)
                    return exRam[((vScroll & 0xF8) << 2) | (curTile & 0x1F)];
            } else {
                if (isSplitArea)
                    return exRam[0x3C0 | (((vScroll & 0xF8) << 2) | (curTile & 0x1F)) >> 2];
            }
    }
    return BasicMapper::readPPU(address);
}


void MMC5::writePPU(int address, unsigned char val){
    ppuRdCount = 0;
    ppuIsReading = false;
    BasicMapper::writePPU(address, val);
}

void MMC5::clockPPU(){

    /*if (*io.ppuAddrBus>= 0x3000)
        *io.ppuAddrBus &= 0x2FFF;*/

    isFetchingSpr = (*io.dbg.tick >= 257) && (*io.dbg.tick <= 319);
    isFetchingNT = (*io.ppuAddrBus >= 0x2000) && ((*io.ppuAddrBus & 0x3FF) < 0x3C0);

    /*if (!(*io.dbg.isRendering)){
        irqStatus &= ~0x80;
        irqStatus &= ~0x40;
        irqCounter = -1;
    }*/

    /*if (!isFetchingSpr){
        if (!(*io.dbg.tick & 7) && (*io.dbg.tick < 336)){
            curTile++;
            if ((vSplitMode & 0x80) && (exRamMode < 2)){
                if (vSplitMode & 0x40){
                    if (curTile == (vSplitMode & 0x1F)){
                        isSplitArea = 1;
                    }
                    else if (curTile == 0) {
                        isSplitArea = 0;
                    } else if (curTile >= 34){
                        isSplitArea = 0;
                    }
                } else {
                    if (curTile == 0){
                        isSplitArea = 1;
                    }
                    else if (curTile == (vSplitMode & 0x1F)) {
                        isSplitArea = 0;
                    }
                }
            }
        }
    } else {
        isSplitArea = 0;
    }

    if (*io.dbg.tick == 320){
        curTile = 0;
        if (*io.dbg.sl == -1){
            vScroll = vSplitScroll;
            if (vScroll >= 240)
              vScroll -= 16;
        } else if (*io.dbg.sl < 240) {
            vScroll++;
        }
        if (vScroll >= 240) {
            vScroll -= 240;
        }
    }

    syncCHR();
    clockIRQ();

    lastAddr = *io.ppuAddrBus;*/
}

void MMC5::clockCPU(){

    int isPPUAccess = (io.cpuIO->addressBus >= 0x2000) && (io.cpuIO->addressBus <= 0x3FFF);
    int readingNmiVec = (io.cpuIO->wr == 0) && ((io.cpuIO->addressBus == 0xFFFA) || (io.cpuIO->addressBus == 0xFFFB));

    if (readingNmiVec) {
        irqStatus &= ~0x40;
    }

    if (ppuIsReading){
        idleCount = 0;
    } else {
        idleCount++;
        if (idleCount == 3){
            irqStatus &= ~0x40;
        }
    }

    ppuIsReading = false;

    if (isPPUAccess){
        int ppuReg = io.cpuIO->addressBus & 7;
        if (io.cpuIO->wr){
            switch (ppuReg){
                case 0:
                    is8x16 = ((io.cpuIO->dataBus) & 0x20);
                    break;
                case 1:
                    if (io.cpuIO->dataBus & 0x18){
                        //irqStatus &= ~0x40;
                    }
                    break;
            }
        }
    }
}

void MMC5::clockIRQ(){

    if (irqCounter == 241){
        getchar();
        irqStatus &= ~0x40;
        irqCounter = 0;
        io.cpuIO->irq = 0;
    }

    if (irqCounter == 0){
        irqStatus &= ~0x80;
        io.cpuIO->irq = 0;
    }

    if (irqScanline == irqCounter){
        irqStatus |= 0x80;
        if (irqEnabled){
            io.cpuIO->irq = 1;
        }
    }
}

void MMC5::syncPRG(){

    int mask8  = (io.iNESHeader.prgSize16k << 1) - 1;
    int mask16 = (io.iNESHeader.prgSize16k) - 1;
    int mask32 = (io.iNESHeader.prgSize16k >> 1) - 1;

    switch (prgMode){
        case 0:
            io.swapPRGROM(32, 0, (prgBanks[3] >> 2) & mask32, io.prgBuffer, 0);
            break;
        case 1:
            if (prgBanks[1] & 0x80)
                io.swapPRGROM(16, 0, (prgBanks[1] >> 1) & mask16, io.prgBuffer, 0);
            else
                io.swapPRGROM(16, 0, (prgBanks[1] >> 1) & 7, io.wRam, 1);

            io.swapPRGROM(16, 1, (prgBanks[3] >> 1) & mask16, io.prgBuffer, 0);
            break;
        case 2:
            if (prgBanks[1] & 0x80)
                io.swapPRGROM(16, 0, (prgBanks[1] >> 1) & mask16, io.prgBuffer, 0);
            else
                io.swapPRGROM(16, 0, (prgBanks[1] >> 1) & 7, io.wRam, 1);

            if (prgBanks[2] & 0x80)
                io.swapPRGROM(8, 2, prgBanks[2] & mask8, io.prgBuffer, 0);
            else
                io.swapPRGROM(8, 2, prgBanks[2] & 7, io.wRam, 1);

            io.swapPRGROM(8, 3, prgBanks[3] & mask8, io.prgBuffer, 0);
            break;
        case 3:
            if (prgBanks[0] & 0x80)
                io.swapPRGROM(8, 0, prgBanks[0] & mask8, io.prgBuffer, 0);
            else{
                io.swapPRGROM(8, 0, prgBanks[0] & 7, io.wRam, 1);
            }

            if (prgBanks[1] & 0x80)
                io.swapPRGROM(8, 1, prgBanks[1] & mask8, io.prgBuffer, 0);
            else{
                io.swapPRGROM(8, 1, prgBanks[1] & 7, io.wRam, 1);
            }

            if (prgBanks[2] & 0x80)
                io.swapPRGROM(8, 2, prgBanks[2] & mask8, io.prgBuffer, 0);
            else{
                io.swapPRGROM(8, 2, prgBanks[2] & 7, io.wRam, 1);

            }

            io.swapPRGROM(8, 3, prgBanks[3] & mask8, io.prgBuffer, 0);
            break;
    }
}

void MMC5::sync(){
    syncPRG();
    syncCHR();
    syncNT();
}

void MMC5::syncCHR(){

    if ((exRamMode == 1 && !isFetchingSpr) || isSplitArea){
        return;
    }

    if (is8x16){

        if (!isFetchingSpr && (irqStatus & 0x40)){
            syncCHRB();
        }
        else {
            syncCHRA();
        }

    } else {

        if (curChrSet){
            syncCHRB();
        }
        else {
            syncCHRA();
        }
    }
}


void MMC5::syncCHRA(){

    switch(chrMode){
        case 0: //8K Banks
            io.swapCHR(8, 0, chrSelA[7] & mask8K, io.chrBuffer);
            break;
        case 1: //4K Banks
            io.swapCHR(4, 0, chrSelA[3] & mask4K, io.chrBuffer);
            io.swapCHR(4, 1, chrSelA[7] & mask4K, io.chrBuffer);
            break;
        case 2: //2K Banks
            io.swapCHR(2, 0, chrSelA[1] & mask2K, io.chrBuffer);
            io.swapCHR(2, 1, chrSelA[3] & mask2K, io.chrBuffer);
            io.swapCHR(2, 2, chrSelA[5] & mask2K, io.chrBuffer);
            io.swapCHR(2, 3, chrSelA[7] & mask2K, io.chrBuffer);

            break;
        case 3: //1K Banks
            io.swapCHR(1, 0, chrSelA[0] & mask1K, io.chrBuffer);
            io.swapCHR(1, 1, chrSelA[1] & mask1K, io.chrBuffer);
            io.swapCHR(1, 2, chrSelA[2] & mask1K, io.chrBuffer);
            io.swapCHR(1, 3, chrSelA[3] & mask1K, io.chrBuffer);
            io.swapCHR(1, 4, chrSelA[4] & mask1K, io.chrBuffer);
            io.swapCHR(1, 5, chrSelA[5] & mask1K, io.chrBuffer);
            io.swapCHR(1, 6, chrSelA[6] & mask1K, io.chrBuffer);
            io.swapCHR(1, 7, chrSelA[7] & mask1K, io.chrBuffer);
            break;
    }
}

void MMC5::syncCHRB(){

    switch(chrMode){
        case 0: //8K Banks
            io.swapCHR(8, 0, chrSelB[0x3] & mask8K, io.chrBuffer);
            break;
        case 1: //4K Banks
            io.swapCHR(4, 0, chrSelB[0x3] & mask4K, io.chrBuffer);
            io.swapCHR(4, 1, chrSelB[0x3] & mask4K, io.chrBuffer);
            break;
        case 2: //2K Banks
            io.swapCHR(2, 0, chrSelB[0x1] & mask2K, io.chrBuffer);
            io.swapCHR(2, 1, chrSelB[0x3] & mask2K, io.chrBuffer);
            io.swapCHR(2, 2, chrSelB[0x1] & mask2K, io.chrBuffer);
            io.swapCHR(2, 3, chrSelB[0x3] & mask2K, io.chrBuffer);
            break;
        case 3: //1K Banks
            io.swapCHR(1, 0, chrSelB[0x0] & mask1K, io.chrBuffer);
            io.swapCHR(1, 1, chrSelB[0x1] & mask1K, io.chrBuffer);
            io.swapCHR(1, 2, chrSelB[0x2] & mask1K, io.chrBuffer);
            io.swapCHR(1, 3, chrSelB[0x3] & mask1K, io.chrBuffer);
            io.swapCHR(1, 4, chrSelB[0x0] & mask1K, io.chrBuffer);
            io.swapCHR(1, 5, chrSelB[0x1] & mask1K, io.chrBuffer);
            io.swapCHR(1, 6, chrSelB[0x2] & mask1K, io.chrBuffer);
            io.swapCHR(1, 7, chrSelB[0x3] & mask1K, io.chrBuffer);
            break;
    }
}

void MMC5::syncNT(){

    unsigned char m = ntMapping;
    for (int i = 0; i < 4; i++){

        switch (m & 3){
            case 0:
                io.ntSpace[i] = io.ntSystemRam;
                break;
            case 1:
                io.ntSpace[i] = &io.ntSystemRam[0x400];
                break;
            case 2:
                if ((exRamMode == 0) || (exRamMode == 1)){
                    io.ntSpace[i] = exRam;
                } else {
                    io.ntSpace[i] = zeroes;
                }
                break;
            case 3:
                io.ntSpace[i] = fillMode;
                break;
        }
        m >>= 2;
    }
}

void MMC5::saveSRAM(FILE * batteryFile){
    fwrite(io.wRam, 0x10000, 1, batteryFile);
}

void MMC5::loadSRAM(FILE * batteryFile){
    sync();
    fread(io.wRam, 0x10000, 1, batteryFile);
}

void MMC5::saveState(FILE * file) {

    fwrite(&curChrSet   , sizeof(int *), 1, file);
    fwrite(&is8x16      , sizeof(int *), 1, file);
    fwrite(&ntAddr      , sizeof(int *), 1, file);
    fwrite(&slDetect    , sizeof(int *), 1, file);
    fwrite(&curTile     , sizeof(int *), 1, file);
    fwrite(&isFetchingNT, sizeof(int *), 1, file);
    fwrite(&isSplitArea , sizeof(int *), 1, file);
    fwrite(&vScroll     , sizeof(int *), 1, file);

    fwrite(&tilebank ,     sizeof(unsigned char *), 1, file);
    fwrite(&prgMode  ,     sizeof(unsigned char *), 1, file);
    fwrite(&chrMode  ,     sizeof(unsigned char *), 1, file);
    fwrite(&wRamProt1,     sizeof(unsigned char *), 1, file);
    fwrite(&wRamProt2,     sizeof(unsigned char *), 1, file);
    fwrite(&exRamMode,     sizeof(unsigned char *), 1, file);
    fwrite(&ntMapping,     sizeof(unsigned char *), 1, file);
    fwrite(&fillModeTile , sizeof(unsigned char *), 1, file);
    fwrite(&fillModeColor, sizeof(unsigned char *), 1, file);
    fwrite(&prgRamBank   , sizeof(unsigned char *), 1, file);

    for (int i = 0; i < 4; i++){
        fwrite(&prgBanks[i]   , sizeof(unsigned char *), 1, file);
        fwrite(&chrSelB[i]   , sizeof(unsigned char *), 1, file);
    }

    for (int i = 0; i < 8; i++){
        fwrite(&chrSelA[i]   , sizeof(unsigned char *), 1, file);
    }

    fwrite(&chrUpperBank , sizeof(unsigned char *), 1, file);
    fwrite(&vSplitMode   , sizeof(unsigned char *), 1, file);
    fwrite(&vSplitScroll , sizeof(unsigned char *), 1, file);
    fwrite(&vSplitBank   , sizeof(unsigned char *), 1, file);
    fwrite(&irqCounter   , sizeof(unsigned char *), 1, file);
    fwrite(&irqScanline  , sizeof(unsigned char *), 1, file);
    fwrite(&irqEnabled   , sizeof(unsigned char *), 1, file);
    fwrite(&irqStatus    , sizeof(unsigned char *), 1, file);
    fwrite(&multiplier[0], sizeof(unsigned char *), 1, file);
    fwrite(&multiplier[1], sizeof(unsigned char *), 1, file);
    fwrite(exRam         , sizeof(unsigned char), 0x400, file);
    fwrite(fillMode      , sizeof(unsigned char), 0x400, file);
    fwrite(io.wRam       , sizeof(unsigned char), 0x10000, file);
    fwrite(io.ntSystemRam, sizeof(unsigned char), 0x800, file);

    if (io.iNESHeader.chrSize8k == 0)
        fwrite(io.chrBuffer, sizeof(unsigned char), 0x2000, file);

}

void MMC5::loadState(FILE * file) {

    fread(&curChrSet   , sizeof(int *), 1, file);
    fread(&is8x16      , sizeof(int *), 1, file);
    fread(&ntAddr      , sizeof(int *), 1, file);
    fread(&slDetect    , sizeof(int *), 1, file);
    fread(&curTile     , sizeof(int *), 1, file);
    fread(&isFetchingNT, sizeof(int *), 1, file);
    fread(&isSplitArea , sizeof(int *), 1, file);
    fread(&vScroll     , sizeof(int *), 1, file);

    fread(&tilebank ,     sizeof(unsigned char *), 1, file);
    fread(&prgMode  ,     sizeof(unsigned char *), 1, file);
    fread(&chrMode  ,     sizeof(unsigned char *), 1, file);
    fread(&wRamProt1,     sizeof(unsigned char *), 1, file);
    fread(&wRamProt2,     sizeof(unsigned char *), 1, file);
    fread(&exRamMode,     sizeof(unsigned char *), 1, file);
    fread(&ntMapping,     sizeof(unsigned char *), 1, file);
    fread(&fillModeTile , sizeof(unsigned char *), 1, file);
    fread(&fillModeColor, sizeof(unsigned char *), 1, file);
    fread(&prgRamBank   , sizeof(unsigned char *), 1, file);

    for (int i = 0; i < 4; i++){
        fread(&prgBanks[i]   , sizeof(unsigned char *), 1, file);
        fread(&chrSelB[i]   , sizeof(unsigned char *), 1, file);
    }

    for (int i = 0; i < 8; i++){
        fread(&chrSelA[i]   , sizeof(unsigned char *), 1, file);
    }

    fread(&chrUpperBank , sizeof(unsigned char *), 1, file);
    fread(&vSplitMode   , sizeof(unsigned char *), 1, file);
    fread(&vSplitScroll , sizeof(unsigned char *), 1, file);
    fread(&vSplitBank   , sizeof(unsigned char *), 1, file);
    fread(&irqCounter   , sizeof(unsigned char *), 1, file);
    fread(&irqScanline  , sizeof(unsigned char *), 1, file);
    fread(&irqEnabled   , sizeof(unsigned char *), 1, file);
    fread(&irqStatus    , sizeof(unsigned char *), 1, file);
    fread(&multiplier[0], sizeof(unsigned char *), 1, file);
    fread(&multiplier[1], sizeof(unsigned char *), 1, file);
    fread(exRam         , sizeof(unsigned char), 0x400, file);
    fread(fillMode      , sizeof(unsigned char), 0x400, file);
    fread(io.wRam       , sizeof(unsigned char), 0x10000, file);
    fread(io.ntSystemRam, sizeof(unsigned char), 0x800, file);

    if (io.iNESHeader.chrSize8k == 0)
        fread(io.chrBuffer, sizeof(unsigned char), 0x2000, file);

    sync();
}
