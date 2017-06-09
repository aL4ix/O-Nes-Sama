#ifndef MMC5_HPP_INCLUDED
#define MMC5_HPP_INCLUDED

#include "BasicMapper.hpp"

class MMC5 : public BasicMapper {

    public:

        MMC5(CartIO & ioRef);
        ~MMC5();
        unsigned char readCPU(int address);
        unsigned char readPPU(int address);
        void writeCPU(int address, unsigned char val);
        void writePPU(int address, unsigned char val);
        void sync();
        void clockCPU();
        void clockPPU();

        void genericPPUEvent();
        /*bool loadState(FILE * file);
        void saveState(FILE * file);
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);*/
    protected:
        unsigned char * ntBuffer;
        //Registers
        int prgMode;
        int chrMode;
        int prgBanks[4];
        int wRamProtect[2];
        int exRamMode;
        int nameTableMapping;
        int fillModeTile;
        int fillModeColor;
        int wRAMBankMode;
        int wRAMBanks[4];
        int chrSelects[12];
        int chrLowerBanks[4];
        int chrUpperBank;
        int vertSplitMode;
        int vertSplitScroll;
        int vertSplitBank;
        int IRQScanLine;
        int IRQStatusRd;
        int IRQPending;
        int IRQEnabled;
        unsigned char IRQCounter;
        int mltprFactors[2];
        unsigned char zeroes[0x400] = {0};
        unsigned char expansionRam[0x400]; //1 Kilobyte
        unsigned char fillMode[0x400]; //1 Kilobyte
        //------------------------------------------------------//
        int prgMask32K;
        int prgMask16K;
        int prgMask8K;
        int lastChrSelectAddr;
    private:
        void switchCHRSetA();
        void switchCHRSetB();
        void syncPRG();
        void syncNT();
        void clockIRQCounter();
        unsigned char * setNTSource(int ntMode);
};

#endif // MMC5_HPP_INCLUDED
