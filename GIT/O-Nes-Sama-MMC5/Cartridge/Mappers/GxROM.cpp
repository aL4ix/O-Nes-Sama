#include "GxROM.hpp"

Mapper011::Mapper011(CartIO &ioRef) : BasicMapper(ioRef){ }
Mapper066::Mapper066(CartIO &ioRef) : BasicMapper(ioRef){ }
Mapper255::Mapper255(CartIO &ioRef) : BasicMapper(ioRef){

    mirr = 0;
    prgMode = 0;
    prgPos = 0;
    prgReg = 0;
    chrReg = 0;
    io.wRam = new unsigned char [0x2000];
    io.switch8K(0, 0, io.wRam, io.wRamSpace);
}


void Mapper066::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            val &= 0x3F;
            io.switch32K(0, val >> 4, io.prgBuffer, io.prgSpace);
            io.switch8K(0, val & 3, io.chrBuffer, io.chrSpace);
            break;
    }
}


void Mapper011::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            io.switch32K(0, val & 3, io.prgBuffer, io.prgSpace);
            io.switch8K(0, val >> 4, io.chrBuffer, io.chrSpace);
            break;
    }
}


void Mapper255::writeCPU(int address, unsigned char val){
    BasicMapper::writeCPU(address, val);
    switch (address >> 12){
        case 0x5:
            //4 x 4 bit ram register
            ramReg[address & 0x3] = val & 0xF;
            break;
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            {
                mirr    = (address >> 13) & 1;
                prgMode = (address >> 12) & 1;
                prgPos  = (address >> 6) & 1;
                prgReg  = ((address >> 7) & 0x1F);
                chrReg  = (address & 0x3F);

                //MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, chrReg);
                io.switch8K(0, chrReg, io.chrBuffer, io.chrSpace);

                if (!mirr)
                    io.switchVerMirroring();
                else
                    io.switchHorMirroring();

                if (!prgMode){
                    io.switch32K(0, prgReg, io.prgBuffer, io.prgSpace);
                } else {
                    io.switch16K(0, ((prgReg << 1) | prgPos), io.prgBuffer, io.prgSpace);
                    io.switch16K(1, ((prgReg << 1) | prgPos), io.prgBuffer, io.prgSpace);
                }
            }
        break;
    }
}
