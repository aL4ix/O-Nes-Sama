#ifndef MMC2_HPP_INCLUDED
#define MMC2_HPP_INCLUDED
#include "BasicMapper.hpp"

class MMC2 : public BasicMapper {
public:
    MMC2(CartIO& ioRef);
    ~MMC2();

    void writeCPU(int address, unsigned char val);
    unsigned char readPPU(int address);

    void inline clockCPU() { }
    void inline clockPPU() { }
    void loadSRAM(FILE* batteryFile);
    void saveSRAM(FILE* batteryFile);
    // bool loadState(FILE * file);
    // void saveState(FILE * file);

protected:
    int prgSizeMask;
    int chrSizeMask;
    void syncCHR();
    unsigned char prgBank;
    unsigned char chrBanks[4];
    unsigned char mirroring;
    unsigned char latch[2];
    void setNTMirroring();
};

#endif // MMC2_HPP_INCLUDED
