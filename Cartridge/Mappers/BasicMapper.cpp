#include "BasicMapper.hpp"
#include <stdio.h>

BasicMapper::BasicMapper(CartIO &ioRef) : MemoryMapper(ioRef){

    //Init PRG & CHR
    io.switch16K(0, 0, io.prgBuffer, io.prgSpace);
    io.switch16K(1, io.iNESHeader.prgSize16k > 1, io.prgBuffer, io.prgSpace);
    io.switch8K(0, 0, io.chrBuffer, io.chrSpace);

    if (io.iNESHeader.ntMirroring) //Vertical
    {
        io.switchVerMirroring();
    }

    else //Horizontal
    {
        io.switchHorMirroring();
    }
}

inline unsigned char BasicMapper::readCPU(int address){
    unsigned char ret = io.cpuIO->dataBus;
    switch (address >> 12){
        case 6: case 7:
            if (io.wRam)
                ret = io.wRamSpace[(address - 0x6000) >> 10][address & 0x3FF];
            break;
        case 8: case 9: case 10: case 11:
        case 12: case 13: case 14:case 15:
            ret = io.prgSpace[(address >> 10) & 0x1F][address & 0x3FF];
            break;
    }
    return ret;
}

inline void BasicMapper::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 6: case 7:
            if (io.wRam)
                io.wRamSpace[(address-0x6000) >> 10][address & 0x3FF] = val;
            break;
    }
}

inline unsigned char BasicMapper::readPPU(int address){

    int mask1K = address >> 10;
    if (address > 0x1FFF)
        *io.ppuAddrBus = address & 0x2fff;

    switch (mask1K){
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            return io.chrSpace[mask1K & 7][address & 0x3FF];
        case 8: case 9: case 10: case 11:
            return io.ntSpace[mask1K & 3][address & 0x3FF];
    }
    return 0;
}

inline void BasicMapper::writePPU(int address, unsigned char val){

    int mask1K = address >> 10;

    if (address > 0x1FFF)
        *io.ppuAddrBus = address & 0x2fff;

    switch (mask1K){
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            if (io.chrWritable){
                io.chrSpace[mask1K][address & 0x3FF] = val;

            }
            return;
        case 8: case 9: case 10: case 11:
            io.ntSpace[mask1K & 3][address & 0x3FF] = val;
            return;
    }
}

BasicMapper::~BasicMapper(){
    delete [] io.wRam;
    delete [] io.prgBuffer;
    delete [] io.chrBuffer;
}
