#ifndef CARTRIDGE_HPP_INCLUDED
#define CARTRIDGE_HPP_INCLUDED

#include "CartIO.hpp"
#include "Mappers/MemoryMapper.h"
#include <string>
#include "zlib.h"

class Cartridge{
    public:
        Cartridge(std::string fileName);
        ~Cartridge();
        CartIO io;
        MemoryMapper * mapper;
        int hasBattery;
        void loadRomFile();
        void initMapper();

    private:
        std::string getBaseRomName(std::string tmpRomName);
        std::string fileNameBase;
        std::string romFileName;
        MemoryMapper * createMapper(int mapperNo);

};

#endif // CARTRIDGE_HPP_INCLUDED
