#include "CPU.hpp"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
/*Log level:
    0 = No logging
    1 = Log cpu activity on demand
*/

#define CPU_LOG_LEVEL 0

using namespace std;


CPU::CPU(MemoryMapper& m)
    : mapper(m)
{

    cycleCount = 0;
    addr = 0;
    pageCrossed = false; /*Used to detect a mem page cross causing extra cycles*/
    needsDummy = false; /*Used to determine if a dummy read is needed in W / RMW instructions*/
    generalCycleCount = 0;
    isDMAPending = false;

    // mapper.setCPUCartSpaceMemPtr(cpuCartSpace);
    /*mapper.io.cpuIRQLine = &ints.irq;
    mapper.io.cpuAddrBus = &addressBus;
    mapper.io.cpu = &dataBus;*/

    mapper.io.cpuIO = &io;

    // mapper.io.cpuM2 = &generalCycleCount;
    isRunning = true;
    isPaused = false;
    /*Initialize CPU registers*/
    regs.a = 0x00;
    regs.x = 0x00;
    regs.y = 0x00;
    regs.sp = 0x00;
    regs.p = 0x34;
    regs.pc = 0x0000;
    instData.opcode = 0;
    instData.generalCycleCount = 0;
    io.nmi = 0;
    io.irq = 0;
    io.reset = 0;
    isNMIPending = false;
    isIntPendng = false;
    controller = new Input;
    controller->cpuIO = &io;
    apu = new APU(io);
    apu->setMemoryMapper(&mapper);

/*Initialize Logger*/
#if CPU_LOG_LEVEL == 1
    cpuLogger = new CPULogger(regs, flags, io, instData);
#endif
}

/*******************************************************************************/
/* Memory Handlers                                                             */
/*******************************************************************************/

unsigned char CPU::read(int addr)
{
    unsigned char ret = -1;

    mapper.clockCPU();
    ppu->process(1);
    apu->process(1);
    /*Increase cycle count*/
    cycleCount++;
    generalCycleCount++;

    if ((addr >= 0x4000) && (addr <= 0x401F)) // APU's only read register
    {
        if ((addr == 0x4016) || (addr == 0x4017)) {
            ret = controller->read(addr); // Controller input regs.
        } else {
            ret = apu->readMem(addr);
        }

    }

    else {
        switch (addr >> 12) {
        case 0:
        case 1:
            ret = ram[addr & 0x7FF]; // Masked with 0x7FF to handle ram mirrors
            break;
        case 2:
        case 3: // PPU regs
            ret = ppu->readMem(addr);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            ret = mapper.readCPU(addr);
            break;
        }
    }
    io.addressBus = addr;
    io.dataBus = ret;
    return ret;
}

unsigned char CPU::readCode(int addr)
{
    switch (addr >> 12) {
    case 0:
    case 1:
        return ram[addr & 0x7FF]; // Masked with 0x7FF to handle ram mirrors
    case 4: /* APU & General I/O */
        switch (addr & 0x3F) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
            /* Return Open Bus, because there are not readable registers mapped here*/
            return (addr & 0xFF00) >> 8;
        case 0x15:
            /* PUT APU READ HERE */
            return apu->readMem(addr);
        case 0x16:
        case 0x17:
            return controller->read(addr);
        }
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        return mapper.readCPU(addr);
    case 2:
    case 3:
        return ppu->readMem(addr);
    }
    return -1;
}

void CPU::write(int addr, unsigned char val)
{
    /*Increase cycle count*/
    cycleCount++;
    generalCycleCount++;
    mapper.clockCPU();
    ppu->process(1);
    apu->process(1);
    io.addressBus = addr;
    io.dataBus = val;

    if ((addr >= 0x4000) && (addr <= 0x401F)) // APU's only read register
    {
        if (addr == 0x4016) // Controller write
        {
            controller->write(addr, val);
        }

        else if (addr == 0x4014) // Sprite DMA
        {
            if (generalCycleCount & 1)
                read(addr);
            read(addr);
            for (int i = 0; i < 256; i++) {
                write(0x2004, read((val << 8) + i));
            }
        }

        else // APU Write
        {
            apu->writeMem(addr, val);
        }
    }

    else {
        switch (addr >> 12) {
        case 0:
        case 1:
            ram[addr & 0x7FF] = val; // Masked with 0x7FF to handle ram mirrors
            break;
        case 2:
        case 3:
            ppu->writeMem(addr, val);
            break;

        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            mapper.writeCPU(addr, val);
            break;
        }
    }
}

