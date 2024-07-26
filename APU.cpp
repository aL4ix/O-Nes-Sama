#include "APU.h"

void apuDoNothingToPushAudioSample(short left, short right)
{
}

APU::APU(CPUIO& cpuIO)
    : pushAudioSample(nullptr)
    , halfCycles(0)
    , modeFrameCounter(false)
    , inhibitFrameCounter(false)
    , irqFrameCounter(false)
    , lengthCounterPulse1(0)
    , lengthCounterPulse2(0)
    , lengthCounterTriangle(0)
    , lengthCounterNoise(0)
    , haltPulse1(false)
    , haltPulse2(false)
    , haltTriangle(false)
    , haltNoise(false)
    , enablePulse1(false)
    , enablePulse2(false)
    , enableTriangle(false)
    , enableNoise(false)
    , linearCounterReloadFlagTriangle(false)
    , counterReloadTriangle(0)
    , timerTriangle(0)
    , halfCyclesUntilTriangle(0)
    , sequencerStepTriangle(0)
    , outputTriangle(0)
    , linearCounterTriangle(0)
    , dutyPulse1(0)
    , timerPulse1(0)
    , sequencerStepPulse1(0)
    , halfCyclesUntilPulse1(0)
    , outputPulse1(0)
    , dutyPulse2(0)
    , timerPulse2(0)
    , sequencerStepPulse2(0)
    , halfCyclesUntilPulse2(0)
    , outputPulse2(0)
    , timerNoise(0)
    , halfCyclesUntilNoise(0)
    , shiftRegisterNoise(1)
    , outputNoise(0)
    , sampleAddressDMC(0)
    , sampleLengthDMC(0)
    , addressCounterDMC(0)
    , bytesRemainingCounterDMC(0)
    , sampleBufferDMC(BUFFER_EMPTY)
    , silenceFlagDMC(false)
    , bitsRemainingCounterDMC(0)
    , shiftRegisterDMC(0)
    , outputLevelDMC(0)
    , irqDMC(false)
    , rateInHalfCyclesDMC(0)
    , halfCyclesUntilNextDMC(0)
    , step(0)
    , halfCyclesUntilNextStep(0)
    , cpuIO(cpuIO)
    , board(NULL)
{
    powerup();
    reset();
    // file = fopen("hey.txt", "wb");

    lookupTablePulse[0] = 0;
    for (unsigned n = 1; n < 31; n++)
        lookupTablePulse[n] = AMPLITUDE * 95.52 / (8128.0 / n + 100);
    lookupTableTND[0] = 0;
    for (unsigned n = 1; n < 203; n++)
        lookupTableTND[n] = AMPLITUDE * 163.67 / (24329.0 / n + 100);

    setPushAudioSampleCallback(apuDoNothingToPushAudioSample);

    // Debug
    callCyclesCount = 0;
}

void APU::writeMem(unsigned short address, unsigned char value)
{
    Log.debug(LogCategory::apuWriteMem, "%X: %X", address, value);
    // pause();
    address = address & 0b11111;
    if (address <= 0x17)
        return (this->*writeFuncs[address])(value);
    /*else
        return unimpleme(0);*/
}

unsigned char APU::readMem(unsigned short address)
{
    address = address & 0b11111;
    unsigned char value = (this->*readFuncs[address])();
    Log.debug(LogCategory::apuReadMem, "%X: %X", 0x4000 + address, value);

    /*if(address == 0x15)
    {
        return value;
    }
    else
    {
        //unimpleme(0);
        return cpuIO.dataBus;
    }*/

    return value;
}

unsigned char APU::readLatch()
{
    // return 0;
    return cpuIO.dataBus;
}

void APU::unimpleme(unsigned char)
{
    // throw std::bad_function_call();
}

unsigned char APU::read4015()
{
    unsigned char reg4015 = 0;
    reg4015 |= (bytesRemainingCounterDMC > 0) << 4;
    reg4015 |= (lengthCounterNoise > 0) << 3;
    reg4015 |= (lengthCounterTriangle > 0) << 2;
    reg4015 |= (lengthCounterPulse2 > 0) << 1;
    reg4015 |= (lengthCounterPulse1 > 0);
    reg4015 |= (irqFrameCounter) << 6;
    reg4015 |= (irqDMC) << 7;
    irqFrameCounter = false;
    return reg4015;
}

