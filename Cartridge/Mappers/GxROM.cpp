#include "GxROM.hpp"
/***************************************
* Mapper 66 GNROM                      *
****************************************/

GNROM::GNROM(CartIO &ioRef) : BasicMapper(ioRef){
    prgMask = io.iNESHeader.prgSize16k - 1;
    chrMask = io.iNESHeader.chrSize8k - 1;
}

void GNROM::sync(){
    io.swapPRGROM(32, 0, prgBank, io.prgBuffer, 0);
    io.swapCHR(8, 0, chrBank, io.chrBuffer);
}
void GNROM::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            prgBank = (val >> 4) & prgMask;
            chrBank = (val & 3) & chrMask;
            sync();
            break;
    }
}

void GNROM::loadState(FILE * file){
    fread (&prgBank, sizeof(unsigned char *), 1, file);
    fread (&chrBank, sizeof(unsigned char *), 1, file);
    BasicMapper::loadState(file);
    sync();
}

void GNROM::saveState(FILE * file){
    fwrite (&prgBank, sizeof(unsigned char *), 1, file);
    fwrite (&chrBank, sizeof(unsigned char *), 1, file);
    BasicMapper::saveState(file);
}
/***************************************
* Mapper 11 ColorDreams                *
****************************************/

Mapper011::Mapper011(CartIO &ioRef) : GNROM(ioRef){ }

void Mapper011::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            prgBank = (val & 3) & prgMask;
            chrBank = (val >> 4) & chrMask;
            sync();
            break;
    }
}

void Mapper011::sync(){
    io.swapPRGROM(32, 0, prgBank, io.prgBuffer, 0);
    io.swapCHR(8, 0, chrBank, io.chrBuffer, 0);
}


/***************************************
* Mapper 225 (64 in 1, possibly others *
****************************************/

Mapper225::Mapper225(CartIO &ioRef) : BasicMapper(ioRef){

    mirr = 0;
    prgMode = 0;
    prgPos = 0;
    prgReg = 0;
    chrReg = 0;
    sync();
}

void Mapper225::sync(){

    io.swapCHR(8, 0, chrReg, io.chrBuffer);

    if (!mirr)
        io.switchVerMirroring();
    else
        io.switchHorMirroring();

    if (!prgMode){
        io.swapPRGROM(32, 0, hiBit | prgReg, io.prgBuffer, 0);
    } else {
        io.swapPRGROM(16, 0, ((prgReg << 1) | hiBit) | prgPos, io.prgBuffer, 0);
        io.swapPRGROM(16, 1, ((prgReg << 1) | hiBit) | prgPos, io.prgBuffer, 0);
    }
}

unsigned char Mapper225::readCPU(int address){
    switch (address >> 12){
        case 0x5:
            //4 x 4 bit ram register
            return ramReg[address & 0x3];
    }
    return BasicMapper::readCPU(address);
}

void Mapper225::writeCPU(int address, unsigned char val){
    BasicMapper::writeCPU(address, val);
    switch (address >> 12){
        case 0x5:
            //4 x 4 bit ram register
            ramReg[address & 0x3] = val & 0xF;
            break;
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            hiBit   = (address >> 7) & 0x80;
            mirr    = (address >> 13) & 1;
            prgMode = (address >> 12) & 1;
            prgPos  = (address >> 6) & 1;
            prgReg  = ((address >> 7) & 0x1F);
            chrReg  = (address & 0x3F);
            break;
    }
    sync();
}

void Mapper225::saveState(FILE * file){

    fwrite (&mirr, sizeof(unsigned char *), 1, file);
    fwrite (&prgMode, sizeof(unsigned char *), 1, file);
    fwrite (&prgPos, sizeof(unsigned char *), 1, file);
    fwrite (&prgReg, sizeof(unsigned char *), 1, file);
    fwrite (&chrReg, sizeof(unsigned char *), 1, file);
    fwrite (ramReg, sizeof(unsigned char), 4, file);
    BasicMapper::saveState(file);
}

void Mapper225::loadState(FILE * file){

    fread (&mirr, sizeof(unsigned char *), 1, file);
    fread (&prgMode, sizeof(unsigned char *), 1, file);
    fread (&prgPos, sizeof(unsigned char *), 1, file);
    fread (&prgReg, sizeof(unsigned char *), 1, file);
    fread (&chrReg, sizeof(unsigned char *), 1, file);
    fread (ramReg, sizeof(unsigned char), 4, file);
    BasicMapper::loadState(file);
    sync();
}
