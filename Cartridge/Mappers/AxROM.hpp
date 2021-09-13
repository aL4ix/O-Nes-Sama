#ifndef MAPPER007_HPP_INCLUDED
#define MAPPER007_HPP_INCLUDED

#include "BasicMapper.hpp"

class Mapper007 : public BasicMapper{

public:
    Mapper007(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
    void saveState(FILE * file);
    void loadState(FILE * file);
private:
    int bank;
    void sync();
};

#endif // MAPPER007_HPP_INCLUDED
