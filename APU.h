#ifndef APU_H_INCLUDED
#define APU_H_INCLUDED

#include <cstdio>

#include "CPUIO.hpp"
#include "Cartridge/Mappers/MemoryMapper.h"
#include "RetroAudio.hpp"


class APU
{
    const unsigned short AMPLITUDE = 30000;

    friend int main();

public:
    explicit APU(CPUIO &cpuIO);
    void writeMem(unsigned short Address, unsigned char Value);
    unsigned char readMem(unsigned short Address);

    void (APU::*writeFuncs[24])(unsigned char) = {&APU::write4000, &APU::write4001, &APU::write4002, &APU::write4003, &APU::write4004, &APU::write4005, &APU::write4006, &APU::write4007,
                                                  &APU::write4008, &APU::write4009, &APU::write400A, &APU::write400B, &APU::write400C, &APU::write400D, &APU::write400E, &APU::write400F,
                                                  &APU::write4010, &APU::write4011, &APU::write4012, &APU::write4013, &APU::unimpleme, &APU::write4015, &APU::unimpleme, &APU::write4017};
                                                 //0             1                   2               3               4                   5               6               7
    unsigned char (APU::*readFuncs[32])(void) = {&APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch,
                                                 //8             9                   A               B               C                   D               E               F
                                                 &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch,
                                                 //10            11                  12              13              14                  15              16              17
                                                 &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::read4015,  &APU::readLatch, &APU::readLatch,
                                                 //18            19                  1A              1B              1C                  1D              1E              1F
                                                 &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch,  &APU::readLatch, &APU::readLatch
                                                 };
    unsigned char readLatch();
    void unimpleme(unsigned char); //14 16
    unsigned char read4015();
    void write4000(unsigned char);
    void write4001(unsigned char);
    void write4002(unsigned char);
    void write4003(unsigned char);
    void write4004(unsigned char);
    void write4005(unsigned char);
    void write4006(unsigned char);
    void write4007(unsigned char);
    void write4008(unsigned char);
    void write4009(unsigned char);
    void write400A(unsigned char);
    void write400B(unsigned char);
    void write400C(unsigned char);
    void write400D(unsigned char);
    void write400E(unsigned char);
    void write400F(unsigned char);
    void write4010(unsigned char);
    void write4011(unsigned char);
    void write4012(unsigned char);
    void write4013(unsigned char);
    void write4015(unsigned char);
    void write4017(unsigned char);
    void process(unsigned cpuCycles);
    void powerup();
    void reset();

    //Other
    void setMemoryMapper(MemoryMapper* Board);
    RetroAudio afx;

    //Debug
    unsigned callCyclesCount;

private:
    const bool MODE_4STEP = false;
    const bool MODE_5STEP = true;
    const short BUFFER_EMPTY = -1;
    /*
    tableLookup =
         |  0   1   2   3   4   5   6   7    8   9   A   B   C   D   E   F
    -----+----------------------------------------------------------------
    00-0F  10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
    10-1F  12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    */
    unsigned char lengthCounterTable[32] =
    {
        10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
        12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    };
    /*
    Rate   $0   $1   $2   $3   $4   $5   $6   $7   $8   $9   $A   $B   $C   $D   $E   $F
          ------------------------------------------------------------------------------
    NTSC  428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
    PAL   398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118,  98,  78,  66,  50
    */
    unsigned short rateTableDMC[16] =
    {
        428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
    };
    unsigned halfCycles;
    bool modeFrameCounter;
    bool inhibitFrameCounter;
    bool irqFrameCounter;
    //unsigned char delayResetTimer; ///TO DELETE?
    unsigned char lengthCounterPulse1;
    unsigned char lengthCounterPulse2;
    unsigned char lengthCounterTriangle;
    unsigned char lengthCounterNoise;
    bool haltPulse1;
    bool haltPulse2;
    bool haltTriangle;
    bool haltNoise;
    bool enablePulse1;
    bool enablePulse2;
    bool enableTriangle;
    bool enableNoise;

