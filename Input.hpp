#ifndef INPUT_HPP_INCLUDED
#define INPUT_HPP_INCLUDED

#include "CPUIO.hpp"
#include <cstring>

class Input {
public:
    Input();
    CPUIO* cpuIO;
    unsigned char read(int addr);
    void write(int addr, unsigned char val);
    void latchButtonStatus();
    void updatePlayersInput(bool (*input)[8]);

private:
    const unsigned char _A = 0x01;
    const unsigned char _B = 0x02;
    const unsigned char _Sel = 0x04;
    const unsigned char _Sta = 0x08;
    const unsigned char _Up = 0x10;
    const unsigned char _Dn = 0x20;
    const unsigned char _Lft = 0x40;
    const unsigned char _Rgt = 0x80;
    bool input[2][8];
    int shftRegs[2];
};

#endif // INPUT_HPP_INCLUDED
