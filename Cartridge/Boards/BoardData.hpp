#ifndef BOARDDATA_HPP_INCLUDED
#define BOARDDATA_HPP_INCLUDED

struct BoardData{
    int * cpuIRQLine;
    int * cpuCCount;
    int * ppuAddrBus;
    int prgRomSize16;
    int chrRomSize8;
    unsigned char * iNESHeader;
    unsigned char * prgBuffer;
    unsigned char * chrBuffer;
    unsigned char * wRamBuffer;
    unsigned char * ntBuffer;
    /*Pointers for handling bank switching*/
    unsigned char ** cpuCartSpace;
    unsigned char ** ppuCartSpace;
    unsigned char ** cpuPrgSpace;
    unsigned char ** ppuChrSpace;
    unsigned char ** ppuNTSpace;
};

#endif // BOARDDATA_HPP_INCLUDED
