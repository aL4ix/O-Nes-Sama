#include "MapperUtils.hpp"

/**************************************************************************************************************/
/* PRG Switching functions                                                                                    */
/**************************************************************************************************************/
void MapperUtils::switchPRG4K (unsigned char * prgBuffer, unsigned char **& prgSpace, int cpuBank, int mapperBank){
    prgSpace[cpuBank] = &prgBuffer[mapperBank * _4K];
}

void MapperUtils::switchPRG8K (unsigned char * prgBuffer, unsigned char **& prgSpace, int cpuBank, int mapperBank){
    switchPRG4K(prgBuffer, prgSpace, cpuBank << 1, mapperBank << 1);
    switchPRG4K(prgBuffer, prgSpace, (cpuBank << 1) | 1, (mapperBank << 1) | 1);
}

void MapperUtils::switchPRG16K (unsigned char * prgBuffer, unsigned char **& prgSpace, int cpuBank, int mapperBank){
    switchPRG8K(prgBuffer, prgSpace, cpuBank << 1, mapperBank << 1);
    switchPRG8K(prgBuffer, prgSpace, (cpuBank << 1) | 1, (mapperBank << 1) | 1);
}

void MapperUtils::switchPRG32K (unsigned char * prgBuffer, unsigned char **& prgSpace, int mapperBank){
    switchPRG16K(prgBuffer, prgSpace, 0, mapperBank << 1);
    switchPRG16K(prgBuffer, prgSpace, 1, (mapperBank << 1) | 1);
}

/**************************************************************************************************************/
/* CHR Switching functions                                                                                    */
/**************************************************************************************************************/

void MapperUtils::switchCHR1K (unsigned char * chrBuffer, unsigned char **& chrSpace, int ppuBank, int mapperBank){
    chrSpace[ppuBank] = &chrBuffer[mapperBank * _1K];
}

void MapperUtils::switchCHR2K (unsigned char * chrBuffer, unsigned char **& chrSpace, int ppuBank, int mapperBank){
    switchCHR1K(chrBuffer, chrSpace, ppuBank << 1, mapperBank << 1);
    switchCHR1K(chrBuffer, chrSpace, (ppuBank << 1) | 1, (mapperBank << 1) | 1);
}

void MapperUtils::switchCHR4K (unsigned char * chrBuffer, unsigned char **& chrSpace, int ppuBank, int mapperBank){
    switchCHR2K(chrBuffer, chrSpace, ppuBank << 1, mapperBank << 1);
    switchCHR2K(chrBuffer, chrSpace, (ppuBank << 1) | 1, (mapperBank << 1) | 1);
}

void MapperUtils::switchCHR8K (unsigned char * chrBuffer, unsigned char **& chrSpace, int mapperBank){
    switchCHR4K(chrBuffer, chrSpace, 0, mapperBank << 1);
    switchCHR4K(chrBuffer, chrSpace, 1, (mapperBank << 1) | 1);
}

/**************************************************************************************************************/
/* Nametable Switching functions                                                                              */
/**************************************************************************************************************/

void MapperUtils::switchToHorMirroring(unsigned char * ntBuffer, unsigned char **& ntSpace){
    ntSpace[0] = ntSpace[1] = &ntBuffer[0];
    ntSpace[2] = ntSpace[3] = &ntBuffer[0x400];
}

void MapperUtils::switchToVertMirroring(unsigned char * ntBuffer, unsigned char **& ntSpace){
    ntSpace[0] = ntSpace[2] = &ntBuffer[0];
    ntSpace[1] = ntSpace[3] = &ntBuffer[0x400];
}

void MapperUtils::switchToSingleScrLow(unsigned char * ntBuffer, unsigned char **& ntSpace){
    ntSpace[0] = ntSpace[1] = ntSpace[2] = ntSpace[3] = &ntBuffer[0];
}

void MapperUtils::switchToSingleScrHi(unsigned char * ntBuffer, unsigned char **& ntSpace){
    ntSpace[0] = ntSpace[1] = ntSpace[2] = ntSpace[3] = &ntBuffer[0x400];
}

/**************************************************************************************************************/
/* SRAM Functions                                                                                             */
/**************************************************************************************************************/

void MapperUtils::saveSRAM(unsigned char * wramBuffer, FILE * batteryFile){
    fwrite(wramBuffer, _32K, 1, batteryFile);
}

void MapperUtils::loadSRAM(unsigned char * wramBuffer, FILE * batteryFile){
    fread(wramBuffer, _32K, 1, batteryFile);
}