void CPU::setPPUPtr(PPU* p)
{
    ppu = p;
}

/**************************************************************************************/
/* Addressing                                                                         */
/**************************************************************************************/

int CPU::getAddress(int addrMode)
{
    int addr = -1;
    switch (addrMode) {
    case IMM: /*Immediate mode*/
        addr = regs.pc++;
        return addr & 0xFFFF;
    case ZP: /*Zero Page mode*/
        addr = read(regs.pc++); // 2
        return addr & 0xFFFF;
    case ZPX: /*Zero Page, X mode*/
        addr = read(regs.pc++); // 2
        read(addr); // 3 Dummy, El 6502 obra de manera misteriosa
        addr = (addr + regs.x) & 0xFF;
        return addr & 0xFFFF;
    case ZPY: /*Zero Page, Y mode*/
        addr = read(regs.pc++); // 2
        read(addr); // 3 Dummy...
        addr = (addr + regs.y) & 0xFF;
        return addr & 0xFFFF;
    case ABS: /*Absolute Mode*/
        addr = read(regs.pc++); // 2
        addr |= (read(regs.pc++) << 8); // 3
        return addr & 0xFFFF;
    case ABSX: /*Absolute, X mode*/
        addr = read(regs.pc++); // 2 Lo byte
        pageCrossed = (addr + regs.x) > 0xFF;
        addr = (addr + regs.x) & 0xFF;
        addr |= (read(regs.pc++) << 8); // 3 Hi byte
        /*Invalid high byte dummy reads will need to be checked later in the WR/RMW routines*/
        addr &= 0xFFFF;
        needsDummy = true;
        return addr & 0xFFFF;
    case ABSY: /*Absolute, Y mode*/
        addr = read(regs.pc++); // 2 Lo byte
        pageCrossed = (addr + regs.y) > 0xFF;
        addr = (addr + regs.y) & 0xFF;
        addr |= (read(regs.pc++) << 8); // 3 Lo byte
        /*Invalid high byte and dummy reads will need to be checked later in the WR/RMW routines*/
        needsDummy = true;
        return addr & 0xFFFF;
    case INDX:
        addr = read(regs.pc++); // 2 Fetch pointer, increment PC
        read(addr); // 3 Dummy, Read Pointer
        addr = (addr + regs.x) & 0xFF; // Add X to pointer, wraps with ZP
        addr = read(addr) | (read((addr + 1) & 0xFF) << 8); // 4 & 5 Read Lo & Hi bytes (ptr + x, ptr + x + 1)
        return addr & 0xFFFF;
    case INDY:
        addr = read(regs.pc++); // 2 Fetch pointer, increment PC
        {
            int lo = read(addr); // 3 fetch Lo byte
            int hi = read((addr + 1) & 0xFF); // 4 fetch Hi byte
            pageCrossed = ((lo + regs.y) > 0xFF);
            lo = (lo + regs.y) & 0xFF; // Add Y to Lo byte, with ZP wrapping.
            addr = (lo | (hi << 8));
            /*Invalid high byte will need to be checked later in the read/RMW routines*/
            needsDummy = true;
        }
        return addr & 0xFFFF;
    case IND: {
        /*Get absolute address*/
        unsigned char ptrLo = read(regs.pc++); // 2
        unsigned char ptrHi = read(regs.pc++); // 3
        int ptr = ptrLo | (ptrHi << 8);
        unsigned char lo = read(ptr); // 4
        pollForInterrupts();
        addr = lo | (read(((ptr + 1) & 0xFF) | (ptr & 0xFF00)) << 8); // 5
    }
        return addr & 0xFFFF;
    }
    return -1;
}

