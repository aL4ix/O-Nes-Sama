#ifndef MMC1_HPP_INCLUDED
#define MMC1_HPP_INCLUDED

#include "../../Logging/Logger.hpp"
#include "BasicMapper.hpp"

class MMC1 : public BasicMapper {
public:
    MMC1(CartIO& ioRef);

    void writeCPU(int addr, unsigned char val);
    void clockCPU();
    void setNTMirroring();
    void sync();
    void loadSRAM(FILE* batteryFile);
    void saveSRAM(FILE* batteryFile);

private:
    int isPRGLarge = 0;
    int isCHRLarge = 0;
    int superPRGBank = 0;
    int chrSizeMask = 0;
    int prgSizeMask = 0;
    unsigned char control = 0xC;
    unsigned char shiftReg = 0b10000;
    unsigned char wRamBank = 0;
    unsigned char prgBank = 0;
    unsigned char chrBanks[2];
    unsigned countM2, lastCountM2 = 0;
};

#endif // MMC1_HPP_INCLUDED
