#include "MMC5.hpp"
#include <stdlib.h>

MMC5::MMC5(CartIO & ioRef) : BasicMapper (ioRef){
    ntAddr = -1;
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
    prgBanks[0].bank = 0;
    prgBanks[1].bank = 0;
    prgBanks[2].bank = 0;
    prgBanks[3].bank = 0xFF;
    curChrSet = 0;
    io.wRam = new unsigned char [0x10000]; //64 K
    io.switch8K(0, 0, io.wRam, io.wRamSpace);
    sync();

}

unsigned char MMC5::readCPU(int address){
    unsigned char ret = BasicMapper::readCPU(address);
    switch (address){
        case 0x5204:
            {
                ret = irqStatus;
                irqStatus &= ~0x80;
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
            printf ("\nPRG Mode %X", prgMode);
            break;
        case 0x5101:
            chrMode = val & 3;
            printf ("\nCHR Mode %X", chrMode);
            break;
        case 0x5102:
            wRamProt1 = val & 3;
            break;
        case 0x5103:
            wRamProt2 = val & 3;
            break;
        case 0x5104:
            exRamMode = val & 3;
            printf ("\nExram Mode %X", exRamMode);

            if (exRamMode == 1){
                ppuReadFunc = &MMC5::readPPUExRam1;
            }
            else{
                ppuReadFunc = &MMC5::basicReadPPU;
            }
            break;
        case 0x5105:
            ntMapping = val;
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
            io.switch8K(0, prgRamBank, io.wRam, io.wRamSpace);

            break;
        case 0x5114:
            prgBanks[0].bank = val;
            prgBanks[0].romToggle = val >> 7;
            io.prgWritable = !prgBanks[0].romToggle;
            //if (io.prgWritable) printf ("\nRAM toggle 0: %X", io.prgWritable);
            break;
        case 0x5115:
            prgBanks[1].bank = val;
            prgBanks[1].romToggle = val >> 7;
            io.prgWritable = !prgBanks[1].romToggle;
            //if (io.prgWritable) printf ("\nRAM toggle 1: %X", io.prgWritable);
            break;
        case 0x5116:
            prgBanks[2].bank = val;
            prgBanks[2].romToggle = val >> 7;
            io.prgWritable = !prgBanks[2].romToggle;
            //if (io.prgWritable) printf ("\nRAM toggle 2: %X", io.prgWritable);

            break;
        case 0x5117:
            prgBanks[3].bank = val & 0x7F;
            //io.prgWritable = 0;
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
            break;
        case 0x5205:
            multiplier[0] = val;
            break;
        case 0x5206:
            multiplier[1] = val;
            break;
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

    //if (io.prgWritable){
        switch (address >> 12){
            case 8: case 9: case 10: case 11:
            case 12: case 13: case 14:case 15:
                io.prgSpace[(address >> 10) & 0x1F][address & 0x3FF] = val;
                //printf ("\nWrote RAM via 0x8XXX: %X", address);
            break;
        }
    //}

    sync();
    BasicMapper::writeCPU(address, val);

}

unsigned char MMC5::readPPU(int address){
    return (this->*ppuReadFunc)(address);
}

unsigned char MMC5::basicReadPPU(int address){
    return BasicMapper::readPPU(address);
}

unsigned char MMC5::readPPUExRam1(int address){

    if (isFetchingSpr){
        return BasicMapper::readPPU(address);
    }

    if (address >= 0x3000)
        address &= 0x2FFF;

    int mask1K = address >> 10;
    int mask4K = (io.iNESHeader.chrSize8k << 1) - 1;
    io.ppuAddrBus = address;

    switch (mask1K){

        case 0: case 1: case 2: case 3:
            io.switch4K(0, tilebank & mask4K, io.chrBuffer, io.chrSpace);
            break;
        case 4: case 5: case 6: case 7:
            io.switch4K(1, tilebank & mask4K, io.chrBuffer, io.chrSpace);
            break;
        case 8: case 9: case 10: case 11:
            if ((address & 0x3FF) < 0x3C0){
                ntAddr = address & 0x3FF;
                tilebank = exRam[ntAddr] & 0x3F;

                if (tilebank > mask4K){
                    //printf("\nHere");
                }

                return BasicMapper::readPPU(address);
            }
            else {

                if (tilebank == -1){
                    printf("\nHere");

                }
                unsigned char color = exRam[ntAddr] >> 6;
                return color | (color << 2) | (color << 4) | (color << 6);
            }
    }

    return BasicMapper::readPPU(address);
}

void MMC5::writePPU(int address, unsigned char val){
    BasicMapper::writePPU(address, val);
}

void MMC5::clockPPU(){

    irqStatus &= ~0x40;
    irqStatus |= (*io.dbg.isRendering) << 6; //In Frame
    isFetchingSpr = (*io.dbg.tick >= 256) && (*io.dbg.tick <= 319);

    if (irqEnabled && (irqStatus & 0x80)){
        io.cpuIO->irq = 1;
    }

    clockIRQ();

    syncCHR();

}

void MMC5::clockCPU(){

    int isPPUAccess = (io.cpuIO->addressBus >= 0x2000) && (io.cpuIO->addressBus <= 0x3FFF);

    if (isPPUAccess){
        int ppuReg = io.cpuIO->addressBus & 7;
        if (io.cpuIO->wr){
            switch (ppuReg){
                case 0:
                    is8x16 = ((io.cpuIO->dataBus) & 0x20);
                    break;
                case 1:
                    if (!((io.cpuIO->dataBus) & 0x18)){
                        //printf ("\nTurned off.");
                        irqStatus &= ~0x40;
                    }
                    break;
            }
        }
    }
}

void MMC5::clockIRQ(){

    if (!(irqStatus & 0x40)){
        irqCounter = 0;
    } else {
        if ((*io.dbg.tick == 337)){

            if (irqScanline == irqCounter){
                irqStatus |= 0x80;
            }
            irqCounter++;

        } else {
            irqStatus |= 0x40;
            irqStatus &= ~0x80;
        }
    }
}

void MMC5::syncPRG(){

    int mask8  = (io.iNESHeader.prgSize16k << 1) - 1;
    int mask16 = (io.iNESHeader.prgSize16k) - 1;
    int mask32 = (io.iNESHeader.prgSize16k >> 1) - 1;

    switch (prgMode){
        case 0:
            io.switch32K(0, (prgBanks[3].bank >> 2) & mask32, io.prgBuffer, io.prgSpace);
            break;
        case 1:
            if (prgBanks[1].romToggle)
                io.switch16K(0, (prgBanks[1].bank >> 1) & mask16, io.prgBuffer, io.prgSpace);
            else
                io.switch16K(0, (prgBanks[1].bank >> 1) & 7, io.wRam, io.prgSpace);

            io.switch16K(1, (prgBanks[3].bank >> 1) & mask16, io.prgBuffer, io.prgSpace);
            break;
        case 2:
            if (prgBanks[1].romToggle)
                io.switch16K(0, (prgBanks[1].bank >> 1) & mask16, io.prgBuffer, io.prgSpace);
            else
                io.switch16K(0, (prgBanks[1].bank >> 1) & 7, io.wRam, io.prgSpace);

            if (prgBanks[2].romToggle)
                io.switch8K(2, prgBanks[2].bank & mask8, io.prgBuffer, io.prgSpace);
            else
                io.switch8K(2, prgBanks[2].bank & 7, io.wRam, io.prgSpace);

            io.switch8K(3, prgBanks[3].bank & mask8, io.prgBuffer, io.prgSpace);
            break;
        case 3:
            if (prgBanks[0].romToggle)
                io.switch8K(0, prgBanks[0].bank & mask8, io.prgBuffer, io.prgSpace);
            else
                io.switch8K(0, prgBanks[0].bank & 7, io.wRam, io.prgSpace);

            if (prgBanks[1].romToggle)
                io.switch8K(1, prgBanks[1].bank & mask8, io.prgBuffer, io.prgSpace);
            else
                io.switch8K(1, prgBanks[1].bank & 7, io.wRam, io.prgSpace);

            if (prgBanks[2].romToggle)
                io.switch8K(2, prgBanks[2].bank & mask8, io.prgBuffer, io.prgSpace);
            else
                io.switch8K(2, prgBanks[2].bank & 7, io.wRam, io.prgSpace);

            io.switch8K(3, prgBanks[3].bank & mask8, io.prgBuffer, io.prgSpace);
            break;
    }
}

void MMC5::sync(){
    syncPRG();
    syncCHR();
    syncNT();
}

void MMC5::syncCHR(){

    if (exRamMode == 1 && !isFetchingSpr){
        return;
    }

    if (is8x16){

        if (!isFetchingSpr){
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

    int mask1K = (io.iNESHeader.chrSize8k << 3) - 1;
    int mask2K = (io.iNESHeader.chrSize8k << 2) - 1;
    int mask4K = (io.iNESHeader.chrSize8k << 1) - 1;
    int mask8K = io.iNESHeader.chrSize8k - 1;


    if (tilebank > mask1K){
        printf("\nHere");
    }

    switch(chrMode){
        case 0: //8K Banks
            io.switch8K(0, chrSelA[7] & mask8K, io.chrBuffer, io.chrSpace);
            break;
        case 1: //4K Banks
            io.switch4K(0, chrSelA[3] & mask4K, io.chrBuffer, io.chrSpace);
            io.switch4K(1, chrSelA[7] & mask4K, io.chrBuffer, io.chrSpace);

            break;
        case 2: //2K Banks
            io.switch2K(0, chrSelA[1] & mask2K, io.chrBuffer, io.chrSpace);
            io.switch2K(1, chrSelA[3] & mask2K, io.chrBuffer, io.chrSpace);
            io.switch2K(2, chrSelA[5] & mask2K, io.chrBuffer, io.chrSpace);
            io.switch2K(3, chrSelA[7] & mask2K, io.chrBuffer, io.chrSpace);

            break;
        case 3: //1K Banks
            io.switch1K(0, chrSelA[0] & mask1K, io.chrBuffer, io.chrSpace);
            io.switch1K(1, chrSelA[1] & mask1K, io.chrBuffer, io.chrSpace);
            io.switch1K(2, chrSelA[2] & mask1K, io.chrBuffer, io.chrSpace);
            io.switch1K(3, chrSelA[3] & mask1K, io.chrBuffer, io.chrSpace);
            io.switch1K(4, chrSelA[4] & mask1K, io.chrBuffer, io.chrSpace);
            io.switch1K(5, chrSelA[5] & mask1K, io.chrBuffer, io.chrSpace);
            io.switch1K(6, chrSelA[6] & mask1K, io.chrBuffer, io.chrSpace);
            io.switch1K(7, chrSelA[7] & mask1K, io.chrBuffer, io.chrSpace);
            break;
    }
}

void MMC5::syncCHRB(){

    switch(chrMode){
        case 0: //8K Banks
            io.switch8K(0, chrSelB[0x3], io.chrBuffer, io.chrSpace);
            break;
        case 1: //4K Banks
            io.switch4K(0, chrSelB[0x3], io.chrBuffer, io.chrSpace);
            io.switch4K(1, chrSelB[0x3], io.chrBuffer, io.chrSpace);
            break;
        case 2: //2K Banks
            io.switch2K(0, chrSelB[0x1], io.chrBuffer, io.chrSpace);
            io.switch2K(1, chrSelB[0x3], io.chrBuffer, io.chrSpace);
            io.switch2K(2, chrSelB[0x1], io.chrBuffer, io.chrSpace);
            io.switch2K(3, chrSelB[0x3], io.chrBuffer, io.chrSpace);
            break;
        case 3: //1K Banks
            io.switch1K(0, chrSelB[0x0] , io.chrBuffer, io.chrSpace);
            io.switch1K(1, chrSelB[0x1], io.chrBuffer, io.chrSpace);
            io.switch1K(2, chrSelB[0x2], io.chrBuffer, io.chrSpace);
            io.switch1K(3, chrSelB[0x3], io.chrBuffer, io.chrSpace);
            io.switch1K(4, chrSelB[0x0], io.chrBuffer, io.chrSpace);
            io.switch1K(5, chrSelB[0x1], io.chrBuffer, io.chrSpace);
            io.switch1K(6, chrSelB[0x2], io.chrBuffer, io.chrSpace);
            io.switch1K(7, chrSelB[0x3], io.chrBuffer, io.chrSpace);
            break;
    }
}

void MMC5::syncNT(){

    unsigned char m = ntMapping;
    for (int i = 0; i < 4; i++){

        switch (m & 3){
            case 0: case 1:
                io.ntSpace[i] = &io.ntSystemRam[(m & 3) * 0x400];
                break;
            case 2:
                if (exRamMode == 2){
                    io.ntSpace[i] = zeroes;
                } else {
                    io.ntSpace[i] = exRam;
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
    fread(io.wRam, 0x10000, 1, batteryFile);
}