    //Triangle
    bool linearCounterReloadFlagTriangle;
    unsigned char counterReloadTriangle;
    unsigned short timerTriangle;
    unsigned short halfCyclesUntilTriangle;
    unsigned char sequencerStepTriangle;
    unsigned char outputTriangle;
    unsigned char linearCounterTriangle;
    unsigned char sequencerTriangle[32] = {15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
                                           0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15};
    //FILE* file;

    //Pulse1
    unsigned char sequencerPulse[4][8] = {{0, 1, 0, 0, 0, 0, 0, 0},
                                         {0, 1, 1, 0, 0, 0, 0, 0},
                                         {0, 1, 1, 1, 1, 0, 0, 0},
                                         {1, 0, 0, 1, 1, 1, 1, 1}};
    unsigned char dutyPulse1;
    unsigned short timerPulse1;
    unsigned char sequencerStepPulse1;
    unsigned short halfCyclesUntilPulse1;
    unsigned char outputPulse1;
    bool constantVolumeFlagPulse1;
    unsigned char constVolEnvDivPeriodPulse1;
    bool startFlagPulse1;
    unsigned char envelopeVolumePulse1;
    unsigned char decayDividerCounterPulse1;
    bool enableSweepFlagPulse1;
    unsigned char dividerSweepPulse1;
    bool negateFlagPulse1;
    unsigned char shiftCountPulse1;
    bool reloadSweepPulse1;
    unsigned char dividerSweepCounterPulse1;

    //Pulse2
    unsigned char dutyPulse2;
    unsigned short timerPulse2;
    unsigned char sequencerStepPulse2;
    unsigned short halfCyclesUntilPulse2;
    unsigned char outputPulse2;
    bool constantVolumeFlagPulse2;
    unsigned char constVolEnvDivPeriodPulse2;
    bool startFlagPulse2;
    unsigned char envelopeVolumePulse2;
    unsigned char decayDividerCounterPulse2;
    bool enableSweepFlagPulse2;
    unsigned char dividerSweepPulse2;
    bool negateFlagPulse2;
    unsigned char shiftCountPulse2;
    bool reloadSweepPulse2;
    unsigned char dividerSweepCounterPulse2;

    //Noise
    unsigned short periodTableNoise[16] = {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
    bool constantVolumeFlagNoise;
    unsigned char constVolEnvDivPeriodNoise;
    bool modeFlagNoise;
    unsigned short timerNoise;
    bool startFlagNoise;
    unsigned char decayDividerCounterNoise;
    unsigned char envelopeVolumeNoise;
    unsigned short halfCyclesUntilNoise;
    unsigned short shiftRegisterNoise;
    unsigned char outputNoise;

    //DMC
    unsigned short sampleAddressDMC;
    unsigned short sampleLengthDMC;
    unsigned short addressCounterDMC;
    unsigned short bytesRemainingCounterDMC;
    short sampleBufferDMC;
    bool irqEnableDMC;
    bool loopFlagDMC;
    bool silenceFlagDMC;
    unsigned char bitsRemainingCounterDMC;
    unsigned char shiftRegisterDMC;
    unsigned char outputLevelDMC;
    bool irqDMC;
    unsigned short rateInHalfCyclesDMC;
    unsigned short halfCyclesUntilNextDMC;

    //Frame Counter
    unsigned char step;
    unsigned short halfCyclesUntilNextStep;
    const short frameCounterCycles4Step[7] = {7459, 7456, 7458, 7457, 1, 1, 7457};
    const short frameCounterCycles5Step[6] = {1, 7458, 7456, 7458, 7456, 7454};

    //Mixer
    unsigned short lookupTablePulse[31];
    unsigned short lookupTableTND[203];

    void clockHalfFrame();
    void clockQuarterFrame();
    unsigned char loadLengthCounter(unsigned char Value);
    void processMode0();
    void processMode1();
    void clockDMC();
    void memoryReader();

    void clockTriangle();
    void clockPulse1();
    void clockPulse2();
    void clockNoise();

    bool isSweepSilenced(unsigned short Timer, bool Negate, unsigned char ShiftCount);

    //OTHER
    CPUIO &cpuIO;
    MemoryMapper* board;
};

#endif // APU_H_INCLUDED
