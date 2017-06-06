#include "MMC5.hpp"
#include <stdlib.h>

int chrRegSet8x8 = 0;
int lastChrRegSet = 0;
int fetchingSpr = 0;
int fetchingBG = 0;
int wRamPage = 0;
int wRamBank = 0;
int chrSelWrap = 0;
int chrSelMask = 0;
int is8x16 = 0;

int ppuReg2000 = 0;
int ppuReg2001 = 0;

int ntFetchesCtr = 0;
int lastPPUAddr = 0;
int extendedBank = 0;

MMC5::MMC5(CartIO & ioRef) : BasicMapper (ioRef){
    IRQCounter = 0;
    io.wRam = new unsigned char [0x10000]; //64 K
    io.switch32K(0, 0, io.wRam, io.wRamSpace);
    io.switch32K(1, 1, io.wRam, io.wRamSpace);

    /*prg = &cpuCartSpace[4];
    wRAMOuter[0] = &wramBuffer[0];
    wRAMOuter[1] = &wramBuffer[0x8000];*/

    prgMode = 0;
    chrMode = 0;


    //Fix last bank to the 0x8000 - 0xFFFF area
    sync();
}

void MMC5::clockCPU(){
    //Watch Accesses to PPU registers
    int isPPUAccess = (io.cpuIO->addressBus >= 0x2000) && (io.cpuIO->addressBus <= 0x3FFF);


    if (isPPUAccess){
        int ppuReg = io.cpuIO->addressBus & 7;
        switch (ppuReg){
            case 0:
                ppuReg2000 = io.cpuIO->dataBus;
                is8x16 = (ppuReg2000 & 0x20);
                break;
            case 1:
                ppuReg2001 = io.cpuIO->dataBus;
                if (!(ppuReg2001 & 0x18))
                    IRQStatusRd &= ~0x40;
                break;
        }
    }

}

void MMC5::clockIRQCounter(){

    if (IRQEnabled && (IRQStatusRd & 0x80)){
        io.cpuIO->irq = 1;
    }

    IRQStatusRd &= ~0x40;
    IRQStatusRd |= (*io.dbg.isRendering) << 6; //In Frame

    if (!(IRQStatusRd & 0x40)){
        IRQCounter = 0;
    } else {
        if (ntFetchesCtr == 1){

            if (IRQScanLine == IRQCounter){
                IRQStatusRd |= 0x80;
            }
            IRQCounter++;

        } else {
            IRQStatusRd |= 0x40;
            IRQStatusRd &= ~0x80;
        }

    }

}

void MMC5::clockPPU(){

}

unsigned char MMC5::readPPU(int address){

    if (address >= 0x3000)
        address &= 0x2FFF;

    io.ppuAddrBus = address;

    int mask1K = address >> 10;
    int ntAddr = 0;
    int chrAddr = 0;

    fetchingSpr = (IRQStatusRd & 0x40) && (*io.dbg.tick >= 257) && (*io.dbg.tick <= 320);

    if (!fetchingSpr){
        if (exRamMode == 1){

            switch (mask1K){
                case 8: case 9: case 10: case 11:
                    if ((address & 0x3FF) < 0x3C0){ //Fetch NT Byte
                        ntAddr = address & 0x3FF;
                        extendedBank = (expansionRam[ntAddr] & 0x3F) | (chrUpperBank << 6);
                        io.switch4K(0, extendedBank, io.chrBuffer, io.chrSpace);
                        io.switch4K(1, extendedBank, io.chrBuffer, io.chrSpace);
                    }
                    else{ //Fetching AT byte
                        unsigned char color = expansionRam[ntAddr] >> 6;

                        color = color | (color << 2) | (color << 4) | (color << 6) ;
                        //if (color) printf ("Color: %X", color);
                        return color;

                    }

            }
        } else {
            if (is8x16){
                switchCHRSetB();
            } else {
                if (chrRegSet8x8 < 8){
                    switchCHRSetA();
                } else {
                    switchCHRSetB();
                }
            }
        }
    } else {
        if (is8x16){
            switchCHRSetA();
        } else {
            if (chrRegSet8x8 < 8){
                switchCHRSetA();
            } else {
                switchCHRSetB();
            }
        }
    }

    //Detects the last NT Dummy fetches for clocking scanline counter.
    if ((IRQStatusRd & 0x40) && ((io.ppuAddrBus & 0x2FFF) < 0x23C0) && (io.ppuAddrBus == lastPPUAddr)){
        ntFetchesCtr++;
    } else {
        ntFetchesCtr = 0;
    }
    clockIRQCounter();
    lastPPUAddr = io.ppuAddrBus;

    return BasicMapper::readPPU(address);
}

