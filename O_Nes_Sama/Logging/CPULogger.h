#ifndef CPULOGGER_H_INCLUDED
#define CPULOGGER_H_INCLUDED
#include "Logger.h"
#include "../CPUSharedData.h"
#include "../x6502Interrupts.h"

using namespace std;

class CPULogger : public Logger{
    private:
        Registers &regs;
        ProcessorFlags &flags;
        InterruptLines &ints;
        InstructionData &instData;
        string composeInstruction();

    public:
        CPULogger(Registers &, ProcessorFlags &, InterruptLines &, InstructionData &,
            const char * fileName = NULL);
        void logInstruction();
        ~CPULogger();

};

#endif // CPULOGGER_H_INCLUDED
