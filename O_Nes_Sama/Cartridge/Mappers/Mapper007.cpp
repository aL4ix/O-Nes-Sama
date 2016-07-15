#include "Mapper007.hpp"

Mapper007::Mapper007(unsigned char * header) : GxROM(header){}

void Mapper007::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            regs[0] = val;
            sync();
        break;
    }
}

void Mapper007::sync(){
    MapperUtils::switchPRG32K(prgBuffer, prg, regs[0] & 0x7);
    if ((regs[0] >> 4) & 0x1)
        MapperUtils::switchToSingleScrHi(ntBuffer, ppuNTSpace);
    else
        MapperUtils::switchToSingleScrLow(ntBuffer, ppuNTSpace);
}