void MMC5::writePPU(int address, unsigned char val){

    clockIRQCounter();
    BasicMapper::writePPU(address, val);

}

unsigned char MMC5::readCPU(int address){
    unsigned char ret = BasicMapper::readCPU(address);
    //printf ("\n%X", address);
    switch (address){
        //Return value from readable regs.
        case 0x5204:
            ret = IRQStatusRd;
            IRQStatusRd &= ~0x80; //Clear IRQ Pending
            io.cpuIO->irq = 0;
            break;
        case 0x5205:
            ret = (mltprFactors[0] * mltprFactors[1]);
            break;
        case 0x5206:
            ret = (mltprFactors[0] * mltprFactors[1]) >> 8;
            break;
    }
        //Return value from expansion RAM.
        if (address >= 0x5C00 && address <= 0x5FFF){
            if ((exRamMode == 0) || (exRamMode == 1)){
                ret = io.cpuIO->dataBus;
            }
            else {
                //printf("\nRead ADDR: %x", addr);
                //printf ("\nexRAM Mode: %d", exRamMode);
                ret = expansionRam[address & 0x3FF];
            }
    }
    return ret;
}

void MMC5::writeCPU(int address, unsigned char val){
    BasicMapper::writeCPU(address, val);

    //Watch writes to PPU regs

    switch (address){
        case 0x5100: //PRG Mode
            prgMode = val & 3;
            printf ("\nPRG MODE %d", prgMode);
            break;
        case 0x5101: //CHR Mode
            chrMode = val & 3;
            break;
        case 0x5102: //wRAM protect 1
            wRamProtect[0] = val & 3;
            break;
        case 0x5103: //wRAM protect 2
            wRamProtect[1] = val & 3;
            break;
        case 0x5104: //EX RAM Mode
            exRamMode = val & 3;
            //if (exRamMode == 2 )
            printf ("\nexRam %d", exRamMode);
            break;
        case 0x5105: //Nametable mapping
            nameTableMapping = val;
            //printf ("\nNT Mapping: %x, %x", nameTableMapping, exRamMode);
            break;
        case 0x5106: //Fill Mode Tile
            //printf ("\nFill Mode %d", fillModeTile);
            fillModeTile = val;
            for (int i = 0; i < 0x3C0; i++){
                fillMode[i] = fillModeTile;
            }

            break;
        case 0x5107: //Fill Mode Color
            fillModeColor = val & 3;
            for (int i = 0x3C0; i < 0x400; i++){
                fillMode[i] = fillModeColor | (fillModeColor << 2) | (fillModeColor << 4) | (fillModeColor << 6);
                //printf ("\nFill color!!! %d", fillModeColor);
                //fillMode[i] = (fillModeColor << 6);
            }
            break;
        case 0x5113: //wRAM RAM bank mode
            wRamBank = val & 7;
            //wRamPage = val >> 2;
            io.switch8K(wRamBank >> 2, wRamBank & 3, io.wRam, io.wRamSpace);
            printf("\nWRAM Offset: %x, %x", wRamPage, wRamBank);
            break;
        case 0x5114: //PRG Bank 0
            prgBanks[0] = val;
            break;
        case 0x5115: //PRG Bank 1
            prgBanks[1] = val;
            break;
        case 0x5116: //PRG Bank 2
            prgBanks[2] = val;
            break;
        case 0x5117: //PRG Bank 3
            prgBanks[3] = val & 0x7F;
            break;

        case 0x5120: case 0x5121: case 0x5122: case 0x5123:
        case 0x5124: case 0x5125: case 0x5126: case 0x5127:
        case 0x5128: case 0x5129: case 0x512A: case 0x512B:
            chrSelects[address & 0xF] = val;
            chrRegSet8x8 = address & 0xF;
            break;
        case 0x5130: //Upper CHR Bank bits
            chrUpperBank = val & 3;
            break;
        case 0x5200: //Vertical Split Mode
            vertSplitMode = val;
            break;
        case 0x5201: //Vertical Split Scroll
            vertSplitScroll = val;
            break;
        case 0x5202: //Vertical Split Bank
            vertSplitBank = val;
            break;
        case 0x5203: //IRQ Counter Scanline
            IRQScanLine = val;
            //printf ("\nIRQ Scan Line %d", IRQScanLine);
            break;
        case 0x5204: //IRQ Status
            //printf("\nWrite 0x5204: %x", val);
            IRQEnabled = val;
            //printf ("\nIRQ Status %x", IRQStatus[1]);
            break;
        case 0x5205: //Multiplicand
            mltprFactors[0] = val;
            //printf ("\nMul1 %d", mltprFactors[0]);
            break;
        case 0x5206: //Multiplier
            mltprFactors[1] = val;
            break;
    }

    //Write to expansion RAM
    if (address >= 0x5C00 && address <= 0x5FFF){

        if ((exRamMode == 0) || (exRamMode == 1)){
            if (*io.dbg.isRendering){
                //printf ("\nWrote %X %d", address & 0x3FF, *io.dbg.sl);
                expansionRam[address & 0x3FF] = val;
            } else {
                expansionRam[address & 0x3FF] = 0;
            }
        }
        if (exRamMode == 2){
            expansionRam[address & 0x3FF] = val;
        }
    }

    //printf ("\nPRG MODE: %X", prgMode);
    /*printf("\nIs 8x16 %d", ppuStatus.isSprites8x16);

    printf ("\nCHR MODE: %X", chrMode);
    printf ("\nMapping MODE: %X", nameTableMapping);
    printf ("\nexRamMode: %x", exRamMode);
            //Return value from wRAM*/
    sync();

}