void APU::write4000(unsigned char value)
{
    dutyPulse1 = (value & 0xc0) >> 6;
    haltPulse1 = value & 0x20;
    constantVolumeFlagPulse1 = value & 0x10;
    constVolEnvDivPeriodPulse1 = value & 0xf;
}

void APU::write4001(unsigned char value)
{
    dividerSweepPulse1 = (value & 0x70) >> 4;
    shiftCountPulse1 = value & 0x7;
    enableSweepFlagPulse1 = (value & 0x80) && shiftCountPulse1;
    negateFlagPulse1 = value & 0x8;
    reloadSweepPulse1 = true;

    Log.debug(LogCategory::apuWrite4001, "%X N:%u S:%u E:%u", value, negateFlagPulse1, shiftCountPulse1, enableSweepFlagPulse1);
}

void APU::write4002(unsigned char value)
{
    timerPulse1 = (timerPulse1 & 0x700) | value;
}

void APU::write4003(unsigned char value)
{
    if (enablePulse1) {
        lengthCounterPulse1 = loadLengthCounter(value);
    }
    timerPulse1 = ((value & 0x7) << 8) | (timerPulse1 & 0xFF);
    halfCyclesUntilPulse1 = timerPulse1 * 2;
    sequencerStepPulse1 = 0;
    startFlagPulse1 = true;
}

void APU::write4004(unsigned char value)
{
    dutyPulse2 = (value & 0xc0) >> 6;
    haltPulse2 = value & 0x20;
    constantVolumeFlagPulse2 = value & 0x10;
    constVolEnvDivPeriodPulse2 = value & 0xF;
}

void APU::write4005(unsigned char value)
{
    dividerSweepPulse2 = (value & 0x70) >> 4;
    shiftCountPulse2 = value & 0x7;
    enableSweepFlagPulse2 = (value & 0x80) && shiftCountPulse2;
    negateFlagPulse2 = value & 0x8;
    reloadSweepPulse2 = true;
}

void APU::write4006(unsigned char value)
{
    timerPulse2 = (timerPulse2 & 0x700) | value;
}

void APU::write4007(unsigned char value)
{
    if (enablePulse2) {
        lengthCounterPulse2 = loadLengthCounter(value);
    }
    timerPulse2 = ((value & 0x7) << 8) | (timerPulse2 & 0xFF);
    halfCyclesUntilPulse2 = timerPulse2 * 2;
    sequencerStepPulse2 = 0;
    startFlagPulse2 = true;
}

void APU::write4008(unsigned char value)
{
    haltTriangle = value & 0x80;
    counterReloadTriangle = value & 0x7f;
    Log.debug(LogCategory::apuWrite4008, "%X HC:%u", value, halfCycles);
}

void APU::write4009(unsigned char value)
{
    // Nothing to do here
}

void APU::write400A(unsigned char value)
{
    timerTriangle = (timerTriangle & 0x700) | value;
    Log.debug(LogCategory::apuWrite400A, "%X HC:%u", value, halfCycles);
}

void APU::write400B(unsigned char value)
{
    if (enableTriangle) {
        lengthCounterTriangle = loadLengthCounter(value);
    }
    timerTriangle = ((value & 0x7) << 8) | (timerTriangle & 0xFF);
    linearCounterReloadFlagTriangle = true;
    Log.debug(LogCategory::apuWrite400B, "%X HC:%u", value, halfCycles);
}

void APU::write400C(unsigned char value)
{
    haltNoise = value & 0x20;
    constantVolumeFlagNoise = value & 0x10;
    constVolEnvDivPeriodNoise = value & 0xF;
}

void APU::write400D(unsigned char value)
{
    // Nothing to do here
}

void APU::write400E(unsigned char value)
{
    modeFlagNoise = value & 0x80;
    timerNoise = periodTableNoise[value & 0xF];
    Log.debug(LogCategory::apuWrite400E, "%d", timerNoise);
}

void APU::write400F(unsigned char value)
{
    if (enableNoise) {
        lengthCounterNoise = loadLengthCounter(value);
    }
    startFlagNoise = true;
}

