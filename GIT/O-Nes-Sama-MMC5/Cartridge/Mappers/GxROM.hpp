#ifndef GXROM_HPP_INCLUDED
#define GXROM_HPP_INCLUDED

#include "BasicMapper.hpp"

/* Standard GNROM mapper */
class GNROM : public BasicMapper{

public:
    GNROM(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
    void saveState(FILE * file);
    void loadState(FILE * file);
protected:
    int prgMask;
    int chrMask;
    unsigned char prgBank;
    unsigned char chrBank;
    void sync();
};

/* ColorDreams mapper 11 */
class Mapper011 : public GNROM{

public:
    Mapper011(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
private:
    void sync();
};


/* 64 in 1 and other multi-carts */
class Mapper225 : public BasicMapper{

public:
    Mapper225(CartIO &ioRef);

    //4 x 4 bit ram register
    unsigned char hiBit;
    unsigned char ramReg[4];
    unsigned char mirr;
    unsigned char prgMode;
    unsigned char prgPos;
    unsigned char prgReg;
    unsigned char chrReg;

    unsigned char readCPU(int address);
    void writeCPU(int address, unsigned char val);

    void saveState(FILE * file);
    void loadState(FILE * file);
private:
    void sync();
};


#endif // GXROM_HPP_INCLUDED
