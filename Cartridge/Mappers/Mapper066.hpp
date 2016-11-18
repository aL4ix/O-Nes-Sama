#ifndef MAPPER066_HPP_INCLUDED
#define MAPPER066_HPP_INCLUDED

#include "../Boards/GxROM.hpp"
class Mapper066 : public GxROM{
    public:
        Mapper066(unsigned char * header);
        void write(int addr, unsigned char val);
        bool loadState(FILE * file);
        void saveState(FILE * file);
    private:
        unsigned char banksReg;
        void sync();
};

#endif // MAPPER066_HPP_INCLUDED
