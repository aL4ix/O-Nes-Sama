#ifndef MAPPER010_HPP_INCLUDED
#define MAPPER010_HPP_INCLUDED

#include "../Boards/MMC2.hpp"

class Mapper010 : public MMC2{
    public:
        Mapper010(unsigned char * header);
        ~Mapper010();
        void init();
        void syncPRG();
        void saveSRAM(FILE * batteryFile);
        void loadSRAM(FILE * batteryFile);
};

#endif // MAPPER010_HPP_INCLUDED
