#ifndef MAPPER000_HPP_INCLUDED
#define MAPPER000_HPP_INCLUDED

#include "MemoryMapper.h"

class BasicMapper : public MemoryMapper{

    public:
        unsigned char readCPU(int address);
        unsigned char readPPU(int address);
        void writeCPU (int address, unsigned char val);
        void writePPU (int address, unsigned char val);
        inline void clockCPU() {};
        inline void clockPPU() {};
        void loadSRAM(FILE * batteryFile) {}
        void saveSRAM(FILE * batteryFile) {}
        BasicMapper(CartIO &ioRef);
        ~BasicMapper();
};


class Mapper000 : public BasicMapper{
        Mapper000(CartIO &ioRef);
        ~Mapper000();
};

#endif // MAPPER000_HPP_INCLUDED
