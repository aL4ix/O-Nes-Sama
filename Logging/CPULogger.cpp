#include "CPULogger.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

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

CPULogger::CPULogger(Registers& r, ProcessorFlags& f, CPUIO& i, InstructionData& id)
    : regs(r)
    , flags(f)
    , io(i)
    , instData(id)
{
}

string CPULogger::composeInstruction()
{

    stringstream main_ss, op_arg_ss, dasm_ss, regs_ss;

    int args[2] = { instData.arg1, instData.arg2 };
    int opcode = (int)instData.opcode;
    int length_rawinst = (int)instLengths[opcode];

    main_ss << hex << uppercase << left;
    main_ss << setfill(' ') << setw(6) << regs.pc;
    op_arg_ss << hex << uppercase << setfill('0') << setw(2) << opcode << " ";

    for (int i = 0; i < length_rawinst - 1; i++)
        op_arg_ss << setfill('0') << setw(2) << args[i] << " ";

    main_ss << setfill(' ') << setw(9) << op_arg_ss.str();
    main_ss << setfill(' ') << setw(5) << mnemonics[opcode];
    dasm_ss << hex << uppercase;
    switch ((int)addrModes[opcode]) {
    case IMP:
        dasm_ss << " ";
        break;
    case IMM:
        dasm_ss << "#$" << setfill('0') << setw(2) << args[0] << " ";
        break;
    case ACC:
        dasm_ss << "A";
        break;
    case ZP:
        dasm_ss << "$" << setfill('0') << setw(2) << args[0] << " ";
        break;
    case ZPX:
        dasm_ss << "$" << setfill('0') << setw(2) << args[0] << ",X ";
        break;
    case ZPY:
        dasm_ss << "$" << setfill('0') << setw(2) << args[0] << ",Y ";
        break;
    case ABS:
        dasm_ss << "$" << setfill('0') << setw(2) << args[1];
        dasm_ss << setfill('0') << setw(2) << args[0] << " ";
        break;
    case ABSX:
        dasm_ss << "$" << setfill('0') << setw(2) << args[1];
        dasm_ss << setfill('0') << setw(2) << args[0] << ",X ";
        break;
    case ABSY:
        dasm_ss << "$" << setfill('0') << setw(2) << args[1];
        dasm_ss << setfill('0') << setw(2) << args[0] << ",Y ";
        break;
    case INDX:
        dasm_ss << "($" << setfill('0') << setw(2) << args[0] << ",X) ";
        break;
    case INDY:
        dasm_ss << "($" << setfill('0') << setw(2) << args[0] << "),Y ";
        break;
    case IND:
        dasm_ss << "($" << setfill('0') << setw(2) << args[1];
        dasm_ss << setfill('0') << setw(2) << args[0] << ") ";
    case REL:
        dasm_ss << "$" << setfill('0') << setw(2) << regs.pc + (char)args[0] + 2;
        break;
    }

    int cycCount = (instData.generalCycleCount * 3);
    slCount += (instData.generalCycleCount * 3) / 341;

    main_ss << setfill(' ') << setw(28) << dasm_ss.str();
    regs_ss << hex << uppercase << setfill('0') << setw(2);
    regs_ss << "A:" << setfill('0') << setw(2) << (int)regs.a << " ";
    regs_ss << "X:" << setfill('0') << setw(2) << (int)regs.x << " ";
    regs_ss << "Y:" << setfill('0') << setw(2) << (int)regs.y << " ";
    regs_ss << "P:" << setfill('0') << setw(2) << (int)regs.p << " ";
    regs_ss << "SP:" << setfill('0') << setw(2) << (int)regs.sp << " ";
    regs_ss << "CYC:" << right << setfill(' ') << setw(3) << dec << cycCount % 341 << " ";
    regs_ss << "SL:" << right << setfill(' ') << setw(3) << dec << (slCount % 261);
    main_ss << regs_ss.str();
    return main_ss.str();
}

void CPULogger::logInstruction()
{
    Log.debug(LogCategory::cpuLogger, "%s", composeInstruction().c_str());
}

CPULogger::~CPULogger() { }
