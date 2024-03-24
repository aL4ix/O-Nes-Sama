#include "MMC2_4.hpp"

MMC2::MMC2(CartIO& ioRef)
    : BasicMapper(ioRef)
{
    chrSizeMask = (io.iNESHeader.chrSize8k << 1) - 1;
    prgSizeMask = (io.iNESHeader.prgSize16k << 1) - 1;

    if (io.iNESHeader.mapperNo == 10) { // IF MMC4, emulate 8K of wRAM
        io.wRam = new unsigned char[0x2000];
        io.switch8K(0, 0, io.wRam, io.wRamSpace);
    }

    io.switch8K(1, prgSizeMask - 2, io.prgBuffer, io.prgSpace);
    io.switch8K(2, prgSizeMask - 1, io.prgBuffer, io.prgSpace);
    io.switch8K(3, prgSizeMask, io.prgBuffer, io.prgSpace);
}

void MMC2::writeCPU(int address, unsigned char val)
{
    BasicMapper::writeCPU(address, val);

    switch (address >> 12) {
    case 0xA:
        if (io.iNESHeader.mapperNo == 9)
            io.switch8K(0, val & 0xF, io.prgBuffer, io.prgSpace);
        else if (io.iNESHeader.mapperNo == 10)
            io.switch16K(0, val & 0xF, io.prgBuffer, io.prgSpace);
        break;
    case 0xB:
        chrBanks[0] = val;
        break;
    case 0xC:
        chrBanks[1] = val;
        break;
    case 0xD:
        chrBanks[2] = val;
        break;
    case 0xE:
        chrBanks[3] = val;
        break;
    case 0xF:
        mirroring = val;
        setNTMirroring();
        break;
    }
}

void MMC2::syncCHR()
{
    if (!latch[0])
        io.switch4K(0, chrBanks[0] & chrSizeMask, io.chrBuffer, io.chrSpace);
    else
        io.switch4K(0, chrBanks[1] & chrSizeMask, io.chrBuffer, io.chrSpace);
    if (!latch[1])
        io.switch4K(1, chrBanks[2] & chrSizeMask, io.chrBuffer, io.chrSpace);
    else
        io.switch4K(1, chrBanks[3] & chrSizeMask, io.chrBuffer, io.chrSpace);
}

void MMC2::setNTMirroring()
{
    if (mirroring & 1) // Horizontal
        io.switchHorMirroring();
    else // Vertical
        io.switchVerMirroring();
}

unsigned char MMC2::readPPU(int address)
{
    unsigned char ret = BasicMapper::readPPU(address);
    syncCHR();

    switch (address & 0x1FF0) {
    case 0x0FD0:
        latch[0] = 0;
        break;
    case 0x0FE0:
        latch[0] = 1;
        break;
    case 0x1FD0:
        latch[1] = 0;
        break;
    case 0x1FE0:
        latch[1] = 1;
        break;
    }

    return ret;
}

void MMC2::saveSRAM(FILE* batteryFile)
{
    fwrite(io.wRam, 0x8000, 1, batteryFile);
}

void MMC2::loadSRAM(FILE* batteryFile)
{
    fread(io.wRam, 0x8000, 1, batteryFile);
}
