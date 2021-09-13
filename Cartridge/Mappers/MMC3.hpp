#ifndef MMC3_HPP_INCLUDED
#define MMC3_HPP_INCLUDED

#include "BasicMapper.hpp"

class MMC3 : public BasicMapper {
    public:
        MMC3(CartIO &ioRef);
        ~MMC3();
        void writeCPU(int addr, unsigned char val);
        void sync();
        void clockCPU();
        void clockPPU();
        //unsigned char readPPU(int address);
        //void writePPU(int address, unsigned char val);
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);
        void loadState(FILE * file);
        void saveState(FILE * file);
    protected:
        int chrAccess;
        int ppuA12;
        int oldPPUA12;
        int needsMCACC;
        int edgeCount;
        int edgeCount_old;
        int cycleDelay;
        int prgSizeMask;
        int chrSizeMask;
        //Registers
        unsigned char commandRegs[8];
        unsigned char bankSelect;
        unsigned char mirroring;
        unsigned char irqLatch;
        unsigned char irqReload;
        unsigned char irqCounter;
        unsigned char irqEnable;



        void clockIRQCounter();
        void syncPRG();
        void syncCHR();
        void setNTMirroring();
        void clockIRQCounterFake();
};

#endif // MMC3_HPP_INCLUDED
