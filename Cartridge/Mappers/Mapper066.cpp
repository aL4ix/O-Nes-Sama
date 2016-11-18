#include "Mapper066.hpp"

Mapper066::Mapper066(unsigned char * header) : GxROM(header){}

void Mapper066::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            banksReg = val;
            sync();
        break;
    }
}

void Mapper066::sync(){
    MapperUtils::switchPRG32K(prgBuffer, prg, (banksReg & 0x30) >> 4);
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, banksReg & 0x3);
}

bool Mapper066::loadState(FILE * file){
   Board::loadState(file);
   banksReg = tempR[0];
   sync();
}

void Mapper066::saveState(FILE * file){
    tempR[0] = banksReg;
    Board::saveState(file);
}