void MMC5::sync(){

//    syncSprCHR();
//    syncBGCHR();

    syncPRG();
    syncNT();
}

void MMC5::syncPRG(){
    //Sync PRG ROM or PRG RAM
    //printf ("\nPRG MODE: %X", prgMode);
    int ramToggle = 0;
    int mask8  = (io.iNESHeader.prgSize16k << 1) - 1;
    int mask16 = (io.iNESHeader.prgSize16k) - 1;
    int mask32 = (io.iNESHeader.prgSize16k >> 1) - 1;
    prgBanks[3] = mask8;

    switch (prgMode){
        case 0:
            io.switch32K(0, (prgBanks[3] >> 2) & mask32, io.prgBuffer, io.prgSpace);
            break;
        case 1:
            io.switch16K(0, (prgBanks[1] >> 1) & mask16, io.prgBuffer, io.prgSpace);
            io.switch16K(1, (prgBanks[3] >> 1) & mask16, io.prgBuffer, io.prgSpace);
            break;
        case 2:
            io.switch16K(0, (prgBanks[1] >> 1) & mask16, io.prgBuffer, io.prgSpace);
            io.switch8K(2, prgBanks[2] & mask8, io.prgBuffer, io.prgSpace);
            io.switch8K(3, prgBanks[3] & mask8, io.prgBuffer, io.prgSpace);
            break;
        case 3:
            if (ramToggle) printf ("\nRAM!!");
            io.switch8K(0, prgBanks[0] & mask8, io.prgBuffer, io.prgSpace);
            io.switch8K(1, prgBanks[1] & mask8, io.prgBuffer, io.prgSpace);
            io.switch8K(2, prgBanks[2] & mask8, io.prgBuffer, io.prgSpace);
            io.switch8K(3, prgBanks[3] & mask8, io.prgBuffer, io.prgSpace);
            break;
    }

}

