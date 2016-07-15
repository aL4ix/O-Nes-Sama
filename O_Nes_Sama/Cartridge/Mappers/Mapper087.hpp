#ifndef MAPPER087_HPP_INCLUDED
#define MAPPER087_HPP_INCLUDED

#include "../Boards/GxROM.hpp"

class Mapper087: public GxROM{
    public:
        Mapper087(unsigned char * header);
        void sync();
        void write(int addr, unsigned char val);
};

#endif // MAPPER087_HPP_INCLUDED
