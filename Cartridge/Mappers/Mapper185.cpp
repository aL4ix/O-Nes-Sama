#include "Mapper185.hpp"

/* This is like Mapper 003 (CNROM) but with some weak copy protection mechanism. */

Mapper185::Mapper185(unsigned char * header) : GxROM(header){}

void Mapper185::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            chrBank = val;
            sync();
        break;
    }
}

void Mapper185::sync(){
    unsigned char copyProt = (chrBank & 0x33);
    if (((copyProt & 0xF) == 0) || (copyProt == 0x13)){
        for (int x = 0; x < 8; x++)
            MapperUtils::switchCHR1K(badCHR,ppuChrSpace,x,0);
    } else {
        MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, chrBank & (chrSize8K - 1));
	}
}


bool Mapper185::loadState(FILE * file){
   Board::loadState(file);
   chrBank = tempR[0];
   sync();
}

void Mapper185::saveState(FILE * file){
    tempR[0] = chrBank;
    Board::saveState(file);
}
