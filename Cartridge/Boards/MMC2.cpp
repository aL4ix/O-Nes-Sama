#include "MMC2.hpp"

MMC2::MMC2(unsigned char * header) : Board(header){}

void MMC2::init(){
    Board::init();
    ntBuffer = new unsigned char [MapperUtils::_2K];
    wramBuffer = new unsigned char [MapperUtils::_32K];
    prg = &cpuCartSpace[4];
    chrSizeMask = (chrSize8K << 1) - 1;
    sync();
}

unsigned char MMC2::read(int addr){
    switch (addr >> 12){
        case 0x6: case 0x7: /* WRAM Space */
            return wramBuffer[addr & 0x1FFF];
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            return prg[(addr & 0x7FFF) >> 12][addr & 0xFFF];
    }
    return -1;
}

void MMC2::write(int addr, unsigned char val){
    switch (addr >> 12){
        case 0x6: case 0x7: /* WRAM Space */
            wramBuffer[addr & 0x1FFF] = val;
            break;
        case 0xA:
            prgBank = val;
            syncPRG();
            break;
        case 0xB: chrBanks[0] = val; break;
        case 0xC: chrBanks[1] = val; break;
        case 0xD: chrBanks[2] = val; break;
        case 0xE: chrBanks[3] = val; break;
        case 0xF:
            mirroring = val;
            setNTMirroring();
            break;
    }
}

void MMC2::sync(){
    syncPRG();
    syncCHR();
    setNTMirroring();
}

void MMC2::syncCHR(){
    if (!latch[0])
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 0, chrBanks[0] & chrSizeMask);
    else
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 0, chrBanks[1] & chrSizeMask);
    if (!latch[1])
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 1, chrBanks[2] & chrSizeMask);
    else
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 1, chrBanks[3] & chrSizeMask);
}




void MMC2::setNTMirroring() {
    if (mirroring & 1) //Horizontal
        MapperUtils::switchToHorMirroring(ntBuffer, ppuNTSpace);
    else //Vertical
        MapperUtils::switchToVertMirroring(ntBuffer, ppuNTSpace);
}

void MMC2::setPPUAddress(int addr){
    syncCHR();
    switch (addr & 0x3FF0){
        case 0x0FD0: latch[0] = 0; break;
        case 0x0FE0: latch[0] = 1; break;
        case 0x1FD0: latch[1] = 0; break;
        case 0x1FE0: latch[1] = 1; break;
    }
}

bool MMC2::loadState(FILE * file){
    int ret = Board::loadState(file);
    prgBank     = tempR[0];
    chrBanks[0] = tempR[1];
    chrBanks[1] = tempR[2];
    chrBanks[2] = tempR[3];
    chrBanks[3] = tempR[4];
    mirroring   = tempR[5];
    latch[0]    = tempR[6];
    latch[1]    = tempR[7];
    syncPRG();
    return ret;
}

void MMC2::saveState(FILE * file){
    tempR[0] = prgBank;
    tempR[1] = chrBanks[0];
    tempR[2] = chrBanks[1];
    tempR[3] = chrBanks[2];
    tempR[4] = chrBanks[3];
    tempR[5] = mirroring;
    tempR[6] = latch[0];
    tempR[7] = latch[1];
    Board::saveState(file);
}
