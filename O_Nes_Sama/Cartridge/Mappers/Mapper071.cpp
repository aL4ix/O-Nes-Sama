#include "Mapper071.hpp"

Mapper071::Mapper071(unsigned char * header) : GxROM(header){ }

void Mapper071::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x8: case 0x9: case 0xA: case 0xB: //Mirroring
            regs[1] = (val >> 4) & 1;
        case 0xC: case 0xD: case 0xE: case 0xF: //Bank select
            regs[0] = val;
        break;
    }
    sync();
}

void Mapper071::sync(){
    MapperUtils::switchPRG16K(prgBuffer, prg, 0, regs[0] & (prgSize16K - 1));
    MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSize16K - 1);
    if (regs[1])
        MapperUtils::switchToSingleScrHi(ntBuffer, ppuNTSpace);
    else
        MapperUtils::switchToSingleScrLow(ntBuffer, ppuNTSpace);
}
