#include "Mapper011.hpp"

Mapper011::Mapper011(unsigned char * header) : GxROM(header){ }

void Mapper011::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            regs[0] = val;
            sync();
        break;
    }
}

void Mapper011::sync(){
    MapperUtils::switchPRG32K(prgBuffer, prg, regs[0] & 0x3);
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, regs[0] >> 4);
}
