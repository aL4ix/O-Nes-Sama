#ifndef MAPPER002_HPP_INCLUDED
#define MAPPER002_HPP_INCLUDED

#include "BasicMapper.hpp"

class Mapper002 : public BasicMapper{

public:
    Mapper002(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper071 : public BasicMapper{

public:
    Mapper071(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

#endif // MAPPER002_HPP_INCLUDED
