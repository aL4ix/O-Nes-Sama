#ifndef MAPPER000_H_INCLUDED
#define MAPPER000_H_INCLUDED

#include "../Boards/GxROM.hpp"

class Mapper000 : public GxROM {
    private:

    public:
        Mapper000(unsigned char * header);
        void write(int addr, unsigned char val);
        void sync() {}
        bool loadState(FILE * file);
        void saveState(FILE * file);

};

#endif // MAPPER000_H_INCLUDED
