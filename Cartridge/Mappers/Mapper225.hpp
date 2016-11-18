#ifndef MAPPER225_HPP_INCLUDED
#define MAPPER225_HPP_INCLUDED

#include "../Boards/GxROM.hpp"

class Mapper225 : public GxROM {
    private:
        int hiBit;
        int mirr;
        int prgMode;
        int prgPos;
        int prgReg;
        int chrReg;
        int lastAddr;
        unsigned char ramReg[4] = {0xF};
        unsigned char badCHR[0x400] = {0xFF};
    public:
        Mapper225(unsigned char * header);
        void write(int addr, unsigned char val);
        unsigned char read(int addr);
        void sync();
        void init();
        bool loadState(FILE * file);
        void saveState(FILE * file);
};


#endif // MAPPER225_HPP_INCLUDED
