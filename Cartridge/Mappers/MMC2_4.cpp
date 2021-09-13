#include "MMC2_4.hpp"

MMC2::MMC2(CartIO & ioRef) : BasicMapper (ioRef){
    chrSizeMask = (io.iNESHeader.chrSize8k << 1) - 1;
    prgSizeMask = (io.iNESHeader.prgSize16k << 1) - 1;

    if (io.iNESHeader.mapperNo == 10){ // IF MMC4, emulate 8K of wRAM
        io.wRam = new unsigned char [0x2000];
        io.swapPRGRAM(0, 1);
    }

    io.swapPRGROM(8, 1, prgSizeMask - 2, io.prgBuffer, 0);
    io.swapPRGROM(8, 2, prgSizeMask - 1, io.prgBuffer, 0);
    io.swapPRGROM(8, 3, prgSizeMask, io.prgBuffer, 0);
}

void MMC2::writeCPU(int address, unsigned char val){
    BasicMapper::writeCPU(address, val);

    switch (address >> 12){
        case 0xA:
            prgBank = val;
            syncPRG();
            break;
        case 0xB: chrBanks[0] = val;
            break;
        case 0xC: chrBanks[1] = val;
            break;
        case 0xD: chrBanks[2] = val;
            break;
        case 0xE: chrBanks[3] = val;
            break;
        case 0xF:
            mirroring = val;
            setNTMirroring();
            break;
    }
}

void MMC2::syncPRG(){
    if (io.iNESHeader.mapperNo == 9)
        io.swapPRGROM(8, 0, prgBank & 0xF, io.prgBuffer, 0);
    else if (io.iNESHeader.mapperNo == 10)
        io.swapPRGROM(16, 0, prgBank & 0xF, io.prgBuffer, 0);
}

void MMC2::syncCHR(){
    if (!latch[0])
        io.swapCHR(4, 0, chrBanks[0] & chrSizeMask, io.chrBuffer);
    else
        io.swapCHR(4, 0, chrBanks[1] & chrSizeMask, io.chrBuffer);
    if (!latch[1])
        io.swapCHR(4, 1, chrBanks[2] & chrSizeMask, io.chrBuffer);
    else
        io.swapCHR(4, 1, chrBanks[3] & chrSizeMask, io.chrBuffer);
}

void MMC2::setNTMirroring() {
    if (mirroring & 1) //Horizontal
        io.switchHorMirroring();
    else //Vertical
        io.switchVerMirroring();
}

unsigned char MMC2::readPPU(int address){
    unsigned char ret = BasicMapper::readPPU(address);
    syncCHR();

    switch (address & 0x1FF0){
        case 0x0FD0: latch[0] = 0; break;
        case 0x0FE0: latch[0] = 1; break;
        case 0x1FD0: latch[1] = 0; break;
        case 0x1FE0: latch[1] = 1; break;
    }

    return ret;
}

void MMC2::saveSRAM(FILE * batteryFile){
    fwrite(io.wRam, sizeof(unsigned char), 0x2000, batteryFile);
}

void MMC2::loadSRAM(FILE * batteryFile){
    fread(io.wRam, sizeof(unsigned char), 0x2000, batteryFile);
}

void MMC2::loadState(FILE * file){
    fread(&prgBank  , sizeof(unsigned char *), 1, file);
    fread(chrBanks  , sizeof(unsigned char)  , 4, file);
    fread(&mirroring, sizeof(unsigned char *), 1, file);
    fread(latch     , sizeof(unsigned char)  , 2, file);

    if (io.wRam)
        fread(io.wRam, sizeof (unsigned char), 0x2000, file);
    if (io.iNESHeader.chrSize8k == 0)
        fread(io.chrBuffer, sizeof (unsigned char), 0x2000, file);
    fread(io.ntSystemRam, sizeof (unsigned char), 0x800, file);

    syncPRG();
    syncCHR();
    setNTMirroring();

}

void MMC2::saveState(FILE * file){

    fwrite(&prgBank  , sizeof(unsigned char *), 1, file);
    fwrite(chrBanks  , sizeof(unsigned char)  , 4, file);
    fwrite(&mirroring, sizeof(unsigned char *), 1, file);
    fwrite(latch     , sizeof(unsigned char)  , 2, file);

    if (io.wRam)
        fwrite(io.wRam, sizeof (unsigned char), 0x2000, file);
    if (io.iNESHeader.chrSize8k == 0)
        fwrite(io.chrBuffer, sizeof (unsigned char), 0x2000, file);
    fwrite(io.ntSystemRam, sizeof (unsigned char), 0x800, file);

}
