#ifndef MAPPER011_HPP_INCLUDED
#define MAPPER011_HPP_INCLUDED
#include "../Boards/GxROM.hpp"

class Mapper011: public GxROM{
    public:
        Mapper011(unsigned char * header);
        void sync();
        void write(int addr, unsigned char val);
};

#endif // MAPPER011_HPP_INCLUDED
