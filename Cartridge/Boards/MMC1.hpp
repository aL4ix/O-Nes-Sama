#ifndef MMC1_HPP_INCLUDED
#define MMC1_HPP_INCLUDED

#include "Board.hpp"

class MMC1 : public Board {
    public:
        MMC1(unsigned char * header);
        void init();
        ~MMC1();
        unsigned char read(int addr);
        void setPPUAddress(int addr){}
        void clockCPU();
        void inline clockPPU() {}
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);
    protected:
        unsigned char ** prg;
        int countM2;
        int lastCountM2;
        int addr;
        int prgSizeMask;
        int chrSizeMask;
        unsigned char data;
        void pushInShiftReg();
        void loadRegWrite();
        void sync();
        virtual void chrBank0Write() = 0;
        virtual void chrBank1Write() = 0;
        virtual void prgBankWrite() = 0;
        void setNTMirroring();
};

#endif // MMC1_HPP_INCLUDED
