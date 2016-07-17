#include "MMC3.hpp"
#include <stdlib.h>

MMC3::MMC3(unsigned char * header) : Board (header){}

void MMC3::init(){
    Board::init();
    oldPPUA12 = 0;
    edgeCount = 0;
    ntBuffer = new unsigned char [MapperUtils::_2K];
    wramBuffer = new unsigned char [MapperUtils::_32K];
    prgSizeMask = (prgSize16K << 1) - 1;
    chrSizeMask = (chrSize8K << 3) - 1;
    prg = &cpuCartSpace[4];
    commandRegs[6] = 0;
    commandRegs[7] = 1;
    MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, 0);
    sync();
}
unsigned char MMC3::read(int addr){
    unsigned char ret = -1;
    switch (addr >> 12){
        case 0x6: case 0x7:
            ret = wramBuffer[addr & 0x1FFF];
            break;
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            ret = prg[(addr & 0x7FFF) >> 12][addr & 0xFFF];
            break;
    }
    return ret;
}

void MMC3::write(int addr, unsigned char val){

    switch (addr >> 12){
        case 0x6: case 0x7:
            wramBuffer[addr & 0x1FFF] = val;
            break;
        case 0x8: case 0x9:
            if (addr & 1)
                commandRegs[regs[0] & 7] = val;
            else
                regs[0] = val;
            break;
        case 0xA: case 0xB:
            if (addr & 1){/* put ram protect logic here*/}
            else
                regs[2] = val;
            break;
        case 0xC: case 0xD:
            if (addr & 1){
                irqCounter = 0;
                irqReload = 1;
            }
            else{
                irqLatch = val;
            }
            break;
        case 0xE: case 0xF:
            irqEnable = addr & 1;
            if (!irqEnable)
                *cpuIRQLine = 0;
            break;
    }
    sync();
}
void MMC3::syncPRG(){
    bool PRGMode = regs[0] & 0x40;
    MapperUtils::switchPRG8K(prgBuffer, prg, (!PRGMode) << 1, prgSizeMask - 1);
    MapperUtils::switchPRG8K(prgBuffer, prg, PRGMode << 1, commandRegs[6] & prgSizeMask);
    MapperUtils::switchPRG8K(prgBuffer, prg, 1, commandRegs[7] & prgSizeMask);
    MapperUtils::switchPRG8K(prgBuffer, prg, 3, prgSizeMask);
}

void MMC3::syncCHR(){
    bool chrMode = (regs[0] & 0x80);
    //printf ("\nchr mode %x", chrMode);
    MapperUtils::switchCHR1K(chrBuffer, ppuChrSpace, (chrMode  << 2) | 0, commandRegs[0] & 0xFE);
    MapperUtils::switchCHR1K(chrBuffer, ppuChrSpace, (chrMode  << 2) | 1, commandRegs[0] | 1);
    MapperUtils::switchCHR1K(chrBuffer, ppuChrSpace, (chrMode  << 2) | 2, commandRegs[1] & 0xFE);
    MapperUtils::switchCHR1K(chrBuffer, ppuChrSpace, (chrMode  << 2) | 3, commandRegs[1] | 1);
    MapperUtils::switchCHR1K(chrBuffer, ppuChrSpace, (!chrMode << 2) | 0, commandRegs[2] & chrSizeMask);
    MapperUtils::switchCHR1K(chrBuffer, ppuChrSpace, (!chrMode << 2) | 1, commandRegs[3] & chrSizeMask);
    MapperUtils::switchCHR1K(chrBuffer, ppuChrSpace, (!chrMode << 2) | 2, commandRegs[4] & chrSizeMask);
    MapperUtils::switchCHR1K(chrBuffer, ppuChrSpace, (!chrMode << 2) | 3, commandRegs[5] & chrSizeMask);
}

void MMC3::sync(){
    syncPRG();
    syncCHR();
    setNTMirroring();
}

void MMC3::clockIRQCounter(){
    int lastCount = irqCounter;
    if (!irqCounter || irqReload){
        irqCounter = irqLatch;
    }
    else{
        irqCounter--;
    }
    if (!irqCounter && irqEnable){
        *cpuIRQLine = 1;
        //printf ("\n%d, %d", *ppuStatus.sline, *ppuStatus.tick);
    }

    irqReload = 0;

}

void MMC3::setNTMirroring(){
    if(regs[2] & 1)
        MapperUtils::switchToHorMirroring(ntBuffer, ppuNTSpace);
    else
        MapperUtils::switchToVertMirroring(ntBuffer, ppuNTSpace);
}

void inline MMC3::clockCPU(){}

void inline MMC3::clockPPU(){
    //if (!(ppuAddress & 0x1000)) {
    //    edgeCount++;
    //}
    //else {
    //    if (edgeCount)printf ("\n%d", edgeCount);
    //    edgeCount = 0;
    //}
}

inline void MMC3::setPPUAddress(int addr){
    ppuAddress = addr;
    int ppuA12 = (addr & 0x1000) >> 12;
    if (!oldPPUA12 && ppuA12){
        if (edgeCount > 13){
            clockIRQCounter();
        }
    }
    oldPPUA12 = ppuA12;

    if (!ppuA12){
        edgeCount++;
    }
    else{
        edgeCount = 0;
    }
}

void MMC3::saveSRAM(FILE *){}
void MMC3::loadSRAM(FILE *){}