unsigned char * MMC5::setNTSource(int ntMode){
    switch (ntMode){
        case 0:
            return io.ntSystemRam;
        case 1:
            return &io.ntSystemRam[0x400];
        case 2:
            if (exRamMode == 0 || exRamMode == 1){
                return expansionRam;
            } else {
                return zeroes;
            }
            break;
        case 3:
            return fillMode;
    }
    return NULL;
}

void MMC5::syncNT(){

    io.ntSpace[0] = setNTSource(nameTableMapping & 3);
    io.ntSpace[1] = setNTSource((nameTableMapping >> 2) & 3);
    io.ntSpace[2] = setNTSource((nameTableMapping >> 4) & 3);
    io.ntSpace[3] = setNTSource((nameTableMapping >> 6) & 3);

}


void MMC5::switchCHRSetA(){

    //int upperBank = (chrUpperBank & 3) << 6;

    switch(chrMode){
        case 0: //8K Banks
            io.switch8K(0, chrSelects[7], io.chrBuffer, io.chrSpace);
            break;
        case 1: //4K Banks
            io.switch4K(0, chrSelects[3], io.chrBuffer, io.chrSpace);
            io.switch4K(1, chrSelects[7], io.chrBuffer, io.chrSpace);

            break;
        case 2: //2K Banks
            io.switch2K(0, chrSelects[1], io.chrBuffer, io.chrSpace);
            io.switch2K(1, chrSelects[3], io.chrBuffer, io.chrSpace);
            io.switch2K(2, chrSelects[5], io.chrBuffer, io.chrSpace);
            io.switch2K(3, chrSelects[7], io.chrBuffer, io.chrSpace);

            break;
        case 3: //1K Banks
            io.switch1K(0, chrSelects[0], io.chrBuffer, io.chrSpace);
            io.switch1K(1, chrSelects[1], io.chrBuffer, io.chrSpace);
            io.switch1K(2, chrSelects[2], io.chrBuffer, io.chrSpace);
            io.switch1K(3, chrSelects[3], io.chrBuffer, io.chrSpace);
            io.switch1K(4, chrSelects[4], io.chrBuffer, io.chrSpace);
            io.switch1K(5, chrSelects[5], io.chrBuffer, io.chrSpace);
            io.switch1K(6, chrSelects[6], io.chrBuffer, io.chrSpace);
            io.switch1K(7, chrSelects[7], io.chrBuffer, io.chrSpace);
            break;
    }

}


void MMC5::switchCHRSetB(){
    //int upperBank = (chrUpperBank & 3) << 6;

    switch(chrMode){
        case 0: //8K Banks
            io.switch8K(0, chrSelects[0xB], io.chrBuffer, io.chrSpace);
            break;
        case 1: //4K Banks
            io.switch4K(0, chrSelects[0xB], io.chrBuffer, io.chrSpace);
            io.switch4K(1, chrSelects[0xB], io.chrBuffer, io.chrSpace);
            break;
        case 2: //2K Banks
            io.switch2K(0, chrSelects[0x9], io.chrBuffer, io.chrSpace);
            io.switch2K(1, chrSelects[0xB], io.chrBuffer, io.chrSpace);
            io.switch2K(2, chrSelects[0x9], io.chrBuffer, io.chrSpace);
            io.switch2K(3, chrSelects[0xB], io.chrBuffer, io.chrSpace);
            break;
        case 3: //1K Banks
            io.switch1K(0, chrSelects[0x8], io.chrBuffer, io.chrSpace);
            io.switch1K(1, chrSelects[0x9], io.chrBuffer, io.chrSpace);
            io.switch1K(2, chrSelects[0xA], io.chrBuffer, io.chrSpace);
            io.switch1K(3, chrSelects[0xB], io.chrBuffer, io.chrSpace);

            io.switch1K(4, chrSelects[0x8], io.chrBuffer, io.chrSpace);
            io.switch1K(5, chrSelects[0x9], io.chrBuffer, io.chrSpace);
            io.switch1K(6, chrSelects[0xA], io.chrBuffer, io.chrSpace);
            io.switch1K(7, chrSelects[0xB], io.chrBuffer, io.chrSpace);
            break;
    }
}


