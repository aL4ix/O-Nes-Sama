#include "CNROM.hpp"
#include <stdio.h>

Mapper003::Mapper003(CartIO &ioRef) : BasicMapper(ioRef){ }
Mapper087::Mapper087(CartIO &ioRef) : BasicMapper(ioRef){ }
Mapper101::Mapper101(CartIO &ioRef) : BasicMapper(ioRef){ }
Mapper145::Mapper145(CartIO &ioRef) : BasicMapper(ioRef){ }
Mapper149::Mapper149(CartIO &ioRef) : BasicMapper(ioRef){ }
Mapper185::Mapper185(CartIO &ioRef) : BasicMapper(ioRef){ }

void Mapper003::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            io.switch8K(0, val & 7, io.chrBuffer, io.chrSpace);
            break;
    }
}

void Mapper087::writeCPU(int address, unsigned char val){
    switch (address >> 12)
    {
        case 6: case 7:
            val = ((val & 1) << 1) | ((val & 3) >> 1);
            io.switch8K(0, val & 3, io.chrBuffer, io.chrSpace);
            break;
    }
}

void Mapper101::writeCPU(int address, unsigned char val){
    switch (address >> 12)
    {
        case 6: case 7:
            io.switch8K(0, val & 3, io.chrBuffer, io.chrSpace);
            break;
    }
}

void Mapper145::writeCPU(int address, unsigned char val){
    //Sidewinder (Sachen) mapper 145...
    if (address & 0xE100){
        io.switch8K(0, val >> 7, io.chrBuffer, io.chrSpace);
    }
}

void Mapper149::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            val = val >> 7;
            io.switch8K(0, val & 7, io.chrBuffer, io.chrSpace);
            break;
    }
}

void Mapper185::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
        {
            unsigned char copyProt = (val & 0x33);
            if (!(((copyProt & 0xF) == 0) || (copyProt == 0x13))){
                io.switch8K(0, val & (io.iNESHeader.chrSize8k - 1), io.chrBuffer, io.chrSpace);
            }
        }
        break;
    }
}

