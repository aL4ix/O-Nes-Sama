#ifndef MAPPER003_HPP_INCLUDED
#define MAPPER003_HPP_INCLUDED

#include "BasicMapper.hpp"

class Mapper003: public BasicMapper{

public:
    Mapper003(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper087: public BasicMapper{

public:
    Mapper087(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper101: public BasicMapper{

public:
    Mapper101(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper145: public BasicMapper{

public:
    Mapper145(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper149: public BasicMapper{

public:
    Mapper149(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper185: public BasicMapper{

public:
    Mapper185(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

#endif // MAPPER003_HPP_INCLUDED
