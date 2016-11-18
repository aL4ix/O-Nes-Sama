#ifndef MAPPER007_HPP_INCLUDED
#define MAPPER007_HPP_INCLUDED


#include "../Boards/GxROM.hpp"

class Mapper007 : public GxROM{
    public:
        Mapper007(unsigned char * header);
        void write(int addr, unsigned char val);
    private:
        unsigned char prgMirrBank;
        void sync();
        bool loadState(FILE * file);
        void saveState(FILE * file);

};

#endif // MAPPER007_HPP_INCLUDED
