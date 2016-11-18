#include "Mapper071.hpp"

/* Mapper for Camerica/Codemasters games */

Mapper071::Mapper071(unsigned char * header) : GxROM(header){ }

void Mapper071::write(int addr, unsigned char val){
    switch (addr >> 12){
        /*case 0x8: case 0x9: case 0xA: case 0xB: //Mirroring
            regs[1] = (val >> 4) & 1;*/
        case 0xC: case 0xD: case 0xE: case 0xF: //Bank select
            prgBank = val;
        break;
    }
    sync();
}

void Mapper071::sync(){
    MapperUtils::switchPRG16K(prgBuffer, prg, 0, prgBank & (prgSize16K - 1));
    MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSize16K - 1);
    /*if (regs[1])
        MapperUtils::switchToSingleScrHi(ntBuffer, ppuNTSpace);
    else
        MapperUtils::switchToSingleScrLow(ntBuffer, ppuNTSpace);*/
}

bool Mapper071::loadState(FILE * file){
   Board::loadState(file);
   prgBank = tempR[0];
   sync();
}

void Mapper071::saveState(FILE * file){
    tempR[0] = prgBank;
    Board::saveState(file);
}
