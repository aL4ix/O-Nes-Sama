#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <SDL2/SDL.h>
//#include "Memory.h"
const unsigned char _A = 0x01;
const unsigned char _B = 0x02;
const unsigned char _Sel = 0x04;
const unsigned char _Sta = 0x08;
const unsigned char _Up = 0x10;
const unsigned char _Dn = 0x20;
const unsigned char _Lft = 0x40;
const unsigned char _Rgt = 0x80;

class Input{

    public:
        Input();
        unsigned char read(int addr);
        void write (int addr, unsigned char val);

    private:

        int lastWrite;
        //Memory &memory;
        bool strobeHigh;
        unsigned char shftRegValP1, shftRegValP2;
        void latchButtonStatus();
        void latchP1Buttons();
};

#endif // INPUT_H_INCLUDED
