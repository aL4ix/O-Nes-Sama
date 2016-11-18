/*
64 in 1, and other multi-carts
*/

#include "Mapper225.hpp"

Mapper225::Mapper225(unsigned char * header) : GxROM(header){}

void Mapper225::init(){
    GxROM::init();
    MapperUtils::switchPRG32K(prgBuffer, prg, 0);
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, 0);
}

void Mapper225::write(int addr, unsigned char val){

    switch (addr >> 12){
        case 0x5:
            //4 x 4 bit ram register
            ramReg[addr & 0x3] = val & 0xF;
            break;
        case 0x6: case 0x7:
            //printf("Wrote in WRAM area.... bad");
            wramBuffer[addr & 0x1FFF] = val;
            break;
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            {
                mirr    = (addr >> 13) & 1;
                prgMode = (addr >> 12) & 1;
                prgPos  = (addr >> 6) & 1;
                prgReg  = ((addr >> 7) & 0x1F);
                chrReg  = (addr & 0x3F);
                sync();
            }
        break;
    }
}

unsigned char Mapper225::read(int addr){
    unsigned char ret = GxROM::read(addr);
     switch (addr >> 12){
        case 0x5:
            //4 x 4 bit ram register
            ret = ramReg[addr & 0x3] & 0xF;
            break;
        case 0x6: case 0x7:
            ret = wramBuffer[addr & 0x1FFF];
            break;
     }
    return ret;
}

void Mapper225::sync(){
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, chrReg);
    if (!mirr)
        MapperUtils::switchToVertMirroring(ntBuffer, ppuNTSpace);
    else
        MapperUtils::switchToHorMirroring(ntBuffer, ppuNTSpace);

    if (!prgMode){
        MapperUtils::switchPRG32K(prgBuffer, prg, prgReg);
    } else {
        MapperUtils::switchPRG16K(prgBuffer, prg, 0, (prgReg << 1) | prgPos);
        MapperUtils::switchPRG16K(prgBuffer, prg, 1, (prgReg << 1) | prgPos);
    }
}


bool Mapper225::loadState(FILE * file){
    Board::loadState(file);

    for (int i=0; i < 4; i++){
        ramReg[i]  = regs[i];
    }

    mirr     = regs[4];
    prgMode  = regs[5];
    prgPos   = regs[6];
    prgReg   = regs[7];
    chrReg   = regs[8];
    sync();
}

void Mapper225::saveState(FILE * file){

    for (int i=0; i < 4; i++){
        regs[i] = ramReg[i];
    }

    regs[4] = mirr;
    regs[5] = prgMode;
    regs[6] = prgPos;
    regs[7] = prgReg;
    regs[8] = chrReg;

    Board::saveState(file);
}

