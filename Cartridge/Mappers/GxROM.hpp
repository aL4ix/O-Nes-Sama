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


#endif // GXROM_HPP_INCLUDED
