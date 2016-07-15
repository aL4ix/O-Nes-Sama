#include "Mapper002.hpp"
#include <stdio.h>

Mapper002::Mapper002(unsigned char * header) : GxROM(header){}

void Mapper002::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            regs[0] = val;
            sync();
        break;
    }
}

void Mapper002::sync(){
        MapperUtils::switchPRG16K(prgBuffer, prg, 0, regs[0] & 0xF);
        MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSize16K - 1);
}