/*******************************************************************************************/
/* Main instructions execution loop                                                        */
/*******************************************************************************************/

int CPU::run(const int cycles)
{
    isRunning = true;
    int cyclesRemain = cycles;
#ifdef DEBUG_PRECISETIMING
    unsigned totalSum = 0;
#endif // DEBUG_PRECISETIMING

    if (isPaused) {
        return 0;
    }

    while ((cyclesRemain > 0) && (isRunning)) {
        cycleCount = 0;
#ifdef DEBUG_PRECISETIMING
        unsigned localGeneralCount = generalCycleCount;
#endif // DEBUG_PRECISETIMING
        if (isIntPendng) {
            interruptSequence(0);
            if (isIRQPending) {
                Log.debug(LogCategory::cpuRun, "REAL: %d %d", *mapper.io.dbg.sl, *mapper.io.dbg.tick);
            }
        }

        pageCrossed = false;
        needsDummy = false;

        /*Data to feed the CPU Loggers and Debuggers*/
        instData.opcode = read(regs.pc); // 1
#if CPU_LOG_LEVEL == 1
        instData.arg1 = readCode(regs.pc + 1);
        instData.arg2 = readCode(regs.pc + 2);
        cpuLogger->logInstruction();
#endif
        regs.pc++;

#include "opcodes.inc"

#ifdef DEBUG_PRECISETIMING
        if ((generalCycleCount - localGeneralCount) != cycleCount) {
            Log.debug(LogCategory::cpuPreciseTime, "Debug Precise Timing");
        }
        totalSum += cycleCount;
#endif // DEBUG_PRECISETIMING

        cyclesRemain -= cycleCount;
        instData.generalCycleCount = generalCycleCount;
    }

#ifdef DEBUG_PRECISETIMING
    if (totalSum != cycles - cyclesRemain) {
        Log.debug(LogCategory::cpuPreciseTime, "NEL ts:%u s:%d cr:%d", totalSum, cycles, cyclesRemain);
    }
#endif // DEBUG_PRECISETIMING

    return cyclesRemain;
}

/***********************************************************************/
/*Interrupts implementation                                            */
/***********************************************************************/

inline void CPU::interruptSequence(int brk)
{

    int vec = 0;
    unsigned char pVal = regs.p;

    if (brk) {
        read(regs.pc++);
        pVal |= (flags.B_FLAG | flags.U_FLAG); // Activate B and U flag
    } else {
        read(regs.pc);
        read(regs.pc);
    }

    if (!io.reset) {
        write(0x100 + (regs.sp--), regs.pc >> 8); // Store PC HI
        write(0x100 + (regs.sp--), regs.pc & 0xFF); // Store PC LO
    } else {
        read(0x100 + (regs.sp--)); // Dummy Store PC HI
        read(0x100 + (regs.sp--)); // Dummy Store PC LO
    }

    // Select Interrupt Vector
    if (io.irq || brk) {
        vec = 0xFFFE;
        brk = 0;
    }
    if (io.nmi) {
        vec = 0xFFFA;
    }

    if (io.reset)
        vec = 0xFFFC;

    if (!io.reset)
        write(0x100 + (regs.sp--), pVal); // Store P
    else
        read(0x100 + (regs.sp--));

    regs.p |= flags.I_FLAG;
    regs.pc = read(vec);
    regs.pc |= ((read(vec + 1)) << 8);
    io.reset = 0;
    io.nmi = 0;
    io.irq = 0;
}

void CPU::pollForInterrupts()
{
    isIRQPending = (!(regs.p & flags.I_FLAG) && io.irq);
    if (isIRQPending && io.nmi) {
        Log.debug(LogCategory::cpuAll, "IRQ Forgotten...");
        io.irq = 0;
    }
    isIntPendng = io.reset || io.nmi || isIRQPending;
}

