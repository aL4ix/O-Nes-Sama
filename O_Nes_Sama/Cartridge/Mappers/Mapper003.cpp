#include "Mapper003.hpp"

Mapper003::Mapper003(unsigned char * header) : GxROM(header){}

void Mapper003::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            regs[0] = val;
            sync();
        break;
    }
}

void Mapper003::sync(){
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, regs[0] & (chrSize8K - 1));
}
