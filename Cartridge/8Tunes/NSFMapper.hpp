#ifndef NSFMAPPER_HPP_INCLUDED
#define NSFMAPPER_HPP_INCLUDED

#include "../../CPU.hpp"
#include "../CartIO.hpp"
#include "../Mappers/MemoryMapper.hpp"

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

#endif // NSFMAPPER_HPP_INCLUDED
