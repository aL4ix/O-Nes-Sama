#ifndef CARTIO_HPP_INCLUDED
#define CARTIO_HPP_INCLUDED
#include "../CPUIO.hpp"
#include <stdio.h>

struct debug {
    int* isRendering;
    int* tick;
    int* sl;
};

struct iNESHdr {
    int prgSize16k;
    int chrSize8k;
    // Flags 6
    int ntMirroring;
    int hasBackedWram;
    int hasTrainer;
    int has4ScrVram;
    int mapperNoLow;
    // Flags 7
    int isVSUnisystem;
    int isPlayChoice10;
    int iNESVersion;
    int mapperNoHigh;
    int mapperNo;
    unsigned long int romCRC32;
};

struct CartIO {

    unsigned short* ppuAddrBus;
    int chrWritable;
    int prgWritable;
    int wRamWritable;

    unsigned char ppuDataBus;
    struct iNESHdr iNESHeader;
    struct debug dbg;
    struct CPUIO* cpuIO;

    // Memory buffers
    unsigned char ntSystemRam[0x800] = { 0xFF };
    unsigned char* prgBuffer = nullptr;
    unsigned char* chrBuffer = nullptr;
    unsigned char* wRam = nullptr;

    /*Pointers for handling bank switching*/
    unsigned char* prgSpace[32];
    unsigned char* wRamSpace[8];
    unsigned char* chrSpace[8];
    unsigned char* ntSpace[4];

    /*Bank Switching Functions*/

    inline void switch1K(int offset, int memBank, unsigned char* memBuffer, unsigned char** space, int outer = 0)
    {
        space[offset] = &memBuffer[outer + memBank * 0x400];
    }

    inline void switch2K(int offset, int memBank, unsigned char* memBuffer, unsigned char** space, int outer = 0)
    {
        memBank <<= 1;
        offset <<= 1;
        for (int i = 0; i < 2; i++) {
            space[offset + i] = &memBuffer[outer + (memBank + i) * 0x400];
        }
    }

    inline void switch4K(int offset, int memBank, unsigned char* memBuffer, unsigned char** space, int outer = 0)
    {
        memBank <<= 2;
        offset <<= 2;
        for (int i = 0; i < 4; i++) {
            space[offset + i] = &memBuffer[outer + (memBank + i) * 0x400];
        }
    }

    inline void switch8K(int offset, int memBank, unsigned char* memBuffer, unsigned char** space, int outer = 0)
    {
        memBank <<= 3;
        offset <<= 3;

        for (int i = 0; i < 8; i++) {
            space[offset + i] = &memBuffer[outer + (memBank + i) * 0x400];
        }
    }

    inline void switch16K(int offset, int memBank, unsigned char* memBuffer, unsigned char** space, int outer = 0)
    {
        memBank <<= 4;
        offset <<= 4;
        for (int i = 0; i < 16; i++) {
            space[offset + i] = &memBuffer[outer + (memBank + i) * 0x400];
        }
    }

    inline void switch32K(int memBank, unsigned char* memBuffer, unsigned char** space, int outer = 0)
    {

        memBank <<= 5;
        for (int i = 0; i < 32; i++) {
            space[i] = &memBuffer[outer + (memBank + i) * 0x400];
        }
    }

    inline void setOuter256KBank(int memBank)
    {
    }

    /*NT mirroring functions*/

    void switchVerMirroring()
    {
        ntSpace[0] = ntSpace[2] = ntSystemRam;
        ntSpace[1] = ntSpace[3] = &ntSystemRam[0x400];
    }

    void switchHorMirroring()
    {
        ntSpace[0] = ntSpace[1] = ntSystemRam;
        ntSpace[2] = ntSpace[3] = &ntSystemRam[0x400];
    }

    void switchToSingleScrLo()
    {
        ntSpace[0] = ntSpace[1] = ntSpace[2] = ntSpace[3] = ntSystemRam;
    }

    void switchToSingleScrHi()
    {
        ntSpace[0] = ntSpace[1] = ntSpace[2] = ntSpace[3] = &ntSystemRam[0x400];
    }
};

#endif // CARTIO_HPP_INCLUDED
