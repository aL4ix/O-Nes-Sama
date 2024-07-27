#ifndef MEMORYMAPPER_HPP_INCLUDED
#define MEMORYMAPPER_HPP_INCLUDED

#include "../../CPUIO.hpp"
#include "../CartDB.hpp"
#include "../CartIO.hpp"

class MemoryMapper {
public:
    CartIO& io;
    CPUIO* cpuIO;
    MemoryMapper(CartIO&);
    virtual ~MemoryMapper();
    virtual unsigned char readCPU(int address) = 0;
    virtual unsigned char readPPU(int address) = 0;
    virtual void writeCPU(int address, unsigned char val) = 0;
    virtual void writePPU(int address, unsigned char val) = 0;
    virtual void clockCPU() = 0;
    virtual void clockPPU() = 0;
    virtual void loadSRAM(FILE* batteryFile) = 0;
    virtual void saveSRAM(FILE* batteryFile) = 0;
};

#endif // MEMORYMAPPER_HPP_INCLUDED
