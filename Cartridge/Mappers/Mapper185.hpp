#ifndef MAPPER185_HPP_INCLUDED
#define MAPPER185_HPP_INCLUDED

#include "../Boards/GxROM.hpp"

class Mapper185: public GxROM{
    public:
        Mapper185(unsigned char * header);
        void sync();
        void write(int addr, unsigned char val);
                bool loadState(FILE * file);
        void saveState(FILE * file);
    private:
        unsigned char badCHR[0x400] = {0xFF};
        unsigned char chrBank;

};

#endif // MAPPER185_HPP_INCLUDED
