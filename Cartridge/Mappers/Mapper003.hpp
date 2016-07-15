#ifndef MAPPER003_HPP_INCLUDED
#define MAPPER003_HPP_INCLUDED
#include "../Boards/GxROM.hpp"

class Mapper003: public GxROM{
    public:
        Mapper003(unsigned char * header);
        void sync();
        void write(int addr, unsigned char val);
};

#endif // MAPPER003_HPP_INCLUDED