void CPU::reset()
{
    io.reset = 1;
    interruptSequence(0);
    apu->reset();
    Log.debug(LogCategory::cpuAll, "Reset: %X", regs.pc);
}

/***********************************************************************/
/* Instructions implementations                                        */
/***********************************************************************/

void CPU::rd(rdOp op, int addr)
{
    unsigned char val = 0;
    pollForInterrupts();
    /*If page crossed the operation will take 1 cycle more*/
    val = read(addr); // Dummy Read
    // Fix high byte
    addr = (addr + (pageCrossed << 8)) & 0xFFFF;
    if (pageCrossed)
        val = read(addr);
    (this->*op)(val);
}

void CPU::rmw(rmwOp op, int addr)
{
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    addr &= 0xFFFF;
    unsigned char val = read(addr);
    write(addr, val); // Dummy pff...
    (this->*op)(val);
    pollForInterrupts();
    write(addr, val);
}

void CPU::setZN(unsigned char reg)
{
    regs.p &= ~(flags.N_FLAG | flags.Z_FLAG);
    regs.p |= ((reg & flags.N_FLAG) | ((reg == 0) << 1));
}

// Read Operations
void CPU::LDReg(unsigned char& reg, unsigned char val)
{
    reg = val;
    setZN(reg);
}

void CPU::LDA(unsigned char val) { LDReg(regs.a, val); }
void CPU::LDX(unsigned char val) { LDReg(regs.x, val); }
void CPU::LDY(unsigned char val) { LDReg(regs.y, val); }

void CPU::EOR(unsigned char val)
{
    regs.a ^= val;
    setZN(regs.a);
}

inline void CPU::NOP(unsigned char val) { } // Does absolutely nothing

void CPU::AND(unsigned char val)
{
    regs.a &= val;
    setZN(regs.a);
}
void CPU::AAC(unsigned char val)
{
    AND(val);
    regs.p &= ~flags.C_FLAG;
    regs.p |= ((regs.p & flags.N_FLAG) >> 7);
}
void CPU::ASR(unsigned char val)
{
    AND(val);
    LSR(regs.a);
}
void CPU::ARR(unsigned char val)
{
    regs.a = ((val & regs.a) >> 1) | ((regs.p & flags.C_FLAG) << 7);
    setZN(regs.a);
    regs.p &= ~(flags.C_FLAG | flags.V_FLAG);
    regs.p |= (regs.a >> 6 & 0x1);
    regs.p |= (((regs.a >> 6 ^ regs.a >> 5) & 0x1) << 6);
}
void CPU::ATX(unsigned char val)
{
    ORA(0xFF);
    AND(val);
    TAX();
}
void CPU::AXS(unsigned char val)
{
    unsigned int res = (regs.a & regs.x) - val;
    regs.p &= ~flags.C_FLAG;
    regs.p |= (res <= 0xFF);
    regs.x = res & 0xFF;
    setZN(regs.x);
}

void CPU::ANE(unsigned char val)
{
    regs.a = (regs.a | 0xEE) & regs.x & val;
    setZN(regs.a);
}

void CPU::LAE(unsigned char val)
{
    regs.sp &= val;
    regs.x = regs.sp;
    regs.a = regs.sp;
    setZN(regs.sp);
}

void CPU::ORA(unsigned char val)
{
    regs.a |= val;
    setZN(regs.a);
}

void CPU::ADC(unsigned char val)
{
    int result = regs.a + val + (regs.p & 1);
    regs.p &= ~(flags.Z_FLAG | flags.C_FLAG | flags.N_FLAG | flags.V_FLAG);
    regs.p |= ((((regs.a ^ val) & 0x80) ^ 0x80) & ((regs.a ^ result) & 0x80)) >> 1;
    regs.p |= (result >> 8) & flags.C_FLAG;
    regs.a = result;
    setZN(regs.a);
}
void CPU::SBC(unsigned char val)
{
    int result = regs.a - val - ((regs.p & 1) ^ 1);
    regs.p &= ~(flags.Z_FLAG | flags.C_FLAG | flags.N_FLAG | flags.V_FLAG);
    regs.p |= ((regs.a ^ result) & (regs.a ^ val) & 0x80) >> 1;
    regs.p |= ((result >> 8) & flags.C_FLAG) ^ flags.C_FLAG;
    regs.a = result;
    setZN(regs.a);
}

