#ifndef NSFMAPPER_H_INCLUDED
#define NSFMAPPER_H_INCLUDED

#include "../../CPU.h"
#include "../CartIO.hpp"
#include "../Mappers/MemoryMapper.h"

class NSFMapper : public MemoryMapper {
public:
    NSFMapper(CartIO& ioRef);
    unsigned char readCPU(int address);
    unsigned char readPPU(int address);
    void writeCPU(int address, unsigned char val);
    void writePPU(int address, unsigned char val);
    void clockCPU();
    void clockPPU();
    void loadSRAM(FILE* batteryFile);
    void saveSRAM(FILE* batteryFile);
    void setCPU(CPU* cpu);

private:
    CPU* cpu;
};

#endif // NSFMAPPER_H_INCLUDED
