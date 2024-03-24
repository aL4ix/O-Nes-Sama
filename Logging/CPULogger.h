#ifndef CPULOGGER_H_INCLUDED
#define CPULOGGER_H_INCLUDED
#include "../CPUIO.hpp"
#include "../CPUSharedData.h"
#include "Logger.h"

using namespace std;

class CPULogger : public Logger {
private:
    Registers& regs;
    ProcessorFlags& flags;
    CPUIO& io;
    InstructionData& instData;
    string composeInstruction();

public:
    CPULogger(Registers&, ProcessorFlags&, CPUIO&, InstructionData&,
        const char* fileName = NULL);
    void logInstruction();
    ~CPULogger();
};

#endif // CPULOGGER_H_INCLUDED
