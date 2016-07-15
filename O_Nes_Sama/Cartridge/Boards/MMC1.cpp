#include "MMC1.hpp"

MMC1::MMC1(unsigned char * header) : Board(header){}

void MMC1::init(){
    Board::init();
    ntBuffer = new unsigned char [MapperUtils::_2K];
    wramBuffer = new unsigned char [MapperUtils::_32K];
    regs[4] = 0b10000; //Shift register assigned to reg 4 for convenience
    regs[0] = 0xC;
    regs[1] = 0;
    regs[2] = 0;
    regs[3] = 0;
    prgSizeMask = prgSize16K - 1;
    if (chrSize8K > 0)
        chrSizeMask = (chrSize8K << 1) - 1;
    else
        chrSizeMask = 1;
    prg = &cpuCartSpace[4];
    sync();
}

unsigned char MMC1::read(int addr){
    switch (addr >> 12){
        case 0x6: case 0x7: /* WRAM Space */
            return wramBuffer[addr & 0x1FFF];
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            return prg[(addr & 0x7FFF) >> 12][addr & 0xFFF];
    }
    return -1;
}

void MMC1::pushInShiftReg(){
    int bit = data & 1;
    regs[4] = ((regs[4] & 0x1F) >> 1);
    regs[4] = regs[4] | (bit << 4);
}

void MMC1::loadRegWrite(){
    if (data & 0x80){ //Reset MMC1
        regs[4] = 0x10;
        regs[0] |= 0xC;
        sync();
    }
    else{
        if (regs[4] & 1){
            int regNo = (addr & 0x6000) >> 13;
            int regval = (regs[4] >> 1) | ((data & 1) << 4);
            switch (regNo){
                case 0:
                    regs[0] = regval & 0x1F;
                    sync();
                    break;
                case 1:
                    regs[1] = regval & chrSizeMask;
                    chrBank0Write();
                    break;
                case 2:
                    regs[2] = regval & chrSizeMask;
                    chrBank1Write();
                    break;
                case 3:
                    regs[3] = regval & 0x1F;
                    prgBankWrite();
                    break;
            }
            regs[4] = 0x10; /*Reset the Shift Register*/
        }
        else{
            pushInShiftReg();
        }
    }
}

void MMC1::setNTMirroring(){
    switch (regs[0] & 0x3){
        case 0: //One Screen, lower bank
            MapperUtils::switchToSingleScrLow(ntBuffer, ppuNTSpace);
            return;
        case 1: //One Screen, upper bank
            MapperUtils::switchToSingleScrHi(ntBuffer, ppuNTSpace);
            return;
        case 2: //Vertical
            MapperUtils::switchToVertMirroring(ntBuffer, ppuNTSpace);
            return;
        case 3: //Horizontal
            MapperUtils::switchToHorMirroring(ntBuffer, ppuNTSpace);
            return;
    }
}

void MMC1::clockCPU(){
    countM2++;
}

void MMC1::saveSRAM(FILE * batteryFile) {
    MapperUtils::saveSRAM(wramBuffer, batteryFile);
}

void MMC1::loadSRAM(FILE * batteryFile) {
    MapperUtils::loadSRAM(wramBuffer, batteryFile);
}
