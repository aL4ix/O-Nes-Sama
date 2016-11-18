#ifndef MAPPER003_HPP_INCLUDED
#define MAPPER003_HPP_INCLUDED
#include "../Boards/GxROM.hpp"

class Mapper003: public GxROM{
    public:
        Mapper003(unsigned char * header);
        void write(int addr, unsigned char val);
    private:
        void sync();
        unsigned char chrBank;
        bool loadState(FILE * file);
        void saveState(FILE * file);
};

#endif // MAPPER003_HPP_INCLUDED
