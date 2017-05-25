#include "PPU.h"

/*
Every cc is a ppu cc, a cpu cc is noted as cpucc
for NTSC 3 cc's are 1 cpucc
Scanlines per frame 262
A typical scanline lasts 341 cc
if rendering is enabled, every odd frame lasts -1 cc, it will finish one cc earlier
it starts in a even frame

sprite 0 hit, start at earliest at 3rd tick

*/

/*
loopy_v =

yyy NN YYYYY XXXXX
||| || ||||| +++++-- coarse X scroll
||| || +++++-------- coarse Y scroll
||| ++-------------- nametable select
+++----------------- fine Y scroll

unsigned short
fineYMask = 0b111000000000000;
ntMask = 0b110000000000;
coarseYMask = 0b1111100000;
coarseXMask = 0b11111;
*/

unsigned char reverse(unsigned char b) {
    //From http://stackoverflow.com/a/2602885
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

const double ZOOM = 1.0;

PPU::PPU(CPUIO &cio, MemoryMapper &m) : gfx(256*ZOOM, 240*ZOOM), cpuIO(cio), mapper(m)
{
    mapper.io.dbg.sl = &scanlineNum;
    mapper.io.dbg.tick = &ticks;
    //mapper.ppuStatus.tick = &ticks;
    //mapper.ppuStatus.isRendering = &isRendering;
    //mapper.setPPUChrMemPtr(chr);
    //mapper.setPPUNTMemPtr(nametable);
    const unsigned char defaultPalette[] = {70, 70, 70, 0, 6, 90, 0, 6, 120, 2, 6, 115, 53, 3, 76, 87, 0, 14, 90, 0, 0, 65, 0, 0, 18, 2, 0, 0, 20, 0, 0, 30, 0, 0, 30, 0, 0, 21, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 157, 157, 157, 0, 74, 185, 5, 48, 225, 87, 24, 218, 159, 7, 167, 204, 2, 85, 207, 11, 0, 164, 35, 0, 92, 63, 0, 11, 88, 0, 0, 102, 0, 0, 103, 19, 0, 94, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 255, 255, 31, 158, 255, 83, 118, 255, 152, 101, 255, 252, 103, 255, 255, 108, 179, 255, 116, 102, 255, 128, 20, 196, 154, 0, 113, 179, 0, 40, 196, 33, 0, 200, 116, 0, 191, 208, 43, 43, 43, 0, 0, 0, 0, 0, 0, 254, 255, 255, 158, 213, 255, 175, 192, 255, 208, 184, 255, 254, 191, 255, 255, 192, 224, 255, 195, 189, 255, 202, 156, 231, 213, 139, 197, 223, 142, 166, 230, 163, 148, 232, 197, 146, 228, 235, 167, 167, 167, 0, 0, 0, 0, 0, 0};
    loadColorPaletteFromArray(defaultPalette);
    powerOn();
    reset();

    //GFX
    back.MoveTo(0, 0);
    tex.Create(256, 240);
    back.SetTexture(tex);
    back.SetWidth(256*ZOOM);
    back.SetHeight(240*ZOOM);

    //DEBUGGER
    debugProcess = nullptr;
    debugNextTick = false;
    breakpointByAddressNum = 0;
    breakpointByValueNum = 0;
    breakpointByTime.reserve(32);
    breakpointByAddress.reserve(32);
    breakpointByValue.reserve(32);
    for(int i=0; i<342; i++)
        tickFuncs[i] = NULL;

    tickFuncs[257] = &PPU::tick257;
    for(int i=0; i<8; i++)
        tickFuncs[i*8 + 260] = &PPU::tickOamFetchesLow;
    for(int i=0; i<8; i++)
        tickFuncs[i*8 + 262] = &PPU::tickOamFetchesHigh;
    for(int i=0; i<31; i++)
        tickFuncs[i*8 + 7] = &PPU::coarseX;
    for(int i=0; i<2; i++)
        tickFuncs[i*8 + 327] = &PPU::coarseX;
    for(int i=0; i<32; i++)
        tickFuncs[i*8 + 1] = &PPU::tickFetchNT;
    for(int i=0; i<2; i++)
        tickFuncs[i*8 + 321] = &PPU::tickFetchNT;
    for(int i=0; i<32; i++)
        tickFuncs[i*8 + 2] = &PPU::tickFetchAT;
    for(int i=0; i<2; i++)
        tickFuncs[i*8 + 322] = &PPU::tickFetchAT;
    for(int i=0; i<32; i++)
        tickFuncs[i*8 + 4] = &PPU::tickFetchTileLow;
    for(int i=0; i<2; i++)
        tickFuncs[i*8 + 324] = &PPU::tickFetchTileLow;
    for(int i=0; i<32; i++)
        tickFuncs[i*8 + 6] = &PPU::tickFetchTileHigh;
    for(int i=0; i<2; i++)
        tickFuncs[i*8 + 326] = &PPU::tickFetchTileHigh;
    for(int i=0; i<31; i++)
        tickFuncs[i*8 + 8] = &PPU::tickShiftRegisters;
    tickFuncs[336] = &PPU::tickShiftRegisters;
    tickFuncs[328] = &PPU::tick328;
    tickFuncs[337] = &PPU::tickFetchNT;
    tickFuncs[339] = &PPU::tickFetchNT;

    for(int i=0; i<342; i++)
        spriteFuncs[i] = NULL;
    for(int i=1; i<=63; i++)
        spriteFuncs[i] = &PPU::spriteSecondaryClear;

    spriteFuncs[64] = &PPU::spriteEvaluationStarts;
    for(int i=65; i<=256; i++)
    {
        if(i & 0x1)
            spriteFuncs[i] = &PPU::spriteEvaluationOdd;
        else
            spriteFuncs[i] = &PPU::spriteEvaluationEven;
    }
    for(int i=257; i<=320; i++)
        spriteFuncs[i] = &PPU::spriteEvaluationTileLoading;
    for(int i=321; i<=340; i++)
        spriteFuncs[i] = &PPU::spriteEvaluationBackRend;
    //tickFuncs[255] = &PPU::tick255;
}

PPU::~PPU()
{
}

void PPU::writeMem(unsigned short Address, unsigned char Value)
{
    //printf("PPUW: %d,%d %X = %X\n", scanlineNum, ticks, Address&0x3fff, Value);
    return (this->*writeFuncs[Address & 0x07])(Value);
}

unsigned char PPU::readMem(unsigned short Address)
{
    auto Value = (this->*readFuncs[Address & 0x07])();
    //printf("PPUR: %d,%d, %X = %X\n", scanlineNum, ticks, Address&0x3fff, Value);
    return Value;
}

void PPU::process(int cpuCycles)
{
    int cyclesLeft = cpuCycles*3;
    for (int i=0; i<cyclesLeft; i++)
    {
        ticks++;
        //printf("%d %d\n", scanlineNum, ticks);

        if(ticks == 338) // 256 o 338
        {
            if(scanlineNum == -1)
            {
                if(isOdd && isRendering)
                    ticksInThisScanline = 340;
                else
                    ticksInThisScanline = 341;
            }
            else
                ticksInThisScanline = 341;
        }
        else if (ticks == 304)
		{
			if ((isRendering) && (scanlineNum == -1))
            {
                loopy_v = loopy_t;
            }
		}

        if(ticks == ticksInThisScanline)
        {
            ticks = 0;
            scanlineNum++;
            if(scanlineNum == 240)
            {
                displayFrame();
                isRendering = false;

            }
            else if(scanlineNum == 241)
            {
                reg2002 |= 0x80;
                if(reg2000 & 0x80)
                {
                    triggerNMI();
                    //printf ("\nTrigger: %d, %d", ticks, scanlineNum);
                }
                oamAddress = 0;
            }
            else if(scanlineNum == 261) //OJO cambiar para PAL
            {
                scanlineNum = -1;
                if(reg2001 & 0x18)
                {
                    isRendering = true;
                }
                isOdd = !isOdd;
            }
        }
        else if(scanlineNum == -1 && ticks == 1)
        {
            reg2002 = 0;
            //printf("CZH: %x\n",reg2001);
        }

        if(isRendering)
        {
            if(spriteFuncs[ticks])
                (this->*spriteFuncs[ticks])();
            if(tickFuncs[ticks])
                (this->*tickFuncs[ticks])();
        } else {

        }

        renderTick();

        if(zeroHit>0)
        {
            zeroHit--;
            if(zeroHit == 0)
            {
                reg2002 |= 0x40;
                //printf("HIT: %d,%d\n", scanlineNum, ticks);
            }
        }




        #ifdef DEBUGGER
            for(unsigned i=0; i<breakpointByTime.size(); i++)
            {
                if(breakpointByTime[i].scanline == scanlineNum)
                {
                    if(breakpointByTime[i].tick == ticks)
                    {
                        debugProcess(new BreakpointPPUByTime(scanlineNum, ticks));
                        break;
                    }
                }
            }
            /*if(debugNextTick)
            {
                debugProcess(new BreakpointPPUByTime(scanlineNum, ticks));
                debugNextTick = false;
            }*/
        #endif // DEBUGGER

    }

}

void PPU::renderTick()
{
    if(ticks < 256)
    {
        if(0 <= scanlineNum && scanlineNum <= 239)
        {
            //BG
            const bool chrLow = (shiftRegisterChrLow >> (15-loopy_x)) & 0x1;
            const bool chrHigh = (shiftRegisterChrHigh >> (15-loopy_x)) & 0x1;
            const bool palLow = (shiftRegisterPalLow >> (7-loopy_x)) & 0x1;
            const bool palHigh = (shiftRegisterPalHigh >> (7-loopy_x)) & 0x1;
            shiftRegisterChrHigh <<= 1;
            shiftRegisterChrLow <<= 1;
            shiftRegisterPalLow <<= 1;
            shiftRegisterPalHigh <<= 1;
            const bool latchLow = latchPalette & 0x1;
            const bool latchHigh = latchPalette & 0x2;
            shiftRegisterPalLow |= latchLow;
            shiftRegisterPalHigh |= latchHigh;
            unsigned char colorBG = (chrHigh << 1) | chrLow;
            if(!(reg2001 & 0x8) || ((reg2001 & 0x2) != 0x2 && ticks<8))
                colorBG = 0;

            const unsigned char palBG = (palHigh << 1) | palLow;

            //SPRITES
            unsigned char colorSprite = 0;
            unsigned char palSprite = 0;
            bool frontBack = true;

            for(int n=0; n<8; n++)
            {

                if(counterSpriteX[n] == 0)
                {
                    colorSprite = ((shiftRegisterSpriteHigh[n] >> 7 & 0x1) << 1) | (shiftRegisterSpriteLow[n] >> 7 & 0x1);
                    #ifdef LOGGER_PPU
                        logger.LogVariableFeature("RendSprColor", scanlineNum, "%d n=%d %X\n", ticks, n, colorSprite);
                    #endif // LOGGER_PPU
                    if(!(reg2001 & 0x10) || ((reg2001 & 0x4) != 0x4 && ticks<8))
                        colorSprite = 0;
                    if(colorSprite == 0)
                    {
                        #ifdef LOGGER_PPU
                            logger.LogVariableFeature("RendSprTransp", scanlineNum, "%d n=%d\n", ticks, n);
                        #endif // LOGGER_PPU
                        continue;
                    }


                    palSprite = (latchSpriteAt[n] & 0x3) + 4;
                    frontBack = (latchSpriteAt[n] >> 5) & 0x1;
                    #ifdef LOGGER_PPU
                        logger.LogVariableFeature("RendSprPalFront", scanlineNum, "%d n=%d %X %X\n", ticks, n, palSprite, frontBack);
                    #endif // LOGGER_PPU
                    if(latchSpriteAt[n] & 0x4) //Zero
                    {
                        if(ticks == 255)
                        {
                            colorSprite = 0;
                            continue;
                        }
                        /*if(colorSprite)
                            printf("ZERO: %d.%d\n", scanlineNum, ticks);*/
                        if(zeroHit == 0 && !(reg2002 & 0x40))
                            if(colorBG != 0 && colorSprite != 0 && ticks < 255 )
                            {
                                zeroHit = 4;
                                //printf("AQUI: %d,%d,\n", scanlineNum, ticks);
                            }
                    }
                    break;
                }
            }
            for(int n=0; n<8; n++)
            {
                if(counterSpriteX[n] == 0)
                {
                    shiftRegisterSpriteLow[n] <<= 1;
                    shiftRegisterSpriteHigh[n] <<= 1;
                }
                else if(counterSpriteX[n] < 0xFF)
                    counterSpriteX[n]--;
            }

            unsigned char colorToRender;
            if(colorBG == 0 && colorSprite == 0)
            {
                if(!(reg2001 & 0x18) && 0x3F00 <= loopy_v && loopy_v <= 0x3FFF) // Palette hack
                    colorToRender = loopy_v & 0x1F;
                else
                    colorToRender = 0;
            }
            else if(colorBG == 0 && colorSprite != 0)
                colorToRender = (palSprite<<2) | colorSprite;
            else if(colorBG != 0 && colorSprite == 0)
                colorToRender = (palBG<<2) | colorBG;
            else
            {
                if(frontBack)
                    colorToRender = (palBG<<2) | colorBG;
                else
                    colorToRender = (palSprite<<2) | colorSprite;
            }
            const auto pos = (scanlineNum << 8) | ticks;
            framebuffer[pos].SetColor(colorPalette[palette[colorToRender]].GetColor());
        }
    }
}

void PPU::reset()
{
    scanlineNum = 241;
    ticks = 0;
    isRendering = false;
    ticksInThisScanline = 341;
    isOdd = false;
    reg2000 = 0;
    reg2001 = 0;
    generalLatch = 0;
    writeToggle = false;
    postFetchAddr = 0;
}

void PPU::powerOn()
{
    loopy_t = 0;
}

void PPU::triggerNMI()
{
    cpuIO.nmi = 1;
    //printf("NMI: %d,%d\n", scanlineNum, ticks);
}

void PPU::clearNMI()
{
    cpuIO.nmi = 0;
}

unsigned char PPU::readLatch()
{
    return generalLatch;
}

unsigned char PPU::read2002()
{
    writeToggle = false;
    unsigned char t = reg2002 | (generalLatch & 0x1f);
    reg2002 &= ~0x80;
    //RC

    if(scanlineNum == 241)
    {
        if(ticks < 3)
            clearNMI();
        if(ticks == 0)
            t &= ~0x80;
    }
    //printf("2002: %d\n", t);
    return generalLatch = t;
}

unsigned char PPU::read2004()
{
    if(isRendering)
    {
        return generalLatch = getOam(oamPointer);
    }
    return generalLatch = getOam(oamAddress);
}

unsigned char PPU::read2007()
{
    unsigned char retval = 0;
    postFetchAddr = loopy_v & 0x3FFF;

    if(!isRendering)
    {
        if(reg2000 & 0x04)
            loopy_v += 32;
        else
            loopy_v++;
        mapper.readPPU(loopy_v);
    }
    else
    {
        coarseX();
        coarseFineY();
    }

    if((postFetchAddr & 0x3F00) == 0x3F00){
        retval = palette[postFetchAddr & 0x1F];
    }
    else{
        retval = buf2007;
    }

    //if(postFetchAddr >= 0x3000)
        //postFetchAddr &= ~0x1000;
    buf2007 = intReadMemLean(postFetchAddr, !isRendering);
    addressBus = postFetchAddr;
    mapper.readPPU(postFetchAddr);
    return generalLatch = retval; //Falta grayscale
}

void PPU::write2000(unsigned char Value)
{
    if(isRendering)
        //printf("2002: %x\n", Value);
    if((Value & 0x80) && (reg2002 & 0x80) && !(reg2000 & 0x80)){
        triggerNMI();
        //printf ("\nTrigger: %d, %d", ticks, scanlineNum);
    }
    //RC
    if(!(Value & 0x80) && scanlineNum == 241 && ticks < 3)
        clearNMI();

    if (scanlineNum == -1 && ticks <= 1)
        clearNMI();

    loopy_t = (loopy_t & 0x73FF) | (Value & 3) << 10;
    reg2000 = Value;
    //printf("2000: %d\n", reg2000 & 0x18);
}

void PPU::write2001(unsigned char Value)
{
    //printf("2001: %x, S: %d, T: %d\n", scanlineNum, ticks);
    reg2001 = Value;
    if((Value & 0x18) && (scanlineNum < 240)){
        //printf("TURNED ON: %x %d,%d\n", Value&0x18, scanlineNum, ticks);
        isRendering = true;
    }
    else
    {
        //printf("TURNED OFF: %x %d,%d\n", Value&0x18, scanlineNum, ticks);
        isRendering = false;
    }

    //Intensify colors
    //Grayscale
}

void PPU::write2002(unsigned char Value)
{
    //Nothing to do here
}

void PPU::write2003(unsigned char Value)
{
    //printf("2003 %x\n", cpuState.pc);
    generalLatch = oamAddress = Value;
}

void PPU::write2004(unsigned char Value)
{
    //printf("W2004: %X:%X\n", oamAddr, Value);
    if(isRendering)
        Value = 0xFF;
    setOam(oamAddress++, Value);
}

void PPU::write2005(unsigned char Value)
{
    //if(isRendering)
    //    printf("2005 %x %x\n", cpuState.pc, Value);
    if(writeToggle) // second
    {
        loopy_t &= 0x0C1F;
		loopy_t |= (Value & 0x07) << 12;
		loopy_t |= (Value & 0xF8) << 2;
    }
    else // first
    {
        loopy_t &= 0x7FE0;
		loopy_t |= (Value & 0xF8) >> 3;
		loopy_x = Value & 7;
		//if(isRendering)
        //    printf("MFX: %x %d,%d\n", loopy_x, scanlineNum, ticks);
    }
    writeToggle = !writeToggle;
}

void PPU::write2006(unsigned char Value)
{
    //if(isRendering)
        //printf("2006 %x\n", Value);

    if(writeToggle) // second
    {
        loopy_t &= 0x7F00;
		loopy_t |= Value;
		loopy_v = loopy_t;

		if(isRendering)
		{
            //printf("\nMFV: %X %d,%d\n", loopy_v, scanlineNum, ticks);
            //getchar();
		} else {
            //addressBus = loopy_v;
            mapper.readPPU(loopy_v);
		}


    }
    else // first
    {
        loopy_t &= 0x00FF;
		loopy_t |= (Value & 0x3F) << 8;
    }
    writeToggle = !writeToggle;
}

void PPU::write2007(unsigned char Value)
{
    //printf("W2007: %d,%d\n", scanlineNum, ticks);
    if((loopy_v  & 0x3F00) == 0x3F00)
    {
        Value &= 0x3F;
        auto addr = loopy_v & 0x1F;
        palette[addr] = Value;
        if (!(addr & 0x3))
			palette[addr ^ 0x10] = Value; //Mirror palette

        //if(!isRendering)
            //printf("MFP: %d, %x %d,%d\n", Value, loopy_v & 0x3FFF, scanlineNum, ticks);
    }
    else{
        intWriteMemLean(loopy_v, Value);
    }

    if(!isRendering)
    {
        if(reg2000 & 0x04)
            loopy_v += 32;
        else
            loopy_v++;

        loopy_v &= 0x7FFF;
        //addressBus = loopy_v;
        mapper.readPPU(loopy_v);
    }
    else
    {
        coarseX();
        coarseFineY();
    }
}

unsigned char PPU::intReadMem(unsigned short Address)
{
    unsigned char ret = intReadMemLean(Address);

    #ifdef DEBUGGER
        for(unsigned i=0; i<breakpointByAddressNum; i++)
        {
            if(breakpointByAddress[i].address == Address)
            {
                debugProcess(new BreakpointPPUByAddress(Address));
                break;
            }
        }
        for(unsigned i=0; i<breakpointByValueNum; i++)
        {
            if(breakpointByValue[i].address == Address)
            {
                if(breakpointByValue[i].value == ret)
                {
                    debugProcess(new BreakpointPPUByValue(Address, ret));
                    break;
                }
            }
        }
    #endif // DEBUGGER

    return ret;
}

void PPU::intWriteMem(unsigned short Address, unsigned char Value)
{
    intWriteMemLean(Address, Value);
    #ifdef DEBUGGER
        for(unsigned i=0; i<breakpointByAddressNum; i++)
        {
            if(breakpointByAddress[i].address == Address)
            {
                debugProcess(new BreakpointPPUByAddress(Address));
                break;
            }
        }
        for(unsigned i=0; i<breakpointByValueNum; i++)
        {
            if(breakpointByValue[i].address == Address)
            {
                if(breakpointByValue[i].value == Value)
                {
                    debugProcess(new BreakpointPPUByValue(Address, Value));
                    break;
                }
            }
        }
    #endif // DEBUGGER

    return;
}

unsigned char PPU::intReadMemLean(unsigned short Address, bool updateBus)
{
    addressBus = Address;
    return mapper.readPPU(Address);
}

void PPU::intWriteMemLean(unsigned short Address, unsigned char Value, bool updateBus)
{
    Address &= 0x3FFF;
    addressBus = Address;
    mapper.writePPU(Address, Value);
}

void PPU::coarseX()
{
    if ((loopy_v & 0x001F) == 31) // if coarse X == 31
    {
        loopy_v &= ~0x001F;          // coarse X = 0
        loopy_v ^= 0x0400;           // switch horizontal nametable
    }
    else
        loopy_v += 1;                // increment coarse X
}
/*
void PPU::coarseY()
{
    int y = (loopy_v & 0x03E0) >> 5;        // let y = coarse Y
    if(y == 29)
    {
        y = 0;                          // coarse Y = 0
        loopy_v ^= 0x0800;                    // switch vertical nametable
    }
    else if(y == 31)
        y = 0;                          // coarse Y = 0, nametable not switched
    else
        y += 1;                         // increment coarse Y
    loopy_v = (loopy_v & ~0x03E0) | (y << 5);     // put coarse Y back into v
}
*/
void PPU::coarseFineY()
{
    if ((loopy_v & 0x7000) != 0x7000)        // if fine Y < 7
        loopy_v += 0x1000;                      // increment fine Y
    else
    {
        loopy_v &= ~0x7000;                     // fine Y = 0
        int y = (loopy_v & 0x03E0) >> 5;        // let y = coarse Y
        if (y == 29)
        {
            y = 0;                          // coarse Y = 0
            loopy_v ^= 0x0800;                    // switch vertical nametable
        }
        else if (y == 31)
            y = 0;                          // coarse Y = 0, nametable not switched
        else
            y += 1;                         // increment coarse Y
        loopy_v = (loopy_v & ~0x03E0) | (y << 5);     // put coarse Y back into v
    }
}
/*
void PPU::deCoarseX()
{
    if ((loopy_v & 0x001F) == 0) // if coarse X == 0
    {
        loopy_v |= 0x001F;          // coarse X = 31
        loopy_v ^= 0x0400;           // switch horizontal nametable
    }
    else
        loopy_v -= 1;                // decrement coarse X
}
*/
























void PPU::displayFrame()
{
    const Color32 &backgroundColor = colorPalette[palette[0]];
    gfx.SetBackgroundColor(backgroundColor);
    gfx.DrawBegin();

    tex.LoadFromMemory((void*)framebuffer, 256*240*4);
    back.Draw();

    gfx.DrawEnd();

    //printf("\n\n");
    //system("pause");
}
/*
bool PPU::loadColorPaletteFromFile(const char* FileName)
{
    FILE* file;
    file = fopen(FileName, "rb");
    if(!file)
        return false;
    for(int i=0; i<64; i++)
    {
        unsigned char r = fgetc(file);
        unsigned char g = fgetc(file);
        unsigned char b = fgetc(file);
        colorPalette[i].SetColor(r, g, b);
    }
    fclose(file);
    return true;
}
*/
bool PPU::loadColorPaletteFromArray(const unsigned char* Palette)
{
    for(int i=0; i<64; i++)
    {
        unsigned char r = Palette[i*3+0];
        unsigned char g = Palette[i*3+1];
        unsigned char b = Palette[i*3+2];
        colorPalette[i].SetColor(r, g, b);
    }
    return true;
}

void PPU::loadPaletteFromAttributeTable(const unsigned short VAddress)
{
    const unsigned short NtPos = VAddress & 0x3FF;
    const unsigned short x = NtPos % 32;
    const unsigned short y = NtPos >> 5;
    //                      x / 2             y / 2        * 2
    const int palSubNum = ((x >> 1) % 2) + (((y >> 1) % 2) << 1);
    //const unsigned char at = intReadMem(0x23C0 | (loopy_v & 0x0C00) | ((loopy_v >> 4) & 0x38) | ((loopy_v >> 2) & 0x07));
    const unsigned char at = intReadMem(0x23C0 | (VAddress & 0x0C00) | ((VAddress >> 4) & 0x38) | ((VAddress >> 2) & 0x07));
    curPalette = at >> (2 * palSubNum);
    curPalette &= 0x03;
}

void PPU::loadSetOf4Colors(const int Pal)
{
    const int pal = Pal << 2; // Pal * 4
    setOf4ColorsPalette[0] = Color32::Transparent;
    for(int i=1; i<4; i++)
    {
        auto c = palette[pal+i];
        setOf4ColorsPalette[i] = colorPalette[c];
    }
}

void PPU::saveState(FILE* File)
{
    PPU_State* state = this;
    fwrite(state, sizeof(PPU_State), 1, File);
}

bool PPU::loadState(FILE* File)
{
    PPU_State* state = this;
    auto size = fread(state, sizeof(PPU_State), 1, File);
    return (size == sizeof(PPU_State));
}


/*
void PPU::generateTileWithoutCache(int VAddress, Color32 chrImage[64])
{
    const unsigned char curTile = intReadMemLean(0x2000 | VAddress);
    loadPaletteFromAttributeTable(VAddress);
    const bool curSubBank = reg2000 >> 4 & 0x1;

    //setTextureToSprite(curSprite, curTile);
        //loadChrTile(CurTile);
            unsigned char chrWithoutPalettes[8][8];
            GenerateCHRBitmapWithoutPalette(curSubBank, curTile, chrWithoutPalettes);
        //loadChrTile(CurTile);
        loadSetOf4Colors(curPalette);
        //const Texture& tex = loadTextureTile(curTile, 8);
            GenerateCHRBitmap(chrImage, chrWithoutPalettes);
        //const Texture& tex = loadTextureTile(curTile, 8);
    //setTextureToSprite(curSprite, curTile);
}
*/
void PPU::GenerateCHRBitmapWithoutPalette(bool SubBank, unsigned Tile, unsigned char (&Chr)[8][8])
{
    for(int y = 0; y<8; y++)
    {//                                                      * 16
        unsigned char chr1 = intReadMemLean((SubBank << 12) | ((Tile << 4)+y));
        unsigned char chr2 = intReadMemLean((SubBank << 12) | ((Tile << 4)+8+y));

        for(unsigned char x = 7; x<8; x--)
        {
            Chr[y][x] = (chr1 & 1) | ((chr2 & 1) << 1);
            chr1 = chr1 >> 1;
            chr2 = chr2 >> 1;
        }
    }
}

void PPU::GenerateCHRBitmap(Color32 ChrBitmap[], unsigned char (&Chr)[8][8])
{
    for(int y = 0; y<8; y++)
    {
        for(unsigned char x = 0; x<8; x++)
        {
            const unsigned char color = Chr[y][x];
            ChrBitmap[y*8+x].SetColor(setOf4ColorsPalette[color].GetColor());
        }
    }
}

void PPU::generateCHR(Color32 chrImage[64], const unsigned char Tile, const unsigned char Palette, const bool SubBank)
{
    unsigned char chrWithoutPalettes[8][8];
    GenerateCHRBitmapWithoutPalette(SubBank, Tile, chrWithoutPalettes);
    loadSetOf4Colors(Palette);
    setOf4ColorsPalette[0].SetColor(colorPalette[palette[0]].GetColor());
    GenerateCHRBitmap(chrImage, chrWithoutPalettes);
}

unsigned char PPU::getPaletteFromAttributeTable(const unsigned short VAddress)
{
    const unsigned short NtPos = VAddress & 0x3FF;
    const unsigned short x = NtPos % 32;
    const unsigned short y = NtPos >> 5;
    //                      x / 2             y / 2        * 2
    const int palSubNum = ((x >> 1) % 2) + (((y >> 1) % 2) << 1);
    //const unsigned char at = intReadMem(0x23C0 | (loopy_v & 0x0C00) | ((loopy_v >> 4) & 0x38) | ((loopy_v >> 2) & 0x07));
    const unsigned char at = intReadMemLean(0x23C0 | (VAddress & 0x0C00) | ((VAddress >> 4) & 0x38) | ((VAddress >> 2) & 0x07));
    return (at >> (2 * palSubNum)) & 0x03;
}

void PPU::generateNT(Color32 chrImage[64], unsigned short VAddress)
{
    const unsigned char tile = intReadMemLean(0x2000 | VAddress);
    const unsigned char palette = getPaletteFromAttributeTable(VAddress);
    const bool subBank = reg2000 >> 4 & 0x1;

    generateCHR(chrImage, tile, palette, subBank);
}

void PPU::generateOam(Color32 chrImage[64], unsigned char OamNum)
{
    const unsigned char tile = oam[(OamNum << 2) | 1];
    const unsigned char palette = (oam[(OamNum << 2) | 2] & 0x3) + 4;
    const bool subBank = reg2000 >> 3 & 0x1;

    generateCHR(chrImage, tile, palette, subBank);
}





void PPU::setOam(const unsigned Address, const unsigned char Value)
{
    oamPointer = Address;
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("SetOAM", scanlineNum, "%d %d=%d\n", ticks, oamPointer, Value);
    #endif // LOGGER_PPU
    oam[oamPointer] = Value;
}

unsigned char PPU::getOam(const unsigned Address)
{
    oamPointer = Address;
    const unsigned char Value = oam[oamPointer];
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("GetOAM", scanlineNum, "%d %d=%d\n", ticks, oamPointer, Value);
    #endif
    return Value;
}

inline void PPU::setSecondary(const unsigned Address, const unsigned char Value)
{
    setOam(Address + 64*4, Value);
}

inline unsigned char PPU::getSecondary(const unsigned Address)
{
    return getOam(Address + 64*4);
}

void PPU::spriteSecondaryClear()
{
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("SprEvalSecClear", scanlineNum, "%d\n", ticks);
    #endif // LOGGER_PPU
    setSecondary(ticks >> 1, 0xFF);
}

void PPU::spriteEvaluationStarts()
{
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("SprEvalStarts", scanlineNum, "%d\n", ticks);
    #endif // LOGGER_PPU
    spriteEvalN = spriteEvalM = secNum = 0;
    oamCompleted = oamSecondaryFull = false;
    if(oamAddress >= 8)
    {
        for(int i=0; i<8; i++)
        {
            auto ret = getOam((oamAddress & 0xF8) + i);
            setOam(i, ret);
        }

    }
}

void PPU::spriteEvaluationBackRend()
{
    getSecondary(0);
}

void PPU::spriteEvaluationTileLoading()
{
    oamAddress = 0;

    //Garbage fetches - Quick and dirty (probably wrong as hell)
    switch (ticks){
        case 257: case 265: case 273: case 281: case 289: case 297: case 305:
            tickFetchNT();
            break;
        case 259: case 267: case 275: case 283: case 291: case 299: case 307:
            tickFetchAT();
            break;
    }
}

void PPU::spriteEvaluationOdd()
{
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("SprEvalOdd", scanlineNum, "%d\n", ticks);
    #endif // LOGGER_PPU
    unsigned pos = (spriteEvalN << 2) + oamAddress;
    spriteY = getOam(0xFF & (pos + 0));
    spriteT = getOam(0xFF & (pos + 1));
    spriteA = getOam(0xFF & (pos + 2));
    spriteX = getOam(0xFF & (pos + 3));
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("SprEvalYTAX", scanlineNum, "%X %X %X %X\n", spriteY, spriteT, spriteA, spriteX);
    #endif // LOGGER_PPU
}

void PPU::spriteEvaluationEven()
{
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("SprEvalEven", scanlineNum, "%d\n", ticks);
    #endif // LOGGER_PPU
    if(!oamCompleted)
    {
        const unsigned oamFineY = unsigned(scanlineNum)-spriteY;
        unsigned char height = 8;
        if(reg2000 & 0x20) // Double height
            height = 16;
        if(!oamSecondaryFull)
        {
            if(oamFineY < height) // In range
            {
                #ifdef LOGGER_PPU
                    logger.LogVariableFeature("SprEvalInRange", scanlineNum, "%d\n", ticks);
                #endif // LOGGER_PPU

                const unsigned char address = secNum << 2;
                setSecondary(address | 0, spriteY);
                setSecondary(address | 1, spriteT);
                setSecondary(address | 2, spriteA);
                setSecondary(address | 3, spriteX);
                if(spriteEvalN == 0)
                    setSecondary(address | 2, getSecondary(address | 2) | 0x4);
                else
                    setSecondary(address | 2, getSecondary(address | 2) & ~0x4);
                if(++secNum == 8)
                    oamSecondaryFull = true;
            }
            spriteEvalN = (spriteEvalN + 1) & 63;
            if(spriteEvalN == 0)
                oamCompleted = true;
        }
        else // Secondary is full !!
        {
            if(oamFineY < height) // In range
            {
                reg2002 |= 0x20; // Sprite overflow
                oamCompleted = true; // Finish evaluation
            }
            else // Not in range
            {
                spriteEvalM = (spriteEvalM + 1) & 3;
                spriteEvalN = (spriteEvalN + 1) & 63;
                if(spriteEvalN == 0)
                    oamCompleted = true;
            }
        }
    }
}

void PPU::tick257()
{
    //Take one bit from NT and coarseX from loopy_t
    loopy_v &= ~0x41F;
    loopy_v |= loopy_t & 0x41F;

    for(int n=0; n<8; n++)
    {
        counterSpriteX[n] = getSecondary((n << 2) | 3);
        latchSpriteAt[n] = getSecondary((n << 2) | 2);
    }
}

void PPU::tickOamFetchesLow()
{
    const unsigned char n = (ticks >> 3) & 0x7;

    unsigned char totalHeight;
    unsigned char oamTile;
    bool curSubBank;

    if(!(reg2000 & 0x20)) //8x8
    {
        curSubBank = reg2000 >> 3 & 0x1;
        totalHeight = 7;
        oamTile = getSecondary((n << 2) | 1);
    }
    else //8x16
    {
        curSubBank = getSecondary(n << 2 | 1) & 0x1;
        totalHeight = 15;
        oamTile = getSecondary((n << 2) | 1) & ~0x1;
    }

    if(getSecondary(n << 2) == 0xFF)
    {
        addressBus = (curSubBank << 12) | (0xFF << 4);
        intReadMem(addressBus);
    }
    else
    {
        auto fineY = scanlineNum - getSecondary(n << 2);
        if(latchSpriteAt[n] & 0x80) //FlipY
            fineY = totalHeight - fineY;
        if(fineY >= 8)
        {
            oamTile++;
            fineY -= 8;
        }
        addressBus = (curSubBank << 12) | (oamTile << 4) | fineY;
        shiftRegisterSpriteLow[n] = intReadMem(addressBus);
    }
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("FetchOamLow", scanlineNum, "%d n=%d ab=%X s=%X\n", ticks, n, addressBus, shiftRegisterSpriteLow[n]);
    #endif // LOGGER_PPU
}

void PPU::tickOamFetchesHigh()
{
    const unsigned char n = (ticks >> 3) & 0x7;
    shiftRegisterSpriteHigh[n] = intReadMem(addressBus | 0b1000);
    if(latchSpriteAt[n] & 0x40) //FlipX
    {
        shiftRegisterSpriteLow[n] = reverse(shiftRegisterSpriteLow[n]);
        shiftRegisterSpriteHigh[n] = reverse(shiftRegisterSpriteHigh[n]);
    }
    #ifdef LOGGER_PPU
        logger.LogVariableFeature("FetchOamHigh", scanlineNum, "%d n=%d ab=%X s=%X\n", scanlineNum, ticks, n, addressBus, shiftRegisterSpriteHigh[n]);
    #endif // LOGGER_PPU
}

void PPU::tickFetchNT()
{
    curTile = intReadMem(0x2000 | (loopy_v & 0x0FFF));
}

void PPU::tickFetchAT()
{
    loadPaletteFromAttributeTable(loopy_v);
}

void PPU::tickFetchTileLow()
{
    const auto fineY = (loopy_v >> 12) & 0x7;
    const bool curSubBank = reg2000 >> 4 & 0x1;
    addressBus = (curSubBank << 12) | (curTile << 4) | fineY;
    curChrLow = intReadMem(addressBus);
}

void PPU::tickFetchTileHigh()
{
    const auto fineY = (loopy_v >> 12) & 0x7;
    const bool curSubBank = reg2000 >> 4 & 0x1;
    addressBus = (curSubBank << 12) | (curTile << 4) | 0b1000 | fineY;
    curChrHigh = intReadMem(addressBus);
    if(ticks == 254)
    {
        coarseFineY();
    }
}

void PPU::tickShiftRegisters()
{
    shiftRegisterChrLow |= curChrLow;
    shiftRegisterChrHigh |= curChrHigh;
    latchPalette = curPalette;
}

void PPU::tick328()
{
    shiftRegisterChrLow = curChrLow << 8;
    shiftRegisterChrHigh = curChrHigh << 8;
    for(int i=0; i<8; i++)
    {
        const bool latchLow = curPalette & 0x1;
        shiftRegisterPalLow <<= 1;
        shiftRegisterPalLow |= latchLow;

        const bool latchHigh = curPalette & 0x2;
        shiftRegisterPalHigh <<= 1;
        shiftRegisterPalHigh |= latchHigh;

    }
}
