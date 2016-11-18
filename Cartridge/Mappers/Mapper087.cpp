#include "Mapper087.hpp"

/* Mapper 0087 -

Ninja Jajamaru Kun (J)
City Connection (J)
etc...
*/

Mapper087::Mapper087(unsigned char * header) : GxROM(header){}

void Mapper087::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x6: case 0x7:
            chrBank = val;
            sync();
        break;
    }
}

void Mapper087::sync(){
    unsigned char bank = (((chrBank & 1) << 1) | ((chrBank & 2) >> 1)) & (chrSize8K - 1);
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, bank);
}

bool Mapper087::loadState(FILE * file){
   Board::loadState(file);
   chrBank = tempR[0];
   sync();
}

void Mapper087::saveState(FILE * file){
    tempR[0] = chrBank;
    Board::saveState(file);
}

