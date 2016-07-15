#ifndef MAPPER007_HPP_INCLUDED
#define MAPPER007_HPP_INCLUDED


#include "../Boards/GxROM.hpp"

class Mapper007 : public GxROM{
    public:
        Mapper007(unsigned char * header);
        void write(int addr, unsigned char val);
    private:
        void sync();

};

#endif // MAPPER007_HPP_INCLUDED
