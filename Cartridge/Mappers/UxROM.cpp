#include "UxROM.hpp"
#include <stdio.h>

Mapper002::Mapper002(CartIO &ioRef) : BasicMapper(ioRef){
    //Fixed PRG-ROM bank (CPU 0xC000)
    io.switch16K(1, (io.iNESHeader.prgSize16k - 1), io.prgBuffer, io.prgSpace);
}
Mapper071::Mapper071(CartIO &ioRef) : BasicMapper(ioRef){
    //Fixed PRG-ROM bank (CPU 0xC000)
    io.switch16K(1, (io.iNESHeader.prgSize16k - 1), io.prgBuffer, io.prgSpace);
}


void Mapper002::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            io.switch16K(0, val & (io.iNESHeader.prgSize16k - 1), io.prgBuffer, io.prgSpace);
            break;
    }
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
            io.switch16K(0, val & (io.iNESHeader.prgSize16k - 1), io.prgBuffer, io.prgSpace);
            break;
    }
}
