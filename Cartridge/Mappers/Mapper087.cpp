#include "Mapper087.hpp"

Mapper087::Mapper087(unsigned char * header) : GxROM(header){}

void Mapper087::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x6: case 0x7:
            regs[0] = val;
            sync();
        break;
    }
}

void Mapper087::sync(){
    int bank = (((regs[0] & 1) << 1) | ((regs[0] & 2) >> 1)) & (chrSize8K - 1);
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, bank);
}
