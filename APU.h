#ifndef APU_H_INCLUDED
#define APU_H_INCLUDED

#include <functional>
#include <cstdio>
#include <queue>
#include <SDL2/SDL.h>

#include "x6502Interrupts.h"
#include "Cartridge/Boards/Board.hpp"


const int AMPLITUDE = 28000;
const int SAMPLING = 48000;


class Beeper
{
private:
    double acc;
    double parte;
    unsigned entero;
    std::queue<unsigned short> beeps;
public:
    Beeper();
    ~Beeper();
    void load(unsigned short sample);
    void loadSamples(Uint16 *stream, int length);
    void wait();

    unsigned long long count;
};


void audio_callback(void*, Uint8*, int);


class APU
{
public:
    explicit APU(InterruptLines &ints);
    void writeMem(unsigned short Address, unsigned char Value);
    unsigned char readMem(unsigned short Address);

    void (APU::*writeFuncs[24])(unsigned char) = {&APU::write4000, &APU::write4001, &APU::write4002, &APU::write4003, &APU::write4004, &APU::write4005, &APU::write4006, &APU::write4007,
                                                  &APU::write4008, &APU::write4009, &APU::write400A, &APU::write400B, &APU::write400C, &APU::write400D, &APU::write400E, &APU::write400F,
                                                  &APU::write4010, &APU::write4011, &APU::write4012, &APU::write4013, &APU::unimpleme, &APU::write4015, &APU::unimpleme, &APU::write4017};
                                                 //0             1                   2               3               4                   5               6               7
    unsigned char (APU::*readFuncs[24])(void) = {&APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch,
                                                 //8             9                   A               B               C                   D               E               F
                                                 &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch,
                                                 //10            11                  12              13              14                  15              16              17
                                                 &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::readLatch, &APU::read4015,  &APU::readLatch, &APU::readLatch};
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
    void setMemoryMapper(Board* Board);
    Beeper b;

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
    unsigned short halfCycles;
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
    FILE* file;

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
    unsigned char timerNoise;
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
    const unsigned short lookupTablePulse[31] = {0, 174, 344, 510, 672, 830, 984, 1136, 1283, 1428, 1569, 1707, 1843, 1975, 2105, 2232, 2356, 2478, 2597, 2714, 2829, 2941, 3052, 3160, 3266, 3370, 3472, 3572, 3671, 3767, 3862};
    const unsigned short lookupTableTND[203] = {0, 100, 200, 299, 397, 494, 590, 686, 781, 875, 969, 1061, 1154, 1245, 1335, 1425, 1514, 1603, 1691, 1778, 1864, 1950, 2035, 2120, 2204, 2287, 2370, 2452, 2533, 2614, 2694, 2774, 2853, 2932, 3010, 3087, 3164, 3240, 3316, 3391, 3466, 3540, 3614, 3687, 3760, 3832, 3903, 3974, 4045, 4115, 4185, 4254, 4323, 4391, 4459, 4526, 4593, 4660, 4726, 4791, 4856, 4921, 4985, 5049, 5113, 5176, 5238, 5301, 5362, 5424, 5485, 5546, 5606, 5666, 5725, 5784, 5843, 5902, 5960, 6017, 6075, 6132, 6188, 6245, 6300, 6356, 6411, 6466, 6521, 6575, 6629, 6683, 6736, 6789, 6842, 6894, 6946, 6998, 7049, 7100, 7151, 7202, 7252, 7302, 7351, 7401, 7450, 7499, 7547, 7596, 7644, 7691, 7739, 7786, 7833, 7879, 7926, 7972, 8018, 8064, 8109, 8154, 8199, 8244, 8288, 8332, 8376, 8420, 8463, 8506, 8549, 8592, 8635, 8677, 8719, 8761, 8802, 8844, 8885, 8926, 8967, 9007, 9048, 9088, 9128, 9167, 9207, 9246, 9285, 9324, 9363, 9402, 9440, 9478, 9516, 9554, 9591, 9629, 9666, 9703, 9740, 9776, 9813, 9849, 9885, 9921, 9957, 9992, 10028, 10063, 10098, 10133, 10168, 10202, 10236, 10271, 10305, 10339, 10372, 10406, 10439, 10473, 10506, 10539, 10571, 10604, 10637, 10669, 10701, 10733, 10765, 10797, 10828, 10860, 10891, 10922, 10953, 10984, 11015, 11046, 11076, 11106, 11137};

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
    InterruptLines &ints;
    Board* board;

};

#endif // APU_H_INCLUDED
