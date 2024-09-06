#include "CPULogger.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>


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
