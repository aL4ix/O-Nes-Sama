#ifndef MAPPERUTILS_HPP_INCLUDED
#define MAPPERUTILS_HPP_INCLUDED
#include <stdio.h>

class MapperUtils{

public:
    /**************************************************************************************************************/
    /* Helper constants                                                                                           */
    /**************************************************************************************************************/

    static const int _1K   =  0x0400;
    static const int _2K   =  0x0800;
    static const int _4K   =  0x1000;
    static const int _8K   =  0x2000;
    static const int _16K  =  0x4000;
    static const int _32K  =  0x8000;
    static const int _64K  = 0x10000;
    static const int _128K = 0x20000;
    static const int _256K = 0x40000;
    static const int _512K = 0x80000;

    /**************************************************************************************************************/
    /* PRG Switching functions                                                                                    */
    /**************************************************************************************************************/
    static void switchPRG4K (unsigned char * prgBuffer, unsigned char **& prgSpace, int cpuBank, int mapperBank);
    static void switchPRG8K (unsigned char * prgBuffer, unsigned char **& prgSpace, int cpuBank, int mapperBank);
    static void switchPRG16K (unsigned char * prgBuffer, unsigned char **& prgSpace, int cpuBank, int mapperBank);
    static void switchPRG32K (unsigned char * prgBuffer, unsigned char **& prgSpace, int mapperBank);

    /**************************************************************************************************************/
    /* CHR Switching functions                                                                                    */
    /**************************************************************************************************************/

    static void switchCHR1K (unsigned char * chrBuffer, unsigned char **& chrSpace, int ppuBank, int mapperBank);
    static void switchCHR2K (unsigned char * chrBuffer, unsigned char **& chrSpace, int ppuBank, int mapperBank);
    static void switchCHR4K (unsigned char * chrBuffer, unsigned char **& chrSpace, int ppuBank, int mapperBank);
    static void switchCHR8K (unsigned char * chrBuffer, unsigned char **& chrSpace, int mapperBank);

    /**************************************************************************************************************/
    /* Nametable Switching functions                                                                              */
    /**************************************************************************************************************/

    static void switchToHorMirroring(unsigned char * ntBuffer, unsigned char **& ntSpace);
    static void switchToVertMirroring(unsigned char * ntBuffer, unsigned char **& ntSpace);
    static void switchToSingleScrLow(unsigned char * ntBuffer, unsigned char **& ntSpace);
    static void switchToSingleScrHi(unsigned char * ntBuffer, unsigned char **& ntSpace);

    /**************************************************************************************************************/
    /* SRAM Functions                                                                                             */
    /**************************************************************************************************************/

    static void saveSRAM(unsigned char * wramBuffer, FILE * batteryFile);
    static void loadSRAM(unsigned char * wramBuffer, FILE * batteryFile);
};
#endif // MAPPERUTILS_HPP_INCLUDED
