#include "Cartridge.hpp"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "Mappers/BasicMapper.hpp"
#include "Mappers/UxROM.hpp"
#include "Mappers/CNROM.hpp"
#include "Mappers/AxROM.hpp"
#include "Mappers/GxROM.hpp"
#include "Mappers/MMC1.hpp"
#include "Mappers/MMC2_4.hpp"
#include "Mappers/MMC3.hpp"
#include "Mappers/MMC5.hpp"

using namespace std;
std::string batteryPath   = "Battery";

Cartridge::Cartridge(std::string fileName){
    romFileName = fileName;
    batteryPath.append(getBaseRomName(romFileName));
    batteryPath.append(".sav");
    printf("\nRom File path   : %s", romFileName.c_str());
    printf("\nBattery path    : %s", batteryPath.c_str());
    loadRomFile();

    if (mapper->io.iNESHeader.hasBackedWram){
        FILE * batteryFile = fopen(batteryPath.c_str(), "rb");
        if (batteryFile != NULL){
            mapper->loadSRAM(batteryFile);
            fclose(batteryFile);
        }
        else
            printf ("\nError while loading SRAM file");
    }
}

void Cartridge::loadRomFile(){
    ifstream file (romFileName, ios::in|ios::binary|ios::ate);

    if (file.is_open()) {
        unsigned char header[16];


        /*Go to the beginning of the ROM file*/
        file.seekg (0, ios::beg);
        /*Get the 16 bytes of the iNES header*/
        file.read ((char *)header, 0x10);

        io.iNESHeader.mapperNo = (header[7] & 0xF0) | (header[6] >> 4);
        io.iNESHeader.ntMirroring = header[6] & 1;
        io.iNESHeader.hasBackedWram = (header[6] & 0x2) >> 1;

        /*Allocate memory for cartridge Memory Buffers*/
        io.iNESHeader.prgSize16k = header[4];
        int prgSizeInBytes = io.iNESHeader.prgSize16k * 0x4000;//MapperUtils::_16K;
        io.prgBuffer = new unsigned char [prgSizeInBytes];

        /*Get the PRG bytes and store them in the to PRG buffer.*/
        file.seekg (0x10, ios::beg);
        file.read (reinterpret_cast<char*>(io.prgBuffer), prgSizeInBytes);

        /*Get the CHR bytes and store them in the to CHR buffer.*/
        io.iNESHeader.chrSize8k = header[5];
        int chrSizeInBytes = io.iNESHeader.chrSize8k * 0x2000;//MapperUtils::_8K;
        io.chrWritable = !io.iNESHeader.chrSize8k; //If Chr size == 0, then writable (RAM).
        printf ("\nIs CHR Writable: %x", io.chrWritable);

        if (io.chrWritable)
        {
            io.chrBuffer = new unsigned char [0x2000]; //Chr RAM 8K
        }

        else  //CHR ROM!
        {
            io.chrBuffer = new unsigned char [chrSizeInBytes];
            file.seekg (prgSizeInBytes + 0x10, ios::beg);
            file.read (reinterpret_cast<char*>(io.chrBuffer), chrSizeInBytes);
        }

        unsigned long int crc = crc32(0L, NULL, 0L);
        crc =  crc32(crc, io.prgBuffer, prgSizeInBytes);
        crc =  crc32(crc, io.chrBuffer, chrSizeInBytes);
        io.iNESHeader.romCRC32 = crc;
        printf ("\nCRC                     : %lX", crc);
        mapper = createMapper(io.iNESHeader.mapperNo);
        file.close();

        if (!mapper){
            printf("Error: Mapper creation returned NULL (Unsupported mapper no '%d' ?)", io.iNESHeader.mapperNo);
            exit(1);
        }

        printf ("\nMapper No               : %d", io.iNESHeader.mapperNo);
        printf ("\nPRG Size                : %d", io.iNESHeader.prgSize16k);
        printf ("\nCHR Size                : %d", io.iNESHeader.chrSize8k);
        printf ("\nHas battery backed wRam : %d\n", io.iNESHeader.hasBackedWram);

    } else {
        printf("\nInvalid Rom File...");
        exit(1);
    }

}

MemoryMapper * Cartridge::createMapper(int mapperNo){
    MemoryMapper * mpr = NULL;
    switch(mapperNo){
        case 0:
            mpr = new BasicMapper(io);
            break;
        case 1:
            mpr = new MMC1(io);
            break;
        case 2:
            mpr = new Mapper002(io);
            break;
        case 3:
            mpr = new Mapper003(io);
            break;
        case 4:
            mpr = new MMC3(io);
            break;
        case 5:
            mpr = new MMC5(io);
            break;
        case 7:
            mpr = new Mapper007(io);
            break;
        case 9: case 10:
            mpr = new MMC2(io);
            break;
        case 11:
            mpr = new Mapper011(io);
            break;
        case 66:
            mpr = new Mapper066(io);
            break;
        case 71:
            mpr = new Mapper071(io);
            break;
        case 87:
            mpr = new Mapper087(io);
            break;
        case 101:
            mpr = new Mapper101(io);
            break;
        case 145:
            mpr = new Mapper145(io);
            break;
        case 149:
            mpr = new Mapper149(io);
            break;
        case 185:
            mpr = new Mapper185(io);
            break;
        case 225:
            mpr = new Mapper255(io);
            break;
    }
    return mpr;
}

bool Cartridge::loadState(FILE * file){
//    return mapper->loadState(file);
}

void Cartridge::saveState(FILE * file){
//    mapper->saveState(file);
}

std::string Cartridge::getBaseRomName(std::string tmpRomName){
    std::string fileNameBase;
    int extIndex = tmpRomName.find_last_of(".");
    tmpRomName.replace(tmpRomName.begin() + extIndex, tmpRomName.end(), "");
    int pathSepIndex = tmpRomName.find_last_of("/");
    tmpRomName.replace(tmpRomName.begin(), tmpRomName.begin() + pathSepIndex, "");
    fileNameBase = tmpRomName;
    return fileNameBase;
}

Cartridge::~Cartridge(){
    if (io.iNESHeader.hasBackedWram){
        FILE * batteryFile = fopen(batteryPath.c_str(), "wb");
        if (batteryFile != NULL){
            mapper->saveSRAM(batteryFile);
            fclose(batteryFile);
        }
        else
            printf ("\nError while saving SRAM file");
    }
}


