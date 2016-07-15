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
            regs[0] = val;
            syncPRG();
            break;
        case 0xB: regs[1] = val; break;
        case 0xC: regs[2] = val; break;
        case 0xD: regs[3] = val; break;
        case 0xE: regs[4] = val; break;
        case 0xF:
            regs[5] = val;
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
    if (!regs[6])
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 0, regs[1] & chrSizeMask);
    else
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 0, regs[2] & chrSizeMask);
    if (!regs[7])
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 1, regs[3] & chrSizeMask);
    else
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 1, regs[4] & chrSizeMask);
}

void MMC2::setNTMirroring() {
    if (regs[5] & 1) //Horizontal
        MapperUtils::switchToHorMirroring(ntBuffer, ppuNTSpace);
    else //Vertical
        MapperUtils::switchToVertMirroring(ntBuffer, ppuNTSpace);
}

void MMC2::setPPUAddress(int addr){
    syncCHR();
    switch (addr & 0x3FF0){
        case 0x0FD0: regs[6] = 0; break;
        case 0x0FE0: regs[6] = 1; break;
        case 0x1FD0: regs[7] = 0; break;
        case 0x1FE0: regs[7] = 1; break;
    }
}