void APU::write4010(unsigned char value)
{
    if (value & 0x80) {
        irqEnableDMC = true;
    } else {
        irqEnableDMC = false;
        cpuIO.irq = 0;
        irqDMC = false;
    }

    loopFlagDMC = (value & 0x40);
    rateInHalfCyclesDMC = rateTableDMC[value & 0xF];
    Log.debug(LogCategory::apuWrite4010, "CUND: %d", rateInHalfCyclesDMC);
}

void APU::write4011(unsigned char value)
{
    outputLevelDMC = value & 0x7F;
}

void APU::write4012(unsigned char value)
{
    //                           value * 64
    sampleAddressDMC = 0xC000 | (value << 6);
    // sampleAddressDMC = 0x8000 + (value << 6);
}

void APU::write4013(unsigned char value)
{
    //                 value * 16
    sampleLengthDMC = (value << 4) + 1;
}

void APU::write4015(unsigned char value)
{
    if (value & 0x10) {
        if (bytesRemainingCounterDMC == 0) {
            addressCounterDMC = sampleAddressDMC;
            bytesRemainingCounterDMC = sampleLengthDMC;
            halfCyclesUntilNextDMC = 1;
            Log.debug(LogCategory::apuWrite4015, "STARTED: %d HC:%u", bytesRemainingCounterDMC, halfCycles);
        } else {
            Log.debug(LogCategory::apuWrite4015, "NOT STARTED: %d", bytesRemainingCounterDMC);
        }
    } else {
        bytesRemainingCounterDMC = 0;
    }
    irqDMC = false;

    if (value & 0x08) {
        enableNoise = true;
    } else {
        lengthCounterNoise = 0;
        enableNoise = false;
    }

    if (value & 0x04) {
        enableTriangle = true;
    } else {
        lengthCounterTriangle = 0;
        enableTriangle = false;
    }

    if (value & 0x02) {
        enablePulse2 = true;
    } else {
        lengthCounterPulse2 = 0;
        enablePulse2 = false;
    }

    if (value & 0x01) {
        enablePulse1 = true;
    } else {
        lengthCounterPulse1 = 0;
        enablePulse1 = false;
    }

    Log.debug(LogCategory::apuWrite4015, "%X", value);
}

void APU::write4017(unsigned char value)
{
    modeFrameCounter = value & 0x80;
    inhibitFrameCounter = value & 0x40;
    step = 0;

    if (modeFrameCounter == MODE_5STEP)
        halfCyclesUntilNextStep = frameCounterCycles5Step[0];
    else
        halfCyclesUntilNextStep = frameCounterCycles4Step[0];
    if (halfCycles & 1)
        halfCyclesUntilNextStep += 2;
    else
        halfCyclesUntilNextStep += 1;

    if (inhibitFrameCounter) {
        irqFrameCounter = false;
        cpuIO.irq = 0;
    }
}

void APU::process(unsigned cpuCycles)
{
    // Debug
    callCyclesCount += cpuCycles;

    for (unsigned i = 0; i < cpuCycles; i++) {
        if (halfCyclesUntilNextStep > 0)
            halfCyclesUntilNextStep--;
        if (halfCyclesUntilNextStep == 0) {
            if (modeFrameCounter)
                processMode1();
            else
                processMode0();
        }
        if (halfCyclesUntilNextDMC > 0) {
            halfCyclesUntilNextDMC--;
        }
        if (halfCyclesUntilNextDMC == 0) {
            clockDMC();
        }
        if (halfCyclesUntilTriangle > 0)
            halfCyclesUntilTriangle--;
        if (halfCyclesUntilTriangle == 0) {
            clockTriangle();
        }
        if (halfCyclesUntilPulse1 > 0)
            halfCyclesUntilPulse1--;
        if (halfCyclesUntilPulse1 == 0) {
            clockPulse1();
        }
        if (halfCyclesUntilPulse2 > 0)
            halfCyclesUntilPulse2--;
        if (halfCyclesUntilPulse2 == 0) {
            clockPulse2();
        }
        if (halfCyclesUntilNoise > 0)
            halfCyclesUntilNoise--;
        if (halfCyclesUntilNoise == 0) {
            clockNoise();
        }

        /*
        outputTriangle = 0;
        outputNoise = 0;
        outputLevelDMC = 0;
        outputPulse1 = 0;
        outputPulse2 = 0;
        */

        unsigned mixer = lookupTableTND[3 * outputTriangle + 2 * outputNoise + outputLevelDMC];
        mixer += lookupTablePulse[outputPulse1 + outputPulse2];
        if (mixer > 65535) {
            Log.debug(LogCategory::apuAll, "APU MIXER CLIPPING!!!");
            mixer = 65535;
        }
        try {
            pushAudioSample(mixer, mixer);
        } catch (const std::bad_function_call& ex) {
        }

        // fprintf(file, "%c", mixer);
        // b.count++;

        halfCycles++;
    }
}