void MMC5::genericPPUEvent(){}


/*void MMC5::saveSRAM(FILE * file){}
void MMC5::loadSRAM(FILE * file){}

bool MMC5::loadState(FILE * file){

    int ret = Board::loadState(file);
    int size = 0;

    prgMode          = tempR[0];
    chrMode          = tempR[1];
    wRamProtect[0]   = tempR[3];
    wRamProtect[1]   = tempR[4];
    exRamMode        = tempR[5];
    nameTableMapping = tempR[6];
    fillModeTile     = tempR[7];
    fillModeColor    = tempR[8];
    wRAMBankMode     = tempR[9];
    prgBanks[0]      = tempR[10];
    prgBanks[1]      = tempR[11];
    prgBanks[2]      = tempR[12];
    prgBanks[3]      = tempR[13];
    for (int i=0; i < 12; i++)
        chrSelects[i] = tempR[i + 14];
    chrUpperBank     = tempR[26];
    vertSplitMode    = tempR[27];
    vertSplitScroll  = tempR[28];
    vertSplitBank    = tempR[29];
    IRQScanLine      = tempR[30];
    IRQStatus[0]     = tempR[31];
    IRQStatus[1]     = tempR[32];
    mltprFactors[0]  = tempR[33];
    mltprFactors[1]  = tempR[34];
    //chrSelectRegion  = tempR[35];
    fetchingSpr      = tempR[36];
    IRQCounter       = tempR[37];

    size += fread(expansionRam, MapperUtils::_1K, 1, file);
    size += fread(fillMode, MapperUtils::_1K, 1, file);

    sync();
    return ret;

}

void MMC5::saveState(FILE * file){

    tempR[0] = prgMode;
    tempR[1] = chrMode;
    tempR[3] = wRamProtect[0];
    tempR[4] = wRamProtect[1];
    tempR[5] = exRamMode;
    tempR[6] = nameTableMapping;
    tempR[7] = fillModeTile;
    tempR[8] = fillModeColor;
    tempR[9] = wRAMBankMode;
    tempR[10] = prgBanks[0];
    tempR[11] = prgBanks[1];
    tempR[12] = prgBanks[2];
    tempR[13] = prgBanks[3];
    for (int i=0; i < 12; i++)
        tempR[i + 14] = chrSelects[i];
    tempR[26] = chrUpperBank;
    tempR[27] = vertSplitMode;
    tempR[28] = vertSplitScroll;
    tempR[29] = vertSplitBank;
    tempR[30] = IRQScanLine;
    tempR[31] = IRQStatus[0];
    tempR[32] = IRQStatus[1];
    tempR[33] = mltprFactors[0];
    tempR[34] = mltprFactors[1];
    //tempR[35] = chrSelectRegion;
    tempR[36] = fetchingSpr;
    tempR[37] = IRQCounter;

    Board::saveState(file);
    fwrite(expansionRam, MapperUtils::_1K, 1, file);
    fwrite(fillMode, MapperUtils::_1K, 1, file);

}*/

MMC5::~MMC5(){}

