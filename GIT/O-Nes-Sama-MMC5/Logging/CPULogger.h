#ifndef CPULOGGER_H_INCLUDED
#define CPULOGGER_H_INCLUDED
#include "Logger.h"
#include "../CPUSharedData.h"
#include "../CPUIO.hpp"

using namespace std;

class CPULogger : public Logger{
    private:
        Registers &regs;
        ProcessorFlags &flags;
        CPUIO &io;
        InstructionData &instData;
        string composeInstruction();

    public:
        CPULogger(Registers &, ProcessorFlags &, CPUIO &, InstructionData &,
            const char * fileName = NULL);
        void logInstruction();
        ~CPULogger();

};

#endif // CPULOGGER_H_INCLUDED
