#ifndef MAPPER185_HPP_INCLUDED
#define MAPPER185_HPP_INCLUDED

#include "../Boards/GxROM.hpp"

class Mapper185: public GxROM{
    public:
        Mapper185(unsigned char * header);
        void sync();
        void write(int addr, unsigned char val);
    private:
        unsigned char badCHR[0x400] = {0xFF};
};

#endif // MAPPER185_HPP_INCLUDED
