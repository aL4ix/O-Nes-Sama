#include "Cartridge.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

Cartridge::Cartridge(std::string fileName){
    romFileName = fileName;
    loadRomFile();
}

void Cartridge::loadRomFile(){
    ifstream file (romFileName, ios::in|ios::binary|ios::ate);
    unsigned char iNESHeader[16];

    if (file.is_open()) {
        /*Go to the beginning of the ROM file*/
        file.seekg (0, ios::beg);
        /*Get the 16 bytes of the iNES header*/
        file.read ((char *)iNESHeader, 0x10);
        int mapperNo = (iNESHeader[7] & 0xF0) | (iNESHeader[6] >> 4);
        mapper = createMapper(mapperNo, iNESHeader);
        if (!mapper){
            printf("Error: Mapper creation returned NULL (Unsupported mapper no '%d' ?)", mapperNo);
            exit(1);
        }
        int chrRomSize = iNESHeader[5];
        int prgRomSize = iNESHeader[4];
        int prgSizeInBytes = prgRomSize * MapperUtils::_16K;
        int chrSizeInBytes = chrRomSize * MapperUtils::_8K;
        hasBattery = (iNESHeader[6] & 0x2) >> 1;
        mapper->setNTMirroring(iNESHeader[6] & 1);
        mapper->setPrgSize16k(prgRomSize);
        mapper->setChrSize8k(chrRomSize);
        mapper->setHasBattery(hasBattery);
        printf ("\nMapper No   : %d", mapperNo);
        printf ("\nPRG Size    : %d", prgRomSize);
        printf ("\nCHR Size    : %d", chrRomSize);
        printf ("\nHas Battery : %d", hasBattery);
        /*Allocate memory for cartridge Memory Buffers*/
        unsigned char * prgBuffer = new unsigned char [prgSizeInBytes];
        /*Get the PRG bytes and store them in the to PRG buffer.*/
        file.seekg (0x10, ios::beg);
        file.read (reinterpret_cast<char*>(prgBuffer), prgSizeInBytes);
        unsigned char * chrBuffer;
        /*Get the CHR bytes and store them in the to CHR buffer.*/
        if (chrRomSize > 0){ //CHR ROM!
            chrBuffer = new unsigned char [chrSizeInBytes];
            file.seekg (prgSizeInBytes + 0x10, ios::beg);
            file.read (reinterpret_cast<char*>(chrBuffer), chrSizeInBytes);
            mapper->ppuStatus.chrReadOnly = true;
        } else { //Cart's got 8Kb CHR-RAM!
            chrBuffer = new unsigned char [0x2000];
            mapper->ppuStatus.chrReadOnly = false;
        }
        mapper->setPRGBuffer(prgBuffer);
        mapper->setCHRBuffer(chrBuffer);
        file.close();
    } else {
        printf("\nInvalid Rom File...");
        exit(1);
    }
}

/*unsigned char Cartridge::read(int addr){
    return mapper->read(addr);
}

void Cartridge::write(int addr, unsigned char val){
    mapper->write(addr, val);
}*/

Board * Cartridge::createMapper(int mapperNo, unsigned char * header){
    Board * mpr = NULL;
    switch(mapperNo){
        case 0:
            mpr = new Mapper000(header);
            break;
        case 1:
            mpr = new MMC1(header);
            break;
        case 2:
            mpr = new Mapper002(header);
            break;
        case 3:
            mpr = new Mapper003(header);
            break;
        case 4:
            mpr = new MMC3(header);
            break;
        case 7:
            mpr = new Mapper007(header);
            break;
        case 9:
            mpr = new Mapper009(header);
            break;
        case 10:
            mpr = new Mapper010(header);
            break;
        case 11:
            mpr = new Mapper011(header);
            break;
        case 66:
            mpr = new Mapper066(header);
            break;
        case 71:
            mpr = new Mapper071(header);
            break;
        case 87:
            mpr = new Mapper087(header);
            break;
        case 185:
            mpr = new Mapper185(header);
            break;
        case 225:
            mpr = new Mapper225(header);
            break;
    }
    return mpr;
}

bool Cartridge::loadState(FILE * file){
    return mapper->loadState(file);
}

void Cartridge::saveState(FILE * file){
    mapper->saveState(file);
}

Cartridge::~Cartridge(){
}

