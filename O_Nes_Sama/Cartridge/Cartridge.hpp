#ifndef CARTRIDGE_HPP_INCLUDED
#define CARTRIDGE_HPP_INCLUDED

#include "Boards/Board.hpp"
#include "Boards/GxROM.hpp"
#include "Boards/MMC3.hpp"
#include "Mappers/Mapper000.hpp"
#include "Mappers/Mapper001.hpp"
#include "Mappers/Mapper002.hpp"
#include "Mappers/Mapper003.hpp"
#include "Mappers/Mapper007.hpp"
#include "Mappers/Mapper009.hpp"
#include "Mappers/Mapper010.hpp"
#include "Mappers/Mapper011.hpp"
#include "Mappers/Mapper066.hpp"
#include "Mappers/Mapper071.hpp"
#include "Mappers/Mapper087.hpp"
#include "Mappers/Mapper185.hpp"
#include "Mappers/Mapper225.hpp"

#include <string>

class Cartridge{
    public:
        //BoardData bData;
        Cartridge(std::string fileName);
        ~Cartridge();
        Board * mapper;
        int hasBattery;
        void loadRomFile();
        void initMapper();
        void saveState(FILE * file);
        bool loadState(FILE * file);

    private:
        std::string fileNameBase;
        std::string romFileName;
        Board * createMapper(int mapperNo, unsigned char * header);

};

#endif // CARTRIDGE_HPP_INCLUDED
