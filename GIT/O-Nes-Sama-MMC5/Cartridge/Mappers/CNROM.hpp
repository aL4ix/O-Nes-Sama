#ifndef MAPPER003_HPP_INCLUDED
#define MAPPER003_HPP_INCLUDED

#include "BasicMapper.hpp"

class CNROM: public BasicMapper{

public:
    CNROM(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
    void saveState(FILE * file);
    void loadState(FILE * file);
protected:
    unsigned char bank;
    virtual void sync();
};

class Mapper087: public CNROM{

public:
    Mapper087(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);

};

class Mapper101: public CNROM{

public:
    Mapper101(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper145: public CNROM{

public:
    Mapper145(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper149: public CNROM{

public:
    Mapper149(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
};

class Mapper185: public CNROM{

public:
    Mapper185(CartIO &ioRef);
    void writeCPU(int address, unsigned char val);
private:
    unsigned char disabledCHR[0x2000] = {0xFF};
};

#endif // MAPPER003_HPP_INCLUDED
