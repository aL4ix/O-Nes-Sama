#ifndef GXROM_HPP_INCLUDED
#define GXROM_HPP_INCLUDED

#include "BasicMapper.hpp"

/* ColorDreams mapper 11 */
class Mapper011 : public BasicMapper{

public:
    Mapper011(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

/* Standard GNROM mapper */
class Mapper066 : public BasicMapper{

public:
    Mapper066(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};


/* 64 in 1 and other multi-carts */
class Mapper255 : public BasicMapper{

public:
    Mapper255(CartIO &ioRef);
    //4 x 4 bit ram register

    unsigned char ramReg[4];
    int mirr;
    int prgMode;
    int prgPos;
    int prgReg;
    int chrReg;
    void writeCPU(int address, unsigned char val);
};


#endif // GXROM_HPP_INCLUDED
