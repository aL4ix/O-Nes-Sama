#ifndef CPUSHAREDDATA_HPP_INCLUDED
#define CPUSHAREDDATA_HPP_INCLUDED

enum addrMode { IMP,
    IMM,
    ACC,
    ZP,
    ZPX,
    ZPY,
    ABS,
    ABSX,
    ABSY,
    INDX,
    INDY,
    IND,
    REL };

struct Registers {
    unsigned char a;
    unsigned char x;
    unsigned char y;
    unsigned char p;
    unsigned char sp;
    int pc;
};

struct ProcessorFlags {
    /* Constants for the processor status flags */
    const unsigned char N_FLAG = 0x80;
    const unsigned char V_FLAG = 0x40;
    const unsigned char U_FLAG = 0x20;
    const unsigned char B_FLAG = 0x10;
    const unsigned char D_FLAG = 0x8;
    const unsigned char I_FLAG = 0x4;
    const unsigned char Z_FLAG = 0x2;
    const unsigned char C_FLAG = 0x1;
};

struct InstructionData {
    unsigned char opcode;
    unsigned char arg1;
    unsigned char arg2;
    unsigned generalCycleCount;
    int addr;
};

#endif // CPUSHAREDDATA_HPP_INCLUDED
