#include "NSFMapper.hpp"

NSFMapper::NSFMapper(CartIO& ioRef)
    : MemoryMapper(ioRef)
{
    io.wRam = new unsigned char[0x2000]; // 8K
    io.switch8K(0, 0, io.wRam, io.wRamSpace);
}

unsigned char NSFMapper::readCPU(int address)
{
    switch (address >> 12) {
    case 0x6:
    case 0x7:
        return io.wRamSpace[(address - 0x6000) >> 10][address & 0x3FF];
        break;
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0xF:
        if (address == 0xFFFA) {
            // printf("LOOP\n");
            cpu->isRunning = true;
            return 0xB8;
        } else if (address == 0xFFFB)
            return 0x50;
        else if (address == 0xFFFC)
            return 0xFD;
        else {
            auto bank = (address - 0x8000) >> 10;
            return io.prgSpace[bank][address & 0x3FF];
        }
        break;
    }
    return 0xFF;
}

void NSFMapper::writeCPU(int address, unsigned char val)
{
    switch (address >> 12) {
    case 0x5:
        if (address >= 0x5FF8) {
            auto bank = address & 0x7;
            io.switch4K(bank, val & 0x7, io.prgBuffer, io.prgSpace);
        }
    case 0x6:
    case 0x7:
        io.wRamSpace[(address - 0x6000) >> 10][address & 0x3FF] = val;
    }
}

void NSFMapper::clockCPU()
{
}

unsigned char NSFMapper::readPPU(int address)
{
    return 0xFF;
}

void NSFMapper::writePPU(int address, unsigned char val)
{
}

void NSFMapper::clockPPU()
{
}

void NSFMapper::loadSRAM(FILE* batteryFile)
{
}

void NSFMapper::saveSRAM(FILE* batteryFile)
{
}

void NSFMapper::setCPU(CPU* Cpu)
{
    cpu = Cpu;
}

NSFMapper::~NSFMapper()
{
    delete[] io.wRam;
}
