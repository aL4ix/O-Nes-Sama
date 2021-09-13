#ifndef MMC5_HPP_INCLUDED
#define MMC5_HPP_INCLUDED
#include "BasicMapper.hpp"

class MMC5 : public BasicMapper{
    public:
        MMC5(CartIO &ioRef);
        unsigned char readCPU(int address);
        unsigned char readPPU(int address);
        void writeCPU(int address, unsigned char val);
        void writePPU(int address, unsigned char val);

        //Clocking functions and variables
        bool isFetchingSpr;
        bool isFetchingBG;
        void clockIRQ();
        void clockPPU();
        void clockCPU();
        void syncCHR();

        //sRAM functions
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);
        void saveState(FILE * file);
        void loadState(FILE * file);

    private:
        //PPU Access handlers
         unsigned char (MMC5::*ppuReadFunc)(int address);
         unsigned char readPPUExAttr(int address);
         unsigned char readPPUVSplit(int address);
         unsigned char basicReadPPU(int address);

        //Gen vars
        int curChrSet;
        int is8x16;
        int ntAddr;

        int lastAddr;
        int slDetect;
        int curTile;
        int isFetchingNT;
        int isSplitArea;
        int vScroll;

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
        unsigned char prgBanks[4];
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
        void syncCHRA();
        void syncCHRB();
        void syncNT();
        void onPPUFetch();
        unsigned char * setNTSource(int ntMode);

};

#endif // MMC5_HPP_INCLUDED
