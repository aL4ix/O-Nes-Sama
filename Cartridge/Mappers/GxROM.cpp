#include "GxROM.hpp"

Mapper011::Mapper011(CartIO &ioRef) : BasicMapper(ioRef){ }
Mapper066::Mapper066(CartIO &ioRef) : BasicMapper(ioRef){ }

void Mapper066::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            val &= 0x3F;
            io.switch32K(val >> 4, io.prgBuffer, io.prgSpace);
            io.switch8K(0, val & 3, io.chrBuffer, io.chrSpace);
            break;
    }
}


void Mapper011::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            io.switch32K(val & 3, io.prgBuffer, io.prgSpace);
            io.switch8K(0, val >> 4, io.chrBuffer, io.chrSpace);
            break;
    }
}
