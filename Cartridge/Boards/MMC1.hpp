#ifndef MMC1_REV2_HPP_INCLUDED
#define MMC1_REV2_HPP_INCLUDED

#include "Board.hpp"

class MMC1 : public Board {
    public:
        MMC1(unsigned char * header);
        void init();
        ~MMC1();
        unsigned char read(int addr);
        void write(int addr, unsigned char val);
        void setPPUAddress(int addr){}
        void clockCPU();
        void clockPPU(){}
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);
        bool loadState(FILE * file);
        void saveState(FILE * file);
    protected:
        unsigned char ** prg;
        int prgSizeMask = 0;
        int chrSizeMask = 0;
        /* Registers */
        unsigned char load;
        unsigned char control;
        unsigned char chrBanks[2];
        unsigned char prgBank;
        void sync();
        void setNTMirroring();
};

#endif // MMC1_REV2_HPP_INCLUDED
