#ifndef MAPPER009_HPP_INCLUDED
#define MAPPER009_HPP_INCLUDED

#include "../Boards/MMC2.hpp"

class Mapper009 : public MMC2{
    public:
        Mapper009(unsigned char * header);
        ~Mapper009();
        void init();
        void syncPRG();
        void saveSRAM(FILE * batteryFile) {}
        void loadSRAM(FILE * batteryFile) {}
};

#endif // MAPPER009_HPP_INCLUDED
