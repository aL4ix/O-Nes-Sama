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
        bool loadState(FILE * file);
        void saveState(FILE * file);
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);
    protected:
        unsigned char ** prg;
        int oldPPUA12;
        //Registers
        unsigned char commandRegs[8];
        unsigned char bankSelect;
        unsigned char mirroring;
        unsigned char irqLatch;
        unsigned char irqReload;
        unsigned char irqCounter;
        unsigned char irqEnable;

        int altBehavior = 0;

        int prgSizeMask;
        int chrSizeMask;

        int edgeCount;
        void clockIRQCounter();
        void syncPRG();
        void syncCHR();
        void setNTMirroring();
        void clockIRQCounterFake();
};

#endif // MMC3_HPP_INCLUDED