void APU::processMode0()
{
    if (step == 0 || step == 1 || step == 2 || step == 4)
        clockQuarterFrame();
    if (step == 1 || step == 4)
        clockHalfFrame();
    if (step == 3 || step == 4 || step == 5) {
        if (!inhibitFrameCounter) {
            cpuIO.irq = 1;
            irqFrameCounter = true;
        }
    }
    halfCyclesUntilNextStep = frameCounterCycles4Step[step + 1];
    if (++step == 6)
        step = 0;
}

void APU::processMode1()
{
    if (step < 4) {
        clockQuarterFrame();
        if (step == 0 || step == 2)
            clockHalfFrame();
    }
    halfCyclesUntilNextStep = frameCounterCycles5Step[step + 1];
    if (++step == 5)
        step = 0;
}

void APU::clockDMC()
{
    Log.debug(LogCategory::apuDMC, "START: %d %d", bytesRemainingCounterDMC, bitsRemainingCounterDMC);
    halfCyclesUntilNextDMC = rateInHalfCyclesDMC;

    if (!silenceFlagDMC) {
        if (shiftRegisterDMC & 0x1) {
            if (outputLevelDMC <= 125)
                outputLevelDMC += 2;
        } else {
            if (outputLevelDMC >= 2)
                outputLevelDMC -= 2;
        }
    }
    shiftRegisterDMC >>= 1;
    --bitsRemainingCounterDMC;

    if (bitsRemainingCounterDMC == 0) // Output Cycle Ends
    {
        bitsRemainingCounterDMC = 8;
        silenceFlagDMC = (sampleBufferDMC == BUFFER_EMPTY);
        shiftRegisterDMC = sampleBufferDMC;
        sampleBufferDMC = BUFFER_EMPTY;
    }

    if (bytesRemainingCounterDMC > 0 && sampleBufferDMC == BUFFER_EMPTY)
        memoryReader();

    Log.debug(LogCategory::apuDMC, "CLOCK: %d %d", bytesRemainingCounterDMC, bitsRemainingCounterDMC);
}

void APU::memoryReader()
{
    // TODO: Stall CPU!!!
    sampleBufferDMC = board->readCPU(addressCounterDMC++); // board->cpuRead(addressCounter++);
    if (addressCounterDMC == 0)
        addressCounterDMC = 0x8000;
    --bytesRemainingCounterDMC;

    if (bytesRemainingCounterDMC == 0) {
        if (loopFlagDMC) {
            addressCounterDMC = sampleAddressDMC;
            bytesRemainingCounterDMC = sampleLengthDMC;
        } else {
            Log.debug(LogCategory::apuMemReader, "FINISHED");
            if (irqEnableDMC) {
                cpuIO.irq = 1;
                irqDMC = true;
            }
        }
    }
}

void APU::powerup()
{
    write4017(0);
    write4000(0);
    write4001(0);
    write4002(0);
    write4003(0);
    write4004(0);
    write4005(0);
    write4006(0);
    write4007(0);
    write4008(0);
    write4009(0);
    write400A(0);
    write400B(0);
    write400C(0);
    write400D(0);
    write400E(0);
    write400F(0);

    write4010(0);
}

void APU::reset()
{
    write4015(0);
}

