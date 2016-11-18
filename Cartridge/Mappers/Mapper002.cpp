#include "Mapper002.hpp"
#include <stdio.h>

Mapper002::Mapper002(unsigned char * header) : GxROM(header){
    //tempR[0] = &prgBank;
}

void Mapper002::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            prgBank = val;
            sync();
        break;
    }
}

void Mapper002::sync(){
        MapperUtils::switchPRG16K(prgBuffer, prg, 0, prgBank & 0xF);
        MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSize16K - 1);
}

bool Mapper002::loadState(FILE * file){
   Board::loadState(file);
   prgBank = tempR[0];
   sync();
}

void Mapper002::saveState(FILE * file){
    tempR[0] = prgBank;
    Board::saveState(file);
}


