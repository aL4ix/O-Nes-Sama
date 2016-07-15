#ifndef MMC2_HPP_INCLUDED
#define MMC2_HPP_INCLUDED
#include "Board.hpp"

class MMC2 : public Board {
    public:
        MMC2(unsigned char * header);
        void init();
        void sync();
        void syncCHR();
        virtual void syncPRG() = 0;
        ~MMC2();
        unsigned char read(int addr);
        void write(int addr, unsigned char val);
        void inline clockCPU() {}
        void setPPUAddress(int addr);
        void inline clockPPU() {}

    protected:
        int prgSizeMask;
        int chrSizeMask;
        unsigned char ** prg;
        unsigned char latch[2];
        void setNTMirroring();

};

#endif // MMC2_HPP_INCLUDED
