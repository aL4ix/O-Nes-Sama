#include "APU.h"


Beeper::Beeper()
{
    parte = (double(21477272)/12) / SAMPLING;
    acc = parte;
    entero = 0;
    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = 0;

    desiredSpec.freq = SAMPLING;
    desiredSpec.format = AUDIO_U16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 1024;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;
    printf("FREq: %d %d\n", desiredSpec.freq, obtainedSpec.freq);

    // you might want to look for errors here
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);

    // start play audio
    SDL_PauseAudio(0);
}

Beeper::~Beeper()
{
    SDL_CloseAudio();
}

void Beeper::loadSamples(Uint16 *stream, int length)
{
    int i = 0;
    while (i < length) {
        if (beeps.empty()) {
            //printf("U ");
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }
        else
        {
            //printf("SI\n");
            unsigned short c = beeps.front();
            beeps.pop();
            stream[i] = c;
            i++;
        }
    }
    //printf("%d ", beeps.size());
    if(beeps.size() >= 1024*5)
    {
        for(unsigned a=0; a<1024; a++)
            beeps.pop();
        //std::queue<unsigned char> empty;
        //std::swap( beeps, empty );
    }
}

void Beeper::load(unsigned short sample)
{
    if(entero++ == unsigned(acc))
    {
        acc -= entero;
        acc += parte;
        entero = 0;
        SDL_LockAudio();
        beeps.push(sample);
        SDL_UnlockAudio();
    }
}

void Beeper::wait()
{
    int size;
    do {
        SDL_Delay(20);
        SDL_LockAudio();
        size = beeps.size();
        SDL_UnlockAudio();
    } while (size > 0);
}

void audio_callback(void *_beeper, Uint8 *_streamInBytes, int _lengthInBytes)
{
    Uint16 *stream = (Uint16*) _streamInBytes;
    int length = _lengthInBytes / 2;
    Beeper* beeper = (Beeper*) _beeper;

    beeper->loadSamples(stream, length);
}


APU::APU(CPUIO &cpuIO) :
    halfCycles(0), modeFrameCounter(false), inhibitFrameCounter(false), irqFrameCounter(false),
    lengthCounterPulse1(0), lengthCounterPulse2(0), lengthCounterTriangle(0), lengthCounterNoise(0),
    haltPulse1(false), haltPulse2(false), haltTriangle(false), haltNoise(false),
    enablePulse1(false), enablePulse2(false), enableTriangle(false), enableNoise(false),
    linearCounterReloadFlagTriangle(false), counterReloadTriangle(0), timerTriangle(0),
    halfCyclesUntilTriangle(0), sequencerStepTriangle(0), outputTriangle(0),
    linearCounterTriangle(0),
    dutyPulse1(0), timerPulse1(0), sequencerStepPulse1(0), halfCyclesUntilPulse1(0),
    outputPulse1(0),
    dutyPulse2(0), timerPulse2(0), sequencerStepPulse2(0), halfCyclesUntilPulse2(0),
    outputPulse2(0),
    timerNoise(0), halfCyclesUntilNoise(0), shiftRegisterNoise(1), outputNoise(0),
    sampleAddressDMC(0), sampleLengthDMC(0), addressCounterDMC(0), bytesRemainingCounterDMC(0),
    sampleBufferDMC(BUFFER_EMPTY), silenceFlagDMC(false), bitsRemainingCounterDMC(0),
    shiftRegisterDMC(0), outputLevelDMC(0), irqDMC(false), rateInHalfCyclesDMC(0),
    halfCyclesUntilNextDMC(0), step(0), halfCyclesUntilNextStep(0)
    ,cpuIO(cpuIO), board(NULL)
{
    powerup();
    reset();
    file = fopen("hey.txt", "wb");

    lookupTablePulse[0] = 0;
    for(unsigned n=1; n<31; n++)
        lookupTablePulse[n] = AMPLITUDE * 95.52 / (8128.0 / n + 100);
    lookupTableTND[0] = 0;
    for(unsigned n=1; n<203; n++)
        lookupTableTND[n] = AMPLITUDE * 163.67 / (24329.0 / n + 100);
}

