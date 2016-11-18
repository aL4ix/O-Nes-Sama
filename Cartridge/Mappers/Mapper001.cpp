#include "Mapper001.hpp"



Mapper001::Mapper001(unsigned char * header) : MMC1(header){}

void MMC1::sync(){
    /* Sync Mirroring */
    setNTMirroring();
    /* Sync PRG state... jeez what a PITA */
    switch((control & 0xC) >> 2){
        case 0: case 1:
            // No game I know switches between mode
            MapperUtils::switchPRG32K(prgBuffer, prg, (prgBank & prgSizeMask) >> 1);
            break;
        case 2: //Fix first bank at $8000
            MapperUtils::switchPRG16K(prgBuffer, prg, 0, 0);
            MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgBank & prgSizeMask);
            break;
        case 3: //Fix last bank at $C000
            MapperUtils::switchPRG16K(prgBuffer, prg, 0, prgBank & prgSizeMask);
            MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSize16K - 1);
            break;
    }

    /* Sync CHR state... */
    if(control & 0x10){
        /*Switch the 4K bank (val) into the $1000-$1FFF PPU space...*/
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 0, chrBanks[0] & chrSizeMask);
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 1, chrBanks[1] & chrSizeMask);
    } else {
        MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, (chrBanks[0] >> 1) & ((chrSizeMask >> 1) | 1));
    }
}