void APU::clockHalfFrame()
{
    Log.debug(LogCategory::apuHalfFrame, "CH");

    if (haltNoise || lengthCounterNoise == 0) {
    } else
        lengthCounterNoise--;

    if (haltTriangle || lengthCounterTriangle == 0) {
    } else
        lengthCounterTriangle--;

    // Sweep Pulse1
    if (dividerSweepCounterPulse1 > 0) {
        dividerSweepCounterPulse1--;
        Log.debug(LogCategory::apuHalfFrame, "RedDiv to %u", dividerSweepCounterPulse1);
    } else {
        dividerSweepCounterPulse1 = dividerSweepPulse1;
        if (enableSweepFlagPulse1 && !isSweepSilenced(timerPulse1, negateFlagPulse1, shiftCountPulse1)) {
            if (negateFlagPulse1)
                timerPulse1 -= (timerPulse1 >> shiftCountPulse1) + 1; // +1 only for Pulse 1
            else
                timerPulse1 += (timerPulse1 >> shiftCountPulse1);
        }
        Log.debug(LogCategory::apuHalfFrame, "Timer: %u", timerPulse1);
    }
    if (reloadSweepPulse1) {
        /*
           if(dividerSweepCounterPulse1 == 0 && enableSweepFlagPulse1)
           {
               //period is also adjusted??
           }
           else*/
        dividerSweepCounterPulse1 = dividerSweepPulse1;
        reloadSweepPulse1 = false;
        Log.debug(LogCategory::apuHalfFrame, "SetDiv to %u", dividerSweepCounterPulse1);
    }

    // Length Pulse 1
    if (haltPulse1 || lengthCounterPulse1 == 0) {
    } else
        lengthCounterPulse1--;

    // Sweep Pulse 2
    if (dividerSweepCounterPulse2 > 0)
        dividerSweepCounterPulse2--;
    else {
        dividerSweepCounterPulse2 = dividerSweepPulse2;
        if (enableSweepFlagPulse2 && !isSweepSilenced(timerPulse2, negateFlagPulse2, shiftCountPulse2)) {
            if (negateFlagPulse2)
                timerPulse2 -= (timerPulse2 >> shiftCountPulse2); // +1 only for Pulse 1
            else
                timerPulse2 += (timerPulse2 >> shiftCountPulse2);
        }
    }
    if (reloadSweepPulse2) {
        /*
           if(dividerSweepCounterPulse2 == 0 && enableSweepFlagPulse2)
           {
               //period is also adjusted??
           }
           else*/
        dividerSweepCounterPulse2 = dividerSweepPulse2;
        reloadSweepPulse2 = false;
    }

    // Length Pulse 2
    if (haltPulse2 || lengthCounterPulse2 == 0) {
    } else
        lengthCounterPulse2--;
}

void APU::clockQuarterFrame()
{
    Log.debug(LogCategory::apuQuarterFrame, "CQ");
    if (linearCounterReloadFlagTriangle) {
        linearCounterTriangle = counterReloadTriangle;
    } else if (linearCounterTriangle > 0) {
        --linearCounterTriangle;
    }
    if (!haltTriangle)
        linearCounterReloadFlagTriangle = false;

    // Pulse 1
    if (startFlagPulse1 == false) {
        if (decayDividerCounterPulse1 > 0)
            decayDividerCounterPulse1--;
        else {
            decayDividerCounterPulse1 = constVolEnvDivPeriodPulse1;
            if (envelopeVolumePulse1 > 0)
                envelopeVolumePulse1--;
            else if (haltPulse1) // loop flag
            {
                envelopeVolumePulse1 = 15;
            }
        }
    } else {
        startFlagPulse1 = false;
        envelopeVolumePulse1 = 15;
        decayDividerCounterPulse1 = constVolEnvDivPeriodPulse1;
    }

    // Pulse2
    if (startFlagPulse2 == false) {
        if (decayDividerCounterPulse2 > 0)
            decayDividerCounterPulse2--;
        else {
            decayDividerCounterPulse2 = constVolEnvDivPeriodPulse2;
            if (envelopeVolumePulse2 > 0)
                envelopeVolumePulse2--;
            else if (haltPulse2) // loop flag
            {
                envelopeVolumePulse2 = 15;
            }
        }
    } else {
        startFlagPulse2 = false;
        envelopeVolumePulse2 = 15;
        decayDividerCounterPulse2 = constVolEnvDivPeriodPulse2;
    }

    // Noise
    if (startFlagNoise == false) {
        if (decayDividerCounterNoise > 0)
            decayDividerCounterNoise--;
        else {
            decayDividerCounterNoise = constVolEnvDivPeriodNoise;
            if (envelopeVolumeNoise > 0)
                envelopeVolumeNoise--;
            else if (haltNoise) // loop flag
            {
                envelopeVolumeNoise = 15;
            }
        }
    } else {
        startFlagNoise = false;
        envelopeVolumeNoise = 15;
        decayDividerCounterNoise = constVolEnvDivPeriodNoise;
    }
}

