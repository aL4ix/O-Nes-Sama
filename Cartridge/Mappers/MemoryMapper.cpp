#include "MemoryMapper.h"

MemoryMapper::MemoryMapper(CartIO& ioRef)
    : io(ioRef)
{
}

MemoryMapper::~MemoryMapper()
{
    delete[] io.wRam;
    io.wRam = nullptr;
    delete[] io.prgBuffer;
    io.prgBuffer = nullptr;
    delete[] io.chrBuffer;
    io.chrBuffer = nullptr;
}
