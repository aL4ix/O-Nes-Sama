#include "CNROM.hpp"
#include <stdio.h>

/***************************************
* Mapper 03 Standard CNROM             *
****************************************/

CNROM::CNROM(CartIO &ioRef) : BasicMapper(ioRef){ }

void CNROM::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            bank = val;
            sync();
            break;
    }
}

void CNROM::loadState(FILE * file){
    fread (&bank, sizeof(unsigned char *), 1, file);
    BasicMapper::loadState(file);
    sync();
}

void CNROM::saveState(FILE * file){
    fwrite (&bank, sizeof(unsigned char *), 1, file);
    BasicMapper::saveState(file);
}

void CNROM::sync(){
    int mask = io.iNESHeader.chrSize8k - 1;
    io.switch8K(0, bank & mask, io.chrBuffer, io.chrSpace);
}

/***************************************
* Mapper 87 Alternate CNROM            *
****************************************/

Mapper087::Mapper087(CartIO &ioRef) : CNROM(ioRef){ }

void Mapper087::writeCPU(int address, unsigned char val){
    switch (address >> 12)
    {
        case 6: case 7:
            bank = ((val & 1) << 1) | ((val & 3) >> 1);
            CNROM::sync();
            break;
    }
}

/***************************************
* Mapper 101                           *
* Urusei Yatsura - Lum no Wedding Bell *
****************************************/

Mapper101::Mapper101(CartIO &ioRef) : CNROM (ioRef){ }

void Mapper101::writeCPU(int address, unsigned char val){
    switch (address >> 12)
    {
        case 6: case 7:
            bank = val;
            sync();
            break;
    }
}

/***************************************
* Mapper 145 - SA-72007 (Sachen)       *
****************************************/

Mapper145::Mapper145(CartIO &ioRef) : CNROM(ioRef){ }

void Mapper145::writeCPU(int address, unsigned char val){
    if (address & 0xE100){
        bank = val >> 7;
        sync();
    }
}

/***************************************
* Mapper 149 - SA-0036 (Sachen)        *
****************************************/

Mapper149::Mapper149(CartIO &ioRef) : CNROM(ioRef){ }

void Mapper149::writeCPU(int address, unsigned char val){

    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
            bank = val >> 7;
            sync();
            break;
    }
}

/********************************************
* Mapper 185 - CNROM with "copy protection" *
********************************************/

Mapper185::Mapper185(CartIO &ioRef) : CNROM(ioRef){ }

void Mapper185::writeCPU(int address, unsigned char val){
    int cp_val = 0;
    int isSeicrossV2 = io.iNESHeader.romCRC32 == seicrossV2;
    switch (address >> 12){
        case 8: case 9: case 10: case 11: case 12: case 13: case 14:case 15:
        {
            bank = (val & 0x33);

            if (isSeicrossV2)
                cp_val = 0x21;
            else
                cp_val = 0x13;

            if (((bank & 0xF) || isSeicrossV2) && (bank != cp_val)){
                sync();
            } else {
                io.switch8K(0, 0, disabledCHR, io.chrSpace);
            }
        }
        break;
    }
}

