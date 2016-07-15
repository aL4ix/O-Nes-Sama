#ifndef MAPPER071_HPP_INCLUDED
#define MAPPER071_HPP_INCLUDED

#include "../Boards/GxROM.hpp"

class Mapper071: public GxROM{
    public:
        Mapper071(unsigned char * header);
        void sync();
        void write(int addr, unsigned char val);
};

#endif // MAPPER071_HPP_INCLUDED
