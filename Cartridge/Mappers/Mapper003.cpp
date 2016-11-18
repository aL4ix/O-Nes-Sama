#include "Mapper003.hpp"

Mapper003::Mapper003(unsigned char * header) : GxROM(header){}

void Mapper003::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            chrBank = val;
            sync();
        break;
    }
}

void Mapper003::sync(){
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, chrBank & (chrSize8K - 1));
}

bool Mapper003::loadState(FILE * file){
   Board::loadState(file);
   chrBank = tempR[0];
   sync();
}

void Mapper003::saveState(FILE * file){
    tempR[0] = chrBank;
    Board::saveState(file);
}
