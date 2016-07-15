#include "Mapper001.hpp"

Mapper001::Mapper001(unsigned char * header) : MMC1(header){}

void Mapper001::write (int addr, unsigned char val){
     data = val;
     MMC1::addr = addr;

     switch (addr >> 12){
        case 0x6: case 0x7: /* WRAM Space */
            wramBuffer[addr & 0x1FFF] = val;
            break;
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            /* Ignore consecutive CPU cycle writes to load register */
            if ((countM2 - lastCountM2) > 1){
                loadRegWrite();
            }
            lastCountM2 = countM2;
            break;
     }
}

void MMC1::sync(){
    /* Sync Mirroring */
    setNTMirroring();
    /* Sync PRG state... jeez what a PITA */
    switch((regs[0] & 0xC) >> 2){
        case 0: case 1:
            /* No game i know switches between modes*/
            MapperUtils::switchPRG32K(prgBuffer, prg, (regs[3] & prgSizeMask) >> 1);
            break;
        case 2: //Fix first bank at $8000
            MapperUtils::switchPRG16K(prgBuffer, prg, 0, 0);
            MapperUtils::switchPRG16K(prgBuffer, prg, 1, regs[3] & prgSizeMask);
            break;
        case 3: //Fix last bank at $C000
            MapperUtils::switchPRG16K(prgBuffer, prg, 0, regs[3] & prgSizeMask);
            MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSize16K - 1);
            break;
    }

    /* Sync CHR state... */
    switch((regs[0] & 0x10) >> 4){
        case 0:
            chrBank0Write();
            break;
        case 1:
            chrBank0Write();
            chrBank1Write();
            break;
    }
}

void Mapper001::chrBank0Write(){
    if (!(regs[0] & 0x10)){
        /*Switch the 8K bank (val) into the $0000-$1FFF PPU space...*/
        MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, (regs[1] >> 1) & ((chrSizeMask >> 1) | 1));
    }
    else{
        /*Switch the 4K bank (val) into the $0000-$0FFF PPU space...*/
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 0, regs[1]);
    }
}

void Mapper001::chrBank1Write(){
    if (regs[0] & 0x10){
        /*Switch the 4K bank (val) into the $1000-$1FFF PPU space...*/
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 1, regs[2]);
    }
}

void Mapper001::prgBankWrite(){
    switch ((regs[0] & 0xC) >> 2){ //PRG Mode
        case 0: case 1:
            /*Switch the 32K bank (val) into the $8000 - $FFFF CPU space...*/
            MapperUtils::switchPRG32K(prgBuffer, prg, (regs[3] & prgSizeMask) >> 1);
            break;
        case 2:
            /*Switch the 16K bank (val) into the $C000 - $FFFF CPU space...*/
            MapperUtils::switchPRG16K(prgBuffer, prg, 1, regs[3] & prgSizeMask);
            break;
        case 3:
            /*Switch the 16K bank (val) into the $8000 - $BFFF CPU space...*/
            MapperUtils::switchPRG16K(prgBuffer, prg, 0, regs[3] & prgSizeMask);
            break;
    }
}
