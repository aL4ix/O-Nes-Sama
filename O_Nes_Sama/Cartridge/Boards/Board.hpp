#ifndef BOARD_HPP_INCLUDED
#define BOARD_HPP_INCLUDED
#include "../Mappers/MapperUtils.hpp"
#include <stdio.h>
#include <string.h>

struct PPUStatus{
    const int * sline;
    const int * tick;
    int debug;
    bool chrReadOnly;
    bool * isRendering;
};

class Board {
    public:
        Board(unsigned char * header);
        ~Board();
        void setiNESMapperNo(int mapperNo);
        void setPrgSize16k(int prgSize);
        void setChrSize8k(int chrSize);
        void setNTMirroring(int mirroring);
        void setHasBattery (bool battery);
        void setWramBuffer(unsigned char * wram);
        void setPRGBuffer(unsigned char * prg);
        void setCHRBuffer(unsigned char * chr);
        void setNTBuffer(unsigned char * nt);
        void setCPUIRQLine(int * irqLine);
        void setCPUCartSpaceMemPtr(unsigned char ** cpuPtr);
        void setPPUChrMemPtr(unsigned char ** ppuChrPtr);
        void setPPUNTMemPtr(unsigned char ** ppuNtPtr);
        virtual void init();
        virtual void sync() = 0;
        virtual void setPPUAddress(int addr) = 0;
        virtual unsigned char read(int addr) = 0;
        virtual void write(int addr, unsigned char val) = 0;
        virtual void saveSRAM(FILE *) = 0;
        virtual void loadSRAM(FILE *) = 0;
        bool loadState(FILE * file);
        void saveState(FILE * file);
        virtual void clockCPU() = 0;
        virtual void clockPPU() = 0;
        PPUStatus ppuStatus;

    protected:
        //Define an array of 8 general purpose registers for serialization convenience.
        int iNESMapperNo;
        int prgSize16K;
        int chrSize8K;
        int ntMirroring;
        int ppuAddress;
        int *cpuIRQLine;
        bool hasBattery;
        unsigned char regs[8];
        unsigned char * prgBuffer;
        unsigned char * chrBuffer;
        unsigned char * wramBuffer;
        unsigned char * ntBuffer;
        //Pointers to CPU & PPU memory spaces
        unsigned char ** cpuCartSpace;
        unsigned char ** ppuChrSpace;
        unsigned char ** ppuNTSpace;
    private:
        int chrSizeInBytes;
        int wramSizeInBytes;
        int prgSizeInBytes;
};

#endif // BOARD_HPP_INCLUDED
