#include "Mapper010.hpp"

Mapper010::Mapper010(unsigned char * header) : MMC2(header){}

void Mapper010::init(){
    MMC2::init();
    prgSizeMask = prgSize16K - 1;
    /*Fix PRG buffer's last 16KB to the last bank of the CPU PRG space*/
    MapperUtils::switchPRG16K(prgBuffer, prg, 0, prgSizeMask - 1);
    MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSizeMask);
}

void Mapper010::syncPRG(){
   MapperUtils::switchPRG16K(prgBuffer, prg, 0, regs[0] & prgSizeMask);
}

void Mapper010::saveSRAM(FILE * batteryFile) {
    MapperUtils::saveSRAM(wramBuffer, batteryFile);
}

void Mapper010::loadSRAM(FILE * batteryFile) {
    MapperUtils::loadSRAM(wramBuffer, batteryFile);
}
