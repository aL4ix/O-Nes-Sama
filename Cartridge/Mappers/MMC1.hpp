#ifndef MMC1_HPP_INCLUDED
#define MMC1_HPP_INCLUDED

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
};

#endif // MMC1_HPP_INCLUDED
