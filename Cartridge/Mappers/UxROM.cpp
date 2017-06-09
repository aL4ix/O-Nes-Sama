#include "UxROM.hpp"
#include <stdio.h>

UxROM::UxROM(CartIO &ioRef) : BasicMapper(ioRef){
    //Fixed PRG-ROM bank (CPU 0xC000)
    mask = io.iNESHeader.prgSize16k - 1;
    io.switch16K(1, mask, io.prgBuffer, io.prgSpace);
}

void UxROM::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            bank = val;
            sync();
            break;
    }
}

void UxROM::loadState(FILE * file){
    fread (&bank, sizeof(unsigned char *), 1, file);
    BasicMapper::loadState(file);
    sync();
}

void UxROM::saveState(FILE * file){
    fwrite (&bank, sizeof(unsigned char *), 1, file);
    BasicMapper::saveState(file);
}

void UxROM::sync(){
    io.switch16K(0, bank & mask, io.prgBuffer, io.prgSpace);

}

/******************************************
* Code Masters (Micro Machines and others *
*******************************************/


Mapper071::Mapper071(CartIO &ioRef) : UxROM(ioRef){
    //Fixed PRG-ROM bank (CPU 0xC000)
    io.switch16K(1, (io.iNESHeader.prgSize16k - 1), io.prgBuffer, io.prgSpace);
}


void Mapper071::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 8: case 9:
            /*if ((val >> 4) & 1){
                io.switchToSingleScrHi();
            } else {
                io.switchToSingleScrLo();
            }*/
            break;
        case 12: case 13: case 14:case 15:
            bank = val;
            sync();
            //io.switch16K(0, val & (io.iNESHeader.prgSize16k - 1), io.prgBuffer, io.prgSpace);
            break;
    }
}