void CPU::CMPReg(unsigned char reg, unsigned char val)
{
    setZN((reg - val) & 0xFF);
    regs.p &= ~flags.C_FLAG;
    regs.p |= (((reg - val) >> 8) & flags.C_FLAG) ^ flags.C_FLAG;
}

void CPU::CMP(unsigned char val) { CMPReg(regs.a, val); }
void CPU::CPX(unsigned char val) { CMPReg(regs.x, val); }
void CPU::CPY(unsigned char val) { CMPReg(regs.y, val); }
void CPU::BIT(unsigned char val)
{
    regs.p &= ~(flags.V_FLAG | flags.N_FLAG | flags.Z_FLAG);
    setZN(regs.a & val);
    regs.p |= val & (flags.V_FLAG | flags.N_FLAG);
}
void CPU::LAX(unsigned char val)
{
    regs.a = regs.x = val;
    setZN(regs.x);
}
// RMW Operations
void CPU::ASL(unsigned char& val)
{
    regs.p &= ~flags.C_FLAG;
    regs.p |= val >> 7;
    val <<= 1;
    setZN(val);
}
void CPU::LSR(unsigned char& val)
{
    regs.p &= ~(flags.C_FLAG | flags.N_FLAG | flags.Z_FLAG);
    regs.p |= val & 1;
    val = val >> 1;
    setZN(val);
}
void CPU::ROL(unsigned char& val)
{
    int l = val >> 7;
    val <<= 1;
    val |= regs.p & flags.C_FLAG;
    regs.p &= ~(flags.Z_FLAG | flags.N_FLAG | flags.C_FLAG);
    regs.p |= l;
    setZN(val);
}
void CPU::ROR(unsigned char& val)
{
    int l = val & 1;
    val >>= 1;
    val |= (regs.p & flags.C_FLAG) << 7;
    regs.p &= ~(flags.Z_FLAG | flags.N_FLAG | flags.C_FLAG);
    regs.p |= l;
    setZN(val);
}
void CPU::INC(unsigned char& val)
{
    val++;
    setZN(val);
}
void CPU::DEC(unsigned char& val)
{
    val--;
    setZN(val);
}
void CPU::SLO(unsigned char& val)
{
    ASL(val);
    ORA(val);
}
void CPU::SRE(unsigned char& val)
{
    LSR(val);
    EOR(val);
}
void CPU::RLA(unsigned char& val)
{
    ROL(val);
    AND(val);
}
void CPU::RRA(unsigned char& val)
{
    ROR(val);
    ADC(val);
}
void CPU::ISB(unsigned char& val)
{
    INC(val);
    SBC(val);
}
void CPU::DCP(unsigned char& val)
{
    DEC(val);
    CMPReg(regs.a, val);
}

void CPU::bra(bool branchCondition)
{

    pollForInterrupts();
    char jump = read(regs.pc++); // 2
    int tmppc = regs.pc;
    if (branchCondition) {
        read(regs.pc); // 3 - Fetch next instruction opcode (dummy cycle).
        regs.pc = (regs.pc + jump) & 0xFFFF;
        if ((tmppc ^ regs.pc) & 0x100) { // If there was pageCrossing
            pollForInterrupts();
            read(regs.pc); // 4 - Fetch next instruction opcode (dummy cycle).
        }
    }
}

void CPU::BRK()
{ /*isStopped = true;*/
    interruptSequence(1);
}

void CPU::JMPAbs()
{
    unsigned char lo = read(regs.pc++); // 2
    pollForInterrupts();
    regs.pc = lo | (read(regs.pc) << 8); // 3
}

