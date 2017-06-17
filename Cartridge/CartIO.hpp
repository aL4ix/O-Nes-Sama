#ifndef CARTIO_HPP_INCLUDED
#define CARTIO_HPP_INCLUDED
#include <stdio.h>
#include "../CPUIO.hpp"

struct debug{
    bool * isRendering;
    bool isFetchingBGTile;
    unsigned char * reg2000;
    unsigned char * reg2001;
    unsigned char * atColor;
    int * tick;
    int * sl;
};

struct iNESHdr{
    int prgSize16k;
    int chrSize8k;
    //Flags 6
    int ntMirroring;
    int hasBackedWram;
    int hasTrainer;
    int has4ScrVram;
    int mapperNoLow;
    //Flags 7
    int isVSUnisystem;
    int isPlayChoice10;
    int iNESVersion;
    int mapperNoHigh;
    int mapperNo;
    unsigned long int romCRC32;
};

struct CartIO {

    int ppuAddrBus;
    int chrWritable;
    int prgWritable;
    int wRamWritable;

    unsigned char ppuDataBus;
    struct iNESHdr iNESHeader;
    struct debug dbg;
    struct CPUIO * cpuIO;

    //Memory buffers
    unsigned char ntSystemRam[0x800];
    unsigned char * prgBuffer;
    unsigned char * chrBuffer;
    unsigned char * wRam;

    /*Pointers for handling bank switching*/
    unsigned char * prgSpace[32];
    unsigned char * wRamSpace;
    unsigned char * chrSpace[8];
    unsigned char * ntSpace[4];

    /*PRG Bank Switching Functions*/

    inline void swapPRGROM (int size, int offset, int bank, unsigned char * buffer, int wr=0){
        int shift = 0;
        switch (size){
            case 2:  shift = 1;  break;
            case 4:  shift = 2;  break;
            case 8:  shift = 3;  break;
            case 16: shift = 4; break;
            case 32: shift = 5; break;
        }

        if (shift == 0){
            prgSpace[offset] = &buffer[(bank * 0x400)];
        } else {
            bank <<= shift;
            offset <<= shift;

            for (int i=0; i < size; i++){
                prgSpace[offset + i] = &buffer[(bank + i) * 0x400];
            }
        }

        prgWritable = wr;
    }

    inline void swapPRGRAM (int bank, int wr=0){
        wRamSpace = &wRam[bank * 0x2000];
        wRamWritable = wr;
    }

    inline void swapPRGRAMx (int offset, int bank, int wr=0){
        bank <<= 3;
        offset <<= 3;

        for (int i=0; i < 8; i++){
            prgSpace[offset + i] = &wRam[(bank + i) * 0x400];
        }

        //prgSpace[offset] = wRamSpace;
        //wRamSpace = &wRam[bank * 0x2000];
        prgWritable = 1;
    }

    inline void swapCHR (int size, int offset, int bank, unsigned char * buffer, int wr=0){
        int shift = 0;
        switch (size){
            case 2: shift = 1;  break;
            case 4: shift = 2;  break;
            case 8: shift = 3;  break;
        }

        if (shift == 0){
            chrSpace[offset] = &buffer[(bank * 0x400)];
        } else {
            bank <<= shift;
            offset <<= shift;

            for (int i=0; i < size; i++){
                chrSpace[offset + i] = &buffer[(bank + i) * 0x400];
            }
        }
    }

    /*NT mirroring functions*/

    void switchVerMirroring ()
    {
        ntSpace[0] = ntSpace[2] = ntSystemRam;
        ntSpace[1] = ntSpace[3] = &ntSystemRam[0x400];
    }

    void switchHorMirroring ()
    {
        ntSpace[0] = ntSpace[1] = ntSystemRam;
        ntSpace[2] = ntSpace[3] = &ntSystemRam[0x400];
    }

    void switchToSingleScrLo ()
    {
        ntSpace[0] = ntSpace[1] = ntSpace[2] = ntSpace[3] = ntSystemRam;
    }

    void switchToSingleScrHi ()
    {
        ntSpace[0] = ntSpace[1] = ntSpace[2] = ntSpace[3] = &ntSystemRam[0x400];
    }
};


#endif // CARTIO_HPP_INCLUDED
