#include "BasicMapper.hpp"
#include <stdio.h>

BasicMapper::BasicMapper(CartIO &ioRef) : MemoryMapper(ioRef){


    //Init PRG & CHR
    io.swapPRGROM(16, 0, 0, io.prgBuffer, 0);
    io.swapPRGROM(16, 1, io.iNESHeader.prgSize16k > 1, io.prgBuffer, 0);
    io.swapCHR(8, 0, 0, io.chrBuffer);

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
                ret = io.wRamSpace[address & 0x1FFF];
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
            if (io.wRam){
                io.wRamSpace[address & 0x1FFF] = val;
            }
            break;
    }
}

inline unsigned char BasicMapper::readPPU(int address){

    int div400 = address >> 10;

    *io.ppuAddrBus = address;

    if (address >= 0x3000){
        address &= ~ 0x1000;
    }

    switch (div400){
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            return io.chrSpace[div400 & 7][address & 0x3FF];
        case 8: case 9: case 10: case 11:
            return io.ntSpace[div400 & 3][address & 0x3FF];
    }
    return 0;
}

inline void BasicMapper::writePPU(int address, unsigned char val){

    int div400 = address >> 10;

    *io.ppuAddrBus = address;

    if (address >= 0x3000)
        address &= ~ 0x1000;

    switch (div400){
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            if (io.chrWritable){
                io.chrSpace[div400][address & 0x3FF] = val;
            }
            return;
        case 8: case 9: case 10: case 11:
            io.ntSpace[div400 & 3][address & 0x3FF] = val;
            return;
    }
}


void BasicMapper::saveState(FILE * file) {

    if (io.wRam)
        fwrite(io.wRam, sizeof (unsigned char), 0x2000, file);
    if (io.iNESHeader.chrSize8k == 0)
        fwrite(io.chrBuffer, sizeof (unsigned char), 0x2000, file);
    fwrite(io.ntSystemRam, sizeof (unsigned char), 0x800, file);

}

void BasicMapper::loadState(FILE * file) {
    if (io.wRam)
        fread(io.wRam, sizeof (unsigned char), 0x2000, file);
    if (io.iNESHeader.chrSize8k == 0)
        fread(io.chrBuffer, sizeof (unsigned char), 0x2000, file);
    fread(io.ntSystemRam, sizeof (unsigned char), 0x800, file);
}

BasicMapper::~BasicMapper(){
    delete [] io.wRam;
    delete [] io.prgBuffer;
    delete [] io.chrBuffer;
}
