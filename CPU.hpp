#ifndef CPU_HPP_INCLUDED
#define CPU_HPP_INCLUDED
#include "Cartridge/Mappers/MemoryMapper.hpp"
#include "Logging/CPULogger.hpp"

#ifdef EIGHT_TUNES
#include "8Tunes/DummyInput.hpp"
#include "8Tunes/DummyPPU.hpp"
#else
#include "Input.hpp"
#include "PPU.hpp"
#endif // 8TUNES

#include "APU.hpp"
#include "CPUIO.hpp"
// #include "Cartridge/Cartridge.hpp"

struct CpuStateData {
    unsigned char ram[0x800];
    bool isIntPendng;
    bool isNMIPending;
    bool isIRQPending;
    // bool isSprDMAPending;
    Registers regs;
    ProcessorFlags flags;
    InstructionData instData;
    CPUIO io;
};

class CPU : public CpuStateData {

public:
    bool isRunning;
    // int M2;
    CPU(MemoryMapper& m);
    ~CPU();
    int run(int);
    void reset();
    void stop();
    bool isPaused;
    unsigned char read(int);
    void write(int addr, unsigned char);
    unsigned char readCode(int); // Special function for reading opcodes without advancing the clock.
    unsigned char** getPRGPointers();
    void setMemoryMappePtr(MemoryMapper*);
    void setPPUPtr(PPU*);
    void pollForInterrupts();
    void saveState(FILE* file);
    bool loadState(FILE* file);

private:
    MemoryMapper& mapper;
    Input* controller;
    PPU* ppu;
    APU* apu;
    // Cartridge &cart;
    typedef void (CPU::*rmwOp)(unsigned char&); // RMW operation function type
    typedef void (CPU::*rdOp)(unsigned char); // Read operation function type
    CPULogger* cpuLogger;
    int addressBus;
    unsigned char dataBus;
    int getAddress(int);
    void rmw(rmwOp, int);
    void rd(rdOp, int);
    void interruptSequence(int brk = 0);

    /*******************************************************/
    /*Instructions                                         */
    /*******************************************************/

    void setZN(unsigned char);
    void LDReg(unsigned char&, unsigned char);
    void NOP(unsigned char);
    void LDA(unsigned char);
    void LDX(unsigned char);
    void LDY(unsigned char);
    void EOR(unsigned char);
    void AND(unsigned char);
    void AAC(unsigned char);
    void ASR(unsigned char);
    void ARR(unsigned char);
    void ATX(unsigned char);
    void AXS(unsigned char);
    void ORA(unsigned char);
    void ADC(unsigned char);
    void SBC(unsigned char);
    void CMPReg(unsigned char, unsigned char);
    void CMP(unsigned char);
    void CPX(unsigned char);
    void CPY(unsigned char);
    void BIT(unsigned char);
    void LAX(unsigned char);
    void ANE(unsigned char);
    void LAE(unsigned char);
    // RMW Operations
    void ASL(unsigned char&);
    void LSR(unsigned char&);
    void ROL(unsigned char&);
    void ROR(unsigned char&);
    void INC(unsigned char&);
    void DEC(unsigned char&);
    void SLO(unsigned char&);
    void SRE(unsigned char&);
    void RLA(unsigned char&);
    void RRA(unsigned char&);
    void ISB(unsigned char&);
    void DCP(unsigned char&);
    // Write operations
    void STA(int);
    void STX(int);
    void STY(int);
    void SAX(int);
    // Branches
    void bra(bool condition);
    void BPL();
    void BMI();
    void BCS();
    void BCC();
    void BVC();
    void BVS();
    void BNE();
    void BEQ();
    // Misc. Operations
    void JMPAbs();
    void JMPInd();
    void JPI(int);
    void BRK();
    void JSR(int);
    void RTI();
    void RTS();
    void PHP();
    void PLP();
    void PHA();
    void PLA();
    void DEY();
    void TAY();
    void INY();
    void INX();
    void CLC();
    void SEC();
    void CLI();
    void SEI();
    void TYA();
    void CLV();
    void CLD();
    void SED();
    void TXA();
    void TXS();
    void TAX();
    void TSX();
    void DEX();
    void NOP();

    friend class NSFLoader;
    friend int main(int, char**);
    friend class ONesSamaCore;
};

#endif // CPU_HPP_INCLUDED
