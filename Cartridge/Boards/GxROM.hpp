#ifndef GXROM_H_INCLUDED
#define GXROM_H_INCLUDED
#include "Board.hpp"

class GxROM : public Board{
    protected:
        unsigned char ** prg;

    public:
        GxROM(unsigned char * header);
        virtual void sync() = 0;
        void init();
        ~GxROM();
        unsigned char read(int addr);
        void setPPUAddress(int addr) {}
        void inline saveSRAM(FILE * batteryFile) {}
        void inline loadSRAM(FILE * batteryFile) {}
        void inline clockCPU() {}
        void inline clockPPU() {}
        //void __always_inline watchPPUAddress(int addr, int sl, int dot) {}
};

#endif // GXROM_H_INCLUDED
