#include "AxROM.hpp"
#include <stdio.h>

Mapper007::Mapper007(CartIO &ioRef) : BasicMapper(ioRef){ }


void Mapper007::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            bank = val;
            sync();
            break;
    }
}

void Mapper007::sync(){
    int mask = (io.iNESHeader.prgSize16k - 1) >> 1;

    io.swapPRGROM(32, 0, bank & mask, io.prgBuffer, 0);

    if (bank & 0x10)
        io.switchToSingleScrHi();

    else
        io.switchToSingleScrLo();

}

void Mapper007::loadState(FILE * file){
    fread (&bank, sizeof(unsigned char *), 1, file);
    BasicMapper::loadState(file);
    sync();
}

void Mapper007::saveState(FILE * file){
    fwrite (&bank, sizeof(unsigned char *), 1, file);
    BasicMapper::saveState(file);
}