void APU::writeMem(unsigned short Address, unsigned char Value)
{
    //printf("W%X: %X\n", Address, Value);
    //pause();
    if(Address == 0x4017)
    {
        //printf("");
    }
    Address = Address & 0b11111;
    if(Address <= 0x17)
        return (this->*writeFuncs[Address])(Value);
    /*else
        return unimpleme(0);*/
}

unsigned char APU::readMem(unsigned short Address)
{

    Address = Address & 0b11111;
    /*if(Address == 0x15)
    {
        auto Value = (this->*readFuncs[Address])();
        printf("\nR%X: %X\n", 0x4000+Address, Value);
        return Value;
    }
    else
    {
        //unimpleme(0);
        return cpuIO.dataBus;
    }*/

    return (this->*readFuncs[Address])();
}

unsigned char APU::readLatch()
{
    //return 0;
    return cpuIO.dataBus;
}

void APU::unimpleme(unsigned char)
{
    throw std::bad_function_call();
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

void APU::write4000(unsigned char Value)
{
    dutyPulse1 = (Value & 0xc0) >> 6;
    haltPulse1 = Value & 0x20;
    constantVolumeFlagPulse1 = Value & 0x10;
    constVolEnvDivPeriodPulse1 = Value & 0xf;
}

void APU::write4001(unsigned char Value)
{
    dividerSweepPulse1 = (Value & 0x70) >> 4;
    shiftCountPulse1 = Value & 0x7;
    enableSweepFlagPulse1 = (Value & 0x80) && shiftCountPulse1;
    negateFlagPulse1 = Value & 0x8;
    reloadSweepPulse1 = true;
}

void APU::write4002(unsigned char Value)
{
    timerPulse1 = (timerPulse1 & 0x700) | Value;
}

void APU::write4003(unsigned char Value)
{
    if(enablePulse1)
    {
        lengthCounterPulse1 = loadLengthCounter(Value);
    }
    timerPulse1 = ((Value & 0x7) << 8) | (timerPulse1 & 0xFF);
    halfCyclesUntilPulse1 = timerPulse1 * 2;
    sequencerStepPulse1 = 0;
    startFlagPulse1 = true;
}

void APU::write4004(unsigned char Value)
{
    dutyPulse2 = (Value & 0xc0) >> 6;
    haltPulse2 = Value & 0x20;
    constantVolumeFlagPulse2 = Value & 0x10;
    constVolEnvDivPeriodPulse2 = Value & 0xF;
}

void APU::write4005(unsigned char Value)
{
    dividerSweepPulse2 = (Value & 0x70) >> 4;
    shiftCountPulse2 = Value & 0x7;
    enableSweepFlagPulse2 = (Value & 0x80) && shiftCountPulse2;
    negateFlagPulse2 = Value & 0x8;
    reloadSweepPulse2 = true;
}

void APU::write4006(unsigned char Value)
{
    timerPulse2 = (timerPulse2 & 0x700) | Value;
}

void APU::write4007(unsigned char Value)
{
    if(enablePulse2)
    {
        lengthCounterPulse2 = loadLengthCounter(Value);
    }
    timerPulse2 = ((Value & 0x7) << 8) | (timerPulse2 & 0xFF);
    halfCyclesUntilPulse2 = timerPulse2 * 2;
    sequencerStepPulse2 = 0;
    startFlagPulse2 = true;
}

void APU::write4008(unsigned char Value)
{
    haltTriangle = Value & 0x80;
    counterReloadTriangle = Value & 0x7f;
}

void APU::write4009(unsigned char Value)
{
    //Nothing to do here
}

void APU::write400A(unsigned char Value)
{
    timerTriangle = (timerTriangle & 0x700) | Value;
}

void APU::write400B(unsigned char Value)
{
    if(enableTriangle)
    {
        lengthCounterTriangle = loadLengthCounter(Value);
    }
    timerTriangle = ((Value & 0x7) << 8) | (timerTriangle & 0xFF);
    linearCounterReloadFlagTriangle = true;
}

void APU::write400C(unsigned char Value)
{
    haltNoise = Value & 0x20;
    constantVolumeFlagNoise = Value & 0x10;
    constVolEnvDivPeriodNoise = Value & 0xF;
}

void APU::write400D(unsigned char Value)
{
    //Nothing to do here
}

void APU::write400E(unsigned char Value)
{
    modeFlagNoise = Value & 0x80;
    timerNoise = periodTableNoise[Value & 0xF];
    //printf("%d ", timerNoise);
}

void APU::write400F(unsigned char Value)
{
    if(enableNoise)
    {
        lengthCounterNoise = loadLengthCounter(Value);
    }
    startFlagNoise = true;
}

void APU::write4010(unsigned char Value)
{
    if(Value & 0x80)
    {
        irqEnableDMC = true;
    }
    else
    {
        irqEnableDMC = false;
        cpuIO.irq = 0;
        irqDMC = false;
    }

    loopFlagDMC = (Value & 0x40);
    rateInHalfCyclesDMC = rateTableDMC[Value & 0xF];
    //printf("CUND: %d\n", rateInHalfCycles);
}

void APU::write4011(unsigned char Value)
{
    outputLevelDMC = Value & 0x7F;
}

void APU::write4012(unsigned char Value)
{//                            Value * 64
    sampleAddressDMC = 0xC000 | (Value << 6);
    //sampleAddressDMC = 0x8000 + (Value << 6);
}

void APU::write4013(unsigned char Value)
{//                          Value * 16
    sampleLengthDMC = (Value << 4) + 1;
}

void APU::write4015(unsigned char Value)
{
    if(Value & 0x10)
    {
        if(bytesRemainingCounterDMC == 0)
        {
            addressCounterDMC = sampleAddressDMC;
            bytesRemainingCounterDMC = sampleLengthDMC;
            halfCyclesUntilNextDMC = 1;
            //printf("STARTED: %d\n", bytesRemainingCounter, halfCycles);

        }
        else
        {
            //printf("NOT STARTED: %d\n", bytesRemainingCounter);
        }
    }
    else
    {
        bytesRemainingCounterDMC = 0;
    }
    irqDMC = false;

    if(Value & 0x08)
    {
        enableNoise = true;
    }
    else
    {
        lengthCounterNoise = 0;
        enableNoise = false;
    }

    if(Value & 0x04)
    {
        enableTriangle = true;
    }
    else
    {
        lengthCounterTriangle = 0;
        enableTriangle = false;
    }

    if(Value & 0x02)
    {
        enablePulse2 = true;
    }
    else
    {
        lengthCounterPulse2 = 0;
        enablePulse2 = false;
    }

    if(Value & 0x01)
    {
        enablePulse1 = true;
    }
    else
    {
        lengthCounterPulse1 = 0;
        enablePulse1 = false;
    }
}

void APU::write4017(unsigned char Value)
{
    modeFrameCounter = Value & 0x80;
    inhibitFrameCounter = Value & 0x40;
    step = 0;

    if(modeFrameCounter == MODE_5STEP)
        halfCyclesUntilNextStep = frameCounterCycles5Step[0];
    else
        halfCyclesUntilNextStep = frameCounterCycles4Step[0];
    if(halfCycles & 1)
        halfCyclesUntilNextStep += 2;
    else
        halfCyclesUntilNextStep += 1;

    if(inhibitFrameCounter)
    {
        irqFrameCounter = false;
        cpuIO.irq = 0;
    }
}

void APU::process(unsigned cpuCycles)
{
    for(unsigned i=0; i<cpuCycles; i++)
    {
        if(halfCyclesUntilNextStep > 0)
            halfCyclesUntilNextStep--;
        if(halfCyclesUntilNextStep == 0)
        {
            if(modeFrameCounter)
                processMode1();
            else
                processMode0();
        }
        if(halfCyclesUntilNextDMC > 0)
        {
            halfCyclesUntilNextDMC--;
        }
        if(halfCyclesUntilNextDMC == 0)
        {
            clockDMC();
        }
        if(halfCyclesUntilTriangle > 0)
            halfCyclesUntilTriangle--;
        if(halfCyclesUntilTriangle == 0)
        {
            clockTriangle();
        }
        if(halfCyclesUntilPulse1 > 0)
            halfCyclesUntilPulse1--;
        if(halfCyclesUntilPulse1 == 0)
        {
            clockPulse1();
        }
        if(halfCyclesUntilPulse2 > 0)
            halfCyclesUntilPulse2--;
        if(halfCyclesUntilPulse2 == 0)
        {
            clockPulse2();
        }
        if(halfCyclesUntilNoise > 0)
            halfCyclesUntilNoise--;
        if(halfCyclesUntilNoise == 0)
        {
            clockNoise();
        }
        unsigned mixer = lookupTableTND[3*outputTriangle + 2*outputNoise + outputLevelDMC];
        mixer += lookupTablePulse[outputPulse1 + outputPulse2];
        if(mixer > 65535)
        {
            printf("CLIPPING!!!\n");
            mixer = 65535;
        }
        b.load(mixer);

        //fprintf(file, "%c", mixer);
        //b.count++;
        //printf("%d ", outputNoise);

        halfCycles++;
    }
}

void APU::processMode0()
{
    if (step == 0 || step == 1 || step == 2 || step == 4)
        clockQuarterFrame();
    if (step == 1 || step == 4)
        clockHalfFrame();
    if (step == 3 || step == 4 || step == 5)
    {
        if(!inhibitFrameCounter)
        {
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
    if (step < 4)
    {
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
    //printf("START: %d %d\n", bytesRemainingCounter, bitsRemainingCounter);
    halfCyclesUntilNextDMC = rateInHalfCyclesDMC;

    if(!silenceFlagDMC)
    {
        if(shiftRegisterDMC & 0x1)
        {
            if(outputLevelDMC <= 125)
                outputLevelDMC += 2;
        }
        else
        {
            if(outputLevelDMC >= 2)
                outputLevelDMC -= 2;
        }
    }
    shiftRegisterDMC >>= 1;
    --bitsRemainingCounterDMC;

    if(bitsRemainingCounterDMC == 0) // Output Cycle Ends
    {
        bitsRemainingCounterDMC = 8;
        silenceFlagDMC = (sampleBufferDMC == BUFFER_EMPTY);
        shiftRegisterDMC = sampleBufferDMC;
        sampleBufferDMC = BUFFER_EMPTY;
    }

    if(bytesRemainingCounterDMC > 0 && sampleBufferDMC == BUFFER_EMPTY)
        memoryReader();

    //printf("CLOCK: %d %d\n", bytesRemainingCounter, bitsRemainingCounter);
}

void APU::memoryReader()
{
    //TODO: Stall CPU!!!
    sampleBufferDMC = board->readCPU(addressCounterDMC++); // board->cpuRead(addressCounter++);
    if(addressCounterDMC == 0)
        addressCounterDMC = 0x8000;
    --bytesRemainingCounterDMC;

    if(bytesRemainingCounterDMC == 0)
    {
        if(loopFlagDMC)
        {
            addressCounterDMC = sampleAddressDMC;
            bytesRemainingCounterDMC = sampleLengthDMC;
        }
        else
        {
            //printf("FINISHED\n");
            if(irqEnableDMC)
            {
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
    //printf("CH\n");

    if(haltNoise || lengthCounterNoise == 0)
    {
    }
    else
        lengthCounterNoise--;

    if(haltTriangle || lengthCounterTriangle == 0)
    {
    }
    else
        lengthCounterTriangle--;

    //Sweep Pulse1
    if(reloadSweepPulse1)
    {/*
        if(dividerSweepCounterPulse1 == 0 && enableSweepFlagPulse1)
        {
            //period is also adjusted??
        }
        else*/
        dividerSweepCounterPulse1 = dividerSweepPulse1;
        reloadSweepPulse1 = false;
    }
    else if(dividerSweepCounterPulse1 > 0)
        dividerSweepCounterPulse1--;
    else
    {
        dividerSweepCounterPulse1 = dividerSweepPulse1;
        if(enableSweepFlagPulse1 && !isSweepSilenced(timerPulse1, negateFlagPulse1, shiftCountPulse1))
        {
            if(negateFlagPulse1)
                timerPulse1 -= (timerPulse1 >> shiftCountPulse1) + 1; // +1 only for Pulse 1
            else
                timerPulse1 += (timerPulse1 >> shiftCountPulse1);
        }
    }

    //Length Pulse 1
    if(haltPulse1 || lengthCounterPulse1 == 0)
    {
    }
    else
        lengthCounterPulse1--;

    //Sweep Pulse 2
    if(reloadSweepPulse2)
    {/*
        if(dividerSweepCounterPulse2 == 0 && enableSweepFlagPulse2)
        {
            //period is also adjusted??
        }
        else*/
        dividerSweepCounterPulse2 = dividerSweepPulse2;
        reloadSweepPulse2 = false;
    }
    else if(dividerSweepCounterPulse2 > 0)
        dividerSweepCounterPulse2--;
    else
    {
        dividerSweepCounterPulse2 = dividerSweepPulse2;
        if(enableSweepFlagPulse2 && !isSweepSilenced(timerPulse2, negateFlagPulse2, shiftCountPulse2))
        {
            if(negateFlagPulse2)
                timerPulse2 -= (timerPulse2 >> shiftCountPulse2); // +1 only for Pulse 1
            else
                timerPulse2 += (timerPulse2 >> shiftCountPulse2);
        }
    }

    //Length Pulse 2
    if(haltPulse2 || lengthCounterPulse2 == 0)
    {
    }
    else
        lengthCounterPulse2--;
}

void APU::clockQuarterFrame()
{
    //printf("CQ\n");
    if(linearCounterReloadFlagTriangle)
    {
        linearCounterTriangle = counterReloadTriangle;
    }
    else if(linearCounterTriangle > 0)
    {
        --linearCounterTriangle;
    }
    if(!haltTriangle)
        linearCounterReloadFlagTriangle = false;

    //Pulse 1
    if(startFlagPulse1 == false)
    {
        if(decayDividerCounterPulse1 > 0)
            decayDividerCounterPulse1--;
        else
        {
            decayDividerCounterPulse1 = constVolEnvDivPeriodPulse1;
            if(envelopeVolumePulse1 > 0)
                envelopeVolumePulse1--;
            else if(haltPulse1) //loop flag
            {
                envelopeVolumePulse1 = 15;
            }
        }
    }
    else
    {
        startFlagPulse1 = false;
        envelopeVolumePulse1 = 15;
        decayDividerCounterPulse1 = constVolEnvDivPeriodPulse1;
    }

    //Pulse2
    if(startFlagPulse2 == false)
    {
        if(decayDividerCounterPulse2 > 0)
            decayDividerCounterPulse2--;
        else
        {
            decayDividerCounterPulse2 = constVolEnvDivPeriodPulse2;
            if(envelopeVolumePulse2 > 0)
                envelopeVolumePulse2--;
            else if(haltPulse2) //loop flag
            {
                envelopeVolumePulse2 = 15;
            }
        }
    }
    else
    {
        startFlagPulse2 = false;
        envelopeVolumePulse2 = 15;
        decayDividerCounterPulse2 = constVolEnvDivPeriodPulse2;
    }

    //Noise
    if(startFlagNoise == false)
    {
        if(decayDividerCounterNoise > 0)
            decayDividerCounterNoise--;
        else
        {
            decayDividerCounterNoise = constVolEnvDivPeriodNoise;
            if(envelopeVolumeNoise > 0)
                envelopeVolumeNoise--;
            else if(haltNoise) //loop flag
            {
                envelopeVolumeNoise = 15;
            }
        }
    }
    else
    {
        startFlagNoise = false;
        envelopeVolumeNoise = 15;
        decayDividerCounterNoise = constVolEnvDivPeriodNoise;
    }

}

unsigned char APU::loadLengthCounter(unsigned char Value)
{
    return lengthCounterTable[Value >> 3];
}

void APU::clockTriangle()
{
    halfCyclesUntilTriangle = timerTriangle;
    if(lengthCounterTriangle > 0 && linearCounterTriangle > 0)
    {
        outputTriangle = sequencerTriangle[sequencerStepTriangle++];
        sequencerStepTriangle &= 31;
        //printf("%d %d\n", halfCyclesUntilTriangle, sequencerStepTriangle);
    }
}

void APU::clockPulse1()
{
    halfCyclesUntilPulse1 = timerPulse1 * 2;
    sequencerStepPulse1++;
    sequencerStepPulse1 &= 7;
    if(lengthCounterPulse1 > 0
       && !isSweepSilenced(timerPulse1, negateFlagPulse1, shiftCountPulse1)
       && sequencerPulse[dutyPulse1][sequencerStepPulse1])
    {
        outputPulse1 = (constantVolumeFlagPulse1) ? constVolEnvDivPeriodPulse1 : envelopeVolumePulse1;
    }
    else
        outputPulse1 = 0;

    //printf("%d %d\n", halfCyclesUntilPulse1, sequencerStepPulse1);
}

void APU::clockPulse2()
{
    halfCyclesUntilPulse2 = timerPulse2 * 2;
    sequencerStepPulse2++;
    sequencerStepPulse2 &= 7;
    if(lengthCounterPulse2 > 0
       && !isSweepSilenced(timerPulse2, negateFlagPulse2, shiftCountPulse2)
       && sequencerPulse[dutyPulse2][sequencerStepPulse2])
    {
        outputPulse2 = (constantVolumeFlagPulse2) ? constVolEnvDivPeriodPulse2 : envelopeVolumePulse2;
    }
    else
        outputPulse2 = 0;

    //printf("%d %d\n", halfCyclesUntilPulse2, sequencerStepPulse2);
}

void APU::clockNoise()
{
    halfCyclesUntilNoise = timerNoise;
    //printf("SH: %X ", shiftRegisterNoise);
    //getchar();
    bool feedback = shiftRegisterNoise & 0x1;
    //XOR with bit 6 if modeFlag else bit 1
    feedback ^= (modeFlagNoise) ? (shiftRegisterNoise & 0x20) != 0 : (shiftRegisterNoise & 0x2) != 0;
    shiftRegisterNoise >>= 1;
    shiftRegisterNoise |= feedback << 14;
    //printf("SH: %X ", shiftRegisterNoise);
    if((!(shiftRegisterNoise & 0x1)) && lengthCounterNoise)
    {
        outputNoise = (constantVolumeFlagNoise) ? constVolEnvDivPeriodNoise : envelopeVolumeNoise;
    }
    else
        outputNoise = 0;

}

bool APU::isSweepSilenced(unsigned short Timer, bool Negate, unsigned char ShiftCount)
{
    return (Timer < 8 || (!Negate && Timer + (Timer >> ShiftCount) > 0x7FF) );
}

void APU::setMemoryMapper(MemoryMapper* Board)
{
    board = Board;
}
