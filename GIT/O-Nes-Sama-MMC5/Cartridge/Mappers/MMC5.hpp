#ifndef MMC5_HPP_INCLUDED
#define MMC5_HPP_INCLUDED

#include "BasicMapper.hpp"

struct PrgBanks{
    unsigned char romToggle;
    unsigned char bank;
};

class MMC5 : public BasicMapper{
    public:
        MMC5(CartIO &ioRef);
        unsigned char readCPU(int address);
        unsigned char readPPU(int address);
        void writeCPU(int address, unsigned char val);
        void writePPU(int address, unsigned char val);

        //Clocking functions and variables
        bool isFetchingSpr;
        void clockIRQ();
        void clockPPU();
        void clockCPU();

        //sRAM functions
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);

    private:
        //PPU Access handlers
         unsigned char (MMC5::*ppuReadFunc)(int address);
         unsigned char readPPUExRam1(int address);
         unsigned char basicReadPPU(int address);

        //Gen vars
        int curChrSet;
        int is8x16;
        int ntAddr;
        unsigned char tilebank;

        //Registers
        unsigned char prgMode;
        unsigned char chrMode;
        unsigned char wRamProt1;
        unsigned char wRamProt2;
        unsigned char exRamMode;
        unsigned char ntMapping;
        unsigned char fillModeTile;
        unsigned char fillModeColor;
        unsigned char prgRamBank;
        PrgBanks prgBanks[4];
        unsigned char chrSelA[8];
        unsigned char chrSelB[4];
        unsigned char chrUpperBank;
        unsigned char vSplitMode;
        unsigned char vSplitScroll;
        unsigned char vSplitBank;
        int irqCounter;
        unsigned char irqScanline;
        unsigned char irqEnabled;
        unsigned char irqStatus;
        unsigned char multiplier[2];

        //Extra ram and the like
        unsigned char exRam[0x400] = {0xFF};
        unsigned char fillMode[0x400];
        unsigned char zeroes[0x400] = {0};

        void sync();
        void syncPRG();
        void syncCHR();
        void syncCHRA();
        void syncCHRB();
        void syncNT();

        unsigned char * setNTSource(int ntMode);

};

#endif // MMC5_HPP_INCLUDED
