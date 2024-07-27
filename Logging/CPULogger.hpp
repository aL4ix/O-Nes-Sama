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

public:
    CPULogger(Registers&, ProcessorFlags&, CPUIO&, InstructionData&);
    void logInstruction();
    ~CPULogger();
};

#endif // CPULOGGER_HPP_INCLUDED
