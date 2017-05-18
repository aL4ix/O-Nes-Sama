#include "AxROM.hpp"
#include <stdio.h>

Mapper007::Mapper007(CartIO &ioRef) : BasicMapper(ioRef){ }


void Mapper007::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:

            io.switch32K(val & 7, io.prgBuffer, io.prgSpace);

            if (val & 0x10)
            {
                io.switchToSingleScrHi();
            }

            else
            {
                io.switchToSingleScrLo();
            }

            break;
    }
}
