#ifndef CPULOGGER_HPP_INCLUDED
#define CPULOGGER_HPP_INCLUDED
#include "../CPUIO.hpp"
#include "../CPUSharedData.hpp"
#include "Logger.hpp"

using namespace std;

class CPULogger {
private:
    Registers& regs;
    ProcessorFlags& flags;
    CPUIO& io;
    InstructionData& instData;
    string composeInstruction();

    int slCount = 241;
    // clang-format off
    string mnemonics[256] = {
           /*00     01    02     03     04     05     06     07     08     09     0A     0B     0C     0D     0E     0F   */
    /*00*/  " BRK"," ORA"," ---"," ---","*NOP"," ORA"," ASL"," ---"," PHP"," ORA"," ASL"," ---","*NOP"," ORA"," ASL"," ---",
    /*01*/  " BPL"," ORA"," ---"," ---","*NOP"," ORA"," ASL"," ---"," CLC"," ORA","*NOP"," ---","*NOP"," ORA"," ASL"," ---",
    /*02*/  " JSR"," AND"," ---"," ---"," BIT"," AND"," ROL"," ---"," PLP"," AND"," ROL"," ---"," BIT"," AND"," ROL"," ---",
    /*03*/  " BMI"," AND"," ---"," ---","*NOP"," AND"," ROL"," ---"," SEC"," AND","*NOP"," ---","*NOP"," AND"," ROL"," ---",
    /*04*/  " RTI"," EOR"," ---"," ---","*NOP"," EOR"," LSR"," ---"," PHA"," EOR"," LSR"," ---"," JMP"," EOR"," LSR"," ---",
    /*05*/  " BVC"," EOR"," ---"," ---","*NOP"," EOR"," LSR"," ---"," CLI"," EOR","*NOP"," ---","*NOP"," EOR"," LSR"," ---",
    /*06*/  " RTS"," ADC"," ---"," ---","*NOP"," ADC"," ROR"," ---"," PLA"," ADC"," ROR"," ---"," JMP"," ADC"," ROR"," ---",
    /*07*/  " BVS"," ADC"," ---"," ---","*NOP"," ADC"," ROR"," ---"," SEI"," ADC","*NOP"," ---","*NOP"," ADC"," ROR"," ---",
    /*08*/  "*NOP"," STA"," ---"," ---"," STY"," STA"," STX"," ---"," DEY","*NOP"," TXA"," ---"," STY"," STA"," STX"," ---",
    /*09*/  " BCC"," STA"," ---"," ---"," STY"," STA"," STX"," ---"," TYA"," STA"," TXS"," ---","*NOP"," STA"," ---"," ---",
    /*0A*/  " LDY"," LDA"," LDX","*LAX"," LDY"," LDA"," LDX","*LAX"," TAY"," LDA"," TAX"," ---"," LDY"," LDA"," LDX","*LAX",
    /*0B*/  " BCS"," LDA"," ---","*LAX"," LDY"," LDA"," LDX","*LAX"," CLV"," LDA"," TSX"," ---"," LDY"," LDA"," LDX"," ---",
    /*0C*/  " CPY"," CMP"," ---"," ---"," CPY"," CMP"," DEC"," ---"," INY"," CMP"," DEX"," ---"," CPY"," CMP"," DEC"," ---",
    /*0D*/  " BNE"," CMP"," ---"," ---","*NOP"," CMP"," DEC"," ---"," CLD"," CMP","*NOP"," ---","*NOP"," CMP"," DEC"," ---",
    /*0E*/  " CPX"," SBC"," ---"," ---"," CPX"," SBC"," INC"," ---"," INX"," SBC"," NOP"," ---"," CPX"," SBC"," INC"," ---",
    /*0F*/  " BEQ"," SBC"," ---"," ---","*NOP"," SBC"," INC"," ---"," SED"," SBC","*NOP"," ---","*NOP"," SBC"," INC"," ---",
    };

