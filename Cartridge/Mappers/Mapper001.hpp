#ifndef MAPPER001_HPP_INCLUDED
#define MAPPER001_HPP_INCLUDED

#include "../Boards/MMC1.hpp"

class Mapper001 : public MMC1{
    public:
        Mapper001(unsigned char * header);
        ~Mapper001();
        void write (int addr, unsigned char val);
        void chrBank0Write();
        void chrBank1Write();
        void prgBankWrite();
};
#endif // MAPPER001_HPP_INCLUDED