void CPU::JMPInd()
{
    addr = getAddress(IND);
    regs.pc = addr;
}

void CPU::JSR(int addr)
{
    unsigned char lo = read(regs.pc++); // 2
    read(regs.sp + 0x100); // 3
    write((regs.sp--) + 0x100, regs.pc >> 8); // 4
    write((regs.sp--) + 0x100, regs.pc & 0xFF); // 5
    pollForInterrupts();
    regs.pc = lo | (read(regs.pc) << 8); // 6
}

void CPU::RTI()
{
    read(regs.pc); // 2
    read((regs.sp++) + 0x100); // 3
    regs.p = read(0x100 + regs.sp++); // 4
    regs.p &= ~(flags.B_FLAG);
    regs.p |= flags.U_FLAG;
    regs.pc = read(0x100 + regs.sp++); // 5
    pollForInterrupts();
    regs.pc |= (read(0x100 + regs.sp) << 8); // 6
}

void CPU::RTS()
{
    regs.pc--;
    read((regs.sp++) + 0x100);
    regs.pc = read((regs.sp++) + 0x100); // pop(); regs.sp++; //4
    regs.pc |= (read(regs.sp + 0x100) << 8); // pop() << 8; //5
    pollForInterrupts();
    read(regs.pc++); // 6
}

void CPU::PHP()
{
    pollForInterrupts();
    write((regs.sp--) + 0x100, regs.p | flags.B_FLAG | flags.U_FLAG);
}

void CPU::PLP()
{

    read((regs.sp++) + 0x100);
    pollForInterrupts();
    regs.p = read(regs.sp + 0x100); // pop(); //4
    regs.p &= ~(flags.B_FLAG);
    regs.p |= flags.U_FLAG;
}

void CPU::PHA()
{
    pollForInterrupts();
    write((regs.sp--) + 0x100, regs.a);
}

void CPU::PLA()
{
    read((regs.sp++) + 0x100);
    pollForInterrupts();
    regs.a = read(regs.sp + 0x100); // 4
    setZN(regs.a);
}

void CPU::DEY()
{
    regs.y--;
    setZN(regs.y);
}

void CPU::TAY()
{
    regs.y = regs.a;
    setZN(regs.y);
}
void CPU::INY()
{
    regs.y++;
    setZN(regs.y);
}
void CPU::INX()
{
    regs.x++;
    setZN(regs.x);
}
void CPU::CLC() { regs.p &= ~flags.C_FLAG; }
void CPU::SEC() { regs.p |= flags.C_FLAG; }
void CPU::CLI() { regs.p &= ~flags.I_FLAG; };
void CPU::SEI() { regs.p |= flags.I_FLAG; }
void CPU::TYA()
{
    regs.a = regs.y;
    setZN(regs.a);
}
void CPU::CLV() { regs.p &= ~flags.V_FLAG; }
void CPU::CLD() { regs.p &= ~flags.D_FLAG; }
void CPU::SED() { regs.p |= flags.D_FLAG; }
void CPU::TXA()
{
    regs.a = regs.x;
    setZN(regs.a);
}
void CPU::TXS() { regs.sp = regs.x; }
void CPU::TAX()
{
    regs.x = regs.a;
    setZN(regs.x);
}
void CPU::TSX()
{
    regs.x = regs.sp;
    setZN(regs.x);
}
void CPU::DEX()
{
    regs.x--;
    setZN(regs.x);
}
void CPU::NOP() { }

/***********************************************************************/

bool CPU::loadState(FILE* file)
{
    CpuStateData* data = this;
    auto size = fread(data, sizeof(CpuStateData), 1, file);
    return (size == sizeof(CpuStateData));
}

void CPU::saveState(FILE* file)
{
    CpuStateData* data = this;
    fwrite(data, sizeof(CpuStateData), 1, file);
}

/***********************************************************************/

CPU::~CPU()
{
    delete apu;
    delete controller;
}
