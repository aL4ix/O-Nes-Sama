#include "Mapper185.hpp"

Mapper185::Mapper185(unsigned char * header) : GxROM(header){}

void Mapper185::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            regs[0] = val;
            printf ("\nMapper185: Wrote $%x in %x", val, addr);
            sync();
        break;
    }
}

void Mapper185::sync(){
    unsigned char copyProt = (regs[0] & 0x33);
    if (((copyProt & 0xF) == 0) || (copyProt == 0x13)){
        for (int x = 0; x < 8; x++)
            MapperUtils::switchCHR1K(badCHR,ppuChrSpace,x,0);
    } else {
        MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, regs[0] & (chrSize8K - 1));
	}
}
