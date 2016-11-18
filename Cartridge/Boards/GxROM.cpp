#include "GxROM.hpp"

/*

GxROM Board (Discrete mappers)

- Supported 32K PRG switching and 8K CHR switching.
- This board had fixed mirroring, H/V. Implementation based on the image header.
- Did not support WRAM officially AFAIK, but we're providing 8K anyway.
- NROM and CNROM like mappers descend from this class.
- In our implementation, UxROM and AxROM descend from this class as well, since they share a lot of features.
*/

GxROM::GxROM(unsigned char * header) : Board(header) {}

void GxROM::init(){
    Board::init();
    /* Create NT & WRAM Memory */
    wramBuffer = new unsigned char [MapperUtils::_32K];
    printf("\nSize of wRam: %d", sizeof(wramBuffer));
    ntBuffer = new unsigned char [MapperUtils::_2K];
    /*All derived mappers have fixed mirroring so...*/
    if (!ntMirroring)
        MapperUtils::switchToHorMirroring(ntBuffer, ppuNTSpace);
    else
        MapperUtils::switchToVertMirroring(ntBuffer, ppuNTSpace);
    /* Initialize pointers to PRG, CHR and NT*/
    prg = &cpuCartSpace[4];
    MapperUtils::switchPRG16K(prgBuffer, prg, 0, 0);
    MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSize16K > 1);
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, 0);
    /*Synchronize any descendant mapper behavior*/
    sync();
}

unsigned char GxROM::read(int addr){
    switch (addr >> 12){
        case 0x4: case 0x5: /* Open Bus */
            return (addr & 0xFF00) >> 8;
        case 0x6: case 0x7:
            return wramBuffer[addr & 0x1FFF];
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            return prg[(addr - MapperUtils::_32K) >> 12][addr & 0xFFF];
    }
    return -1;
}

GxROM::~GxROM(){}