    char addrModes[256] = {
           /*00   01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F   */
    /*00*/  IMP , INDX, 0x7F, 0x7F, ZP  , ZP  , ZP  , 0x7F, IMP , IMM , ACC , 0x7F, ABS , ABS , ABS , 0x7F,
    /*01*/  REL , INDY, 0x7F, 0x7F, ZPX , ZPX , ZPX , 0x7F, IMP , ABSY, IMP , 0x7F, ABSX, ABSX, ABSX, 0x7F,
    /*02*/  ABS , INDX, 0x7F, 0x7F, ZP  , ZP  , ZP  , 0x7F, IMP , IMM , ACC , 0x7F, ABS , ABS , ABS , 0x7F,
    /*03*/  REL , INDY, 0x7F, 0x7F, ZPX , ZPX , ZPX , 0x7F, IMP , ABSY, IMP , 0x7F, ABSX, ABSX, ABSX, 0x7F,
    /*04*/  IMP , INDX, 0x7F, 0x7F, ZP  , ZP  , ZP  , 0x7F, IMP , IMM , ACC , 0x7F, ABS , ABS , ABS , 0x7F,
    /*05*/  REL , INDY, 0x7F, 0x7F, ZPX , ZPX , ZPX , 0x7F, IMP , ABSY, IMP , 0x7F, ABSX, ABSX, ABSX, 0x7F,
    /*06*/  IMP , INDX, 0x7F, 0x7F, ZP  , ZP  , ZP  , 0x7F, IMP , IMM , ACC , 0x7F, IND , ABS , ABS , 0x7F,
    /*07*/  REL , INDY, 0x7F, 0x7F, ZPX , ZPX , ZPX , 0x7F, IMP , ABSY, IMP , 0x7F, ABSX, ABSX, ABSX, 0x7F,
    /*08*/  IMM , INDX, 0x7F, 0x7F, ZP  , ZP  , ZP  , 0x7F, IMP , IMM , IMP , 0x7F, ABS , ABS , ABS , 0x7F,
    /*09*/  REL , INDY, 0x7F, 0x7F, ZPX , ZPX , ZPY , 0x7F, IMP , ABSY, IMP , 0x7F, 0x7F, ABSX, 0x7F, 0x7F,
    /*0A*/  IMM , INDX, IMM , INDX, ZP  , ZP  , ZP  , ZP  , IMP , IMM , IMP , 0x7F, ABS , ABS , ABS , ABS ,
    /*0B*/  REL , INDY, 0x7F, INDY, ZPX , ZPX , ZPY , ZPY , IMP , ABSY, IMP , 0x7F, ABSX, ABSX, ABSY, 0x7F,
    /*0C*/  IMM , INDX, 0x7F, 0x7F, ZP  , ZP  , ZP  , 0x7F, IMP , IMM , IMP , 0x7F, ABS , ABS , ABS , 0x7F,
    /*0D*/  REL , INDY, 0x7F, 0x7F, ZPX, ZPX  , ZPX , 0x7F, IMP , ABSY, IMP , 0x7F, ABSX, ABSX, ABSX, 0x7F,
    /*0E*/  IMM , INDX, 0x7F, 0x7F, ZP  , ZP  , ZP  , 0x7F, IMP , IMM , IMP , 0x7F, ABS , ABS , ABS , 0x7F,
    /*0F*/  REL , INDY, 0x7F, 0x7F, ZPX, ZPX  , ZPX , 0x7F, IMP , ABSY, IMP , 0x7F, ABSX, ABSX, ABSX, 0x7F,
    };

    char instLengths[256] = {
          /*00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F */
    /*00*/  1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*01*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    /*02*/  3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*03*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    /*04*/  1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*05*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    /*06*/  1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*07*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    /*08*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 2, 3, 3, 3, 0,
    /*09*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,
    /*0A*/  2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3,
    /*0B*/  2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 0, 3, 3, 3, 0,
    /*0C*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*0D*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    /*0E*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*0F*/  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    };
    // clang-format on

public:
    CPULogger(Registers&, ProcessorFlags&, CPUIO&, InstructionData&);
    void logInstruction();
    ~CPULogger();
};

#endif // CPULOGGER_HPP_INCLUDED
