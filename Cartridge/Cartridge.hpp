#ifndef CARTRIDGE_HPP_INCLUDED
#define CARTRIDGE_HPP_INCLUDED

#include "CartIO.hpp"
#include "Mappers/MemoryMapper.h"
#include "zlib.h"
#include <string>

class Cartridge {
public:
    // BoardData bData;
    Cartridge(std::string fileName);
    ~Cartridge();
    CartIO io;
    MemoryMapper* mapper;
    int hasBattery;
    void loadRomFile();
    void initMapper();
    void saveState(FILE* file);
    bool loadState(FILE* file);
    std::string getBaseRomName(std::string tmpRomName);

private:
    std::string fileNameBase;
    std::string romFileName;
    MemoryMapper* createMapper(int mapperNo);
    std::string batteryPath = "Battery";
};

#endif // CARTRIDGE_HPP_INCLUDED
