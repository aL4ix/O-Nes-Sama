#include "Board.hpp"

Board::Board(unsigned char * header) {}

void Board::init(){
    prgSizeInBytes = prgSize16K * MapperUtils::_16K;
    chrSizeInBytes = chrSize8K * MapperUtils::_8K;
}

void Board::setiNESMapperNo(int mapperNo){
    iNESMapperNo = mapperNo;
}

void Board::setPrgSize16k(int prgSize){
    prgSize16K = prgSize;
}

void Board::setChrSize8k(int chrSize){
    chrSize8K = chrSize;
}

void Board::setNTMirroring(int mirroring){
    ntMirroring = mirroring;
}

void Board::setHasBattery (bool battery){
    hasBattery = battery;
}

void Board::setWramBuffer(unsigned char * wram){
    wramBuffer = wram;
}

void Board::setPRGBuffer(unsigned char * prg){
    prgBuffer = prg;
}

void Board::setCHRBuffer(unsigned char * chr){
    chrBuffer = chr;
}

void Board::setNTBuffer(unsigned char * nt){
    ntBuffer = nt;
}

void Board::setCPUIRQLine(int * irqLine){
    cpuIRQLine = irqLine;
}

void Board::setCPUCartSpaceMemPtr(unsigned char ** cpuPtr){
    cpuCartSpace = cpuPtr;
}

void Board::setPPUChrMemPtr(unsigned char ** ppuChrPtr){
    ppuChrSpace = ppuChrPtr;
}

void Board::setPPUNTMemPtr(unsigned char ** ppuNTPtr){
    ppuNTSpace = ppuNTPtr;
}

bool Board::loadState(FILE * file){

    auto size = 0;
    size += fread(tempR, sizeof(tempR), 1, file);

    if (chrSizeInBytes == 0){
        size += fread(chrBuffer, MapperUtils::_8K, 1, file);
    }
    if (wramBuffer)
        size += fread(wramBuffer, MapperUtils::_32K, 1, file);

    size += fread(ntBuffer, MapperUtils::_2K, 1, file);
    return size;
}

void Board::saveState(FILE * file){
    fwrite(tempR, sizeof(tempR), 1, file);
    if (chrSizeInBytes == 0)
        fwrite(chrBuffer, MapperUtils::_8K, 1, file);
    if (wramBuffer)
        fwrite(wramBuffer, MapperUtils::_32K, 1, file);
    fwrite(ntBuffer, MapperUtils::_2K, 1, file);
}

Board::~Board(){
    delete [] prgBuffer;
    delete [] chrBuffer;
    if (wramBuffer)
        delete [] wramBuffer;
    delete [] ntBuffer;
}
