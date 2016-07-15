#ifndef MAPPER002_HPP_INCLUDED
#define MAPPER002_HPP_INCLUDED

#include "../Boards/GxROM.hpp"

class Mapper002 : public GxROM{
    public:
        Mapper002(unsigned char * header);
        void write(int addr, unsigned char val);
    private:
        void sync();

};

#endif // MAPPER002_HPP_INCLUDED
