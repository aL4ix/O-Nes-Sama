#ifndef MMC1_HPP_INCLUDED
#define MMC1_HPP_INCLUDED

#include "BasicMapper.hpp"
class MMC1 : public BasicMapper {
    private:
        int isPRGLarge;
        int isCHRLarge;
        int superPRGBank;
        int chrSizeMask;
        int prgSizeMask;
        unsigned char control;
        unsigned char shiftReg;
        unsigned char wRamBank;
        unsigned char prgBank;
        unsigned char chrBanks[2] = {0};
        unsigned countM2, lastCountM2;
    public:
        MMC1(CartIO &ioRef);
        ~MMC1();
        void writeCPU(int addr, unsigned char val);
        void clockCPU();
        void setNTMirroring();
        void sync();
        void loadSRAM(FILE * batteryFile);
        void saveSRAM(FILE * batteryFile);
};

#endif // MMC1_HPP_INCLUDED
