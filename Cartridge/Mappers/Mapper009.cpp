#include "Mapper009.hpp"

Mapper009::Mapper009(unsigned char * header) : MMC2(header){}

void Mapper009::init(){
    MMC2::init();
    prgSizeMask = (prgSize16K << 1) - 1;
    /*Fix PRG buffer's last 24KB to the tree last banks of the CPU PRG space*/
    MapperUtils::switchPRG8K(prgBuffer, prg, 1, prgSizeMask - 2);
    MapperUtils::switchPRG8K(prgBuffer, prg, 2, prgSizeMask - 1);
    MapperUtils::switchPRG8K(prgBuffer, prg, 3, prgSizeMask);
}


void Mapper009::syncPRG(){
    MapperUtils::switchPRG8K(prgBuffer, prg, 0, prgBank & prgSizeMask);
}
