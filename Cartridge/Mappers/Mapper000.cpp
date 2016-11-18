#include "Mapper000.hpp"

Mapper000::Mapper000(unsigned char * header) : GxROM(header){ }

void Mapper000::write(int addr, unsigned char val){
    /* Write to WRAM */
    switch (addr >> 12){
        case 0x6: case 0x7:
            wramBuffer[addr & 0x1FFF] = val;
        break;
    }
}

bool Mapper000::loadState(FILE * file){ Board::loadState(file); }
void Mapper000::saveState(FILE * file){ Board::saveState(file); }
