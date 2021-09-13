#ifndef MAPPER002_HPP_INCLUDED
#define MAPPER002_HPP_INCLUDED

#include "BasicMapper.hpp"

class UxROM : public BasicMapper{

public:
    UxROM(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
    void saveState(FILE * file);
    void loadState(FILE * file);
protected:
    int mask;
    int bank;
    void sync();
};

class Mapper071 : public UxROM{

public:
    Mapper071(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

#endif // MAPPER002_HPP_INCLUDED