unsigned char APU::loadLengthCounter(unsigned char value)
{
    return lengthCounterTable[value >> 3];
}

void APU::clockTriangle()
{
    halfCyclesUntilTriangle = timerTriangle;
    if (lengthCounterTriangle > 0 && linearCounterTriangle > 0 && timerTriangle >= 2) {
        outputTriangle = sequencerTriangle[sequencerStepTriangle++];
        sequencerStepTriangle &= 31;
        Log.debug(LogCategory::apuTriangle, "%d %d", halfCyclesUntilTriangle, sequencerStepTriangle);
    }
}

void APU::clockPulse1()
{
    halfCyclesUntilPulse1 = timerPulse1 * 2;
    sequencerStepPulse1++;
    sequencerStepPulse1 &= 7;
    if (lengthCounterPulse1 > 0
        && !isSweepSilenced(timerPulse1, negateFlagPulse1, shiftCountPulse1)
        && sequencerPulse[dutyPulse1][sequencerStepPulse1]) {
        outputPulse1 = (constantVolumeFlagPulse1) ? constVolEnvDivPeriodPulse1 : envelopeVolumePulse1;
    } else
        outputPulse1 = 0;

    Log.debug(LogCategory::apuPulse1, "%d %d", halfCyclesUntilPulse1, sequencerStepPulse1);
}

void APU::clockPulse2()
{
    halfCyclesUntilPulse2 = timerPulse2 * 2;
    sequencerStepPulse2++;
    sequencerStepPulse2 &= 7;
    if (lengthCounterPulse2 > 0
        && !isSweepSilenced(timerPulse2, negateFlagPulse2, shiftCountPulse2)
        && sequencerPulse[dutyPulse2][sequencerStepPulse2]) {
        outputPulse2 = (constantVolumeFlagPulse2) ? constVolEnvDivPeriodPulse2 : envelopeVolumePulse2;
    } else
        outputPulse2 = 0;

    Log.debug(LogCategory::apuPulse2, "%d %d", halfCyclesUntilPulse2, sequencerStepPulse2);
}

void APU::clockNoise()
{
    halfCyclesUntilNoise = timerNoise;
    Log.debug(LogCategory::apuNoise, "SH: %X", shiftRegisterNoise);
    // getchar();
    bool feedback = shiftRegisterNoise & 0x1;
    // XOR with bit 6 if modeFlag else bit 1
    feedback ^= (modeFlagNoise) ? (shiftRegisterNoise & 0x40) != 0 : (shiftRegisterNoise & 0x2) != 0;
    shiftRegisterNoise >>= 1;
    shiftRegisterNoise |= feedback << 14;
    Log.debug(LogCategory::apuNoise, "SH: %X", shiftRegisterNoise);
    if (((shiftRegisterNoise & 0x1) == 0) && lengthCounterNoise > 0) {
        outputNoise = (constantVolumeFlagNoise) ? constVolEnvDivPeriodNoise : envelopeVolumeNoise;
    } else
        outputNoise = 0;
}

bool APU::isSweepSilenced(unsigned short timer, bool negate, unsigned char shiftCount)
{
    return (timer < 8 || (!negate && (timer + (timer >> shiftCount) > 0x7FF)));
}

void APU::setMemoryMapper(MemoryMapper* board)
{
    this->board = board;
}

void APU::setPushAudioSampleCallback(void (*pushAudioSampleCallback)(short left, short right))
{
    pushAudioSample = pushAudioSampleCallback;
}
