#ifndef MMC3_HPP_INCLUDED
#define MMC3_HPP_INCLUDED

#include "Board.hpp"

class MMC3 : public Board {
    public:
        MMC3(unsigned char * header);
        void init();
        ~MMC3();
        unsigned char read(int addr);
        void write(int addr, unsigned char val);
        void setPPUAddress(int addr);
        void sync();
        void clockCPU();
        void clockPPU();
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);
    protected:
        unsigned char ** prg;
        int * sl;
        int * tick;
        int oldPPUA12;
        int irqCounter;
        int irqReload;
        int irqEnable;
        int irqLatch;
        int irqFlag;
        int A12EdgeTimer = 0;
        int altBehavior = 0;
        int lastEdge;
        int addr;
        int prgSizeMask;
        int chrSizeMask;
        int commandRegs[8];
        int edgeCount;
        void clockIRQCounter();
        void syncPRG();
        void syncCHR();
        void setNTMirroring();
        void clockIRQCounterFake();
};

#endif // MMC3_HPP_INCLUDED