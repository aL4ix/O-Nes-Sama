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
int nmiDelayCount = 2;

unsigned char reverse(unsigned char b) {
    //From http://stackoverflow.com/a/2602885
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

PPU::PPU(InterruptLines &ints, Board &m) : gfx(256, 240), ints(ints), mapper(m)
{
    mapper.ppuStatus.sline = &scanlineNum;
    mapper.ppuStatus.tick = &ticks;
    mapper.ppuStatus.isRendering = &isRendering;
    mapper.setPPUChrMemPtr(chr);
    mapper.setPPUNTMemPtr(nametable);
    loadColorPaletteFromFile("nespalette.pal");
    powerOn();
    reset();

    //GFX
    back.MoveTo(0, 0);
    tex.Create(256, 240);
    back.SetTexture(tex);

    //DEBUGGER
    debugProcess = nullptr;
    debugNextTick = false;
    breakpointByAddressNum = 0;
    breakpointByValueNum = 0;
    breakpointByTime.reserve(32);
    breakpointByAddress.reserve(32);
    breakpointByValue.reserve(32);
}

PPU::~PPU()
{
}

void __fastcall PPU::writeMem(unsigned short Address, unsigned char Value)
{
    return (this->*writeFuncs[Address & 0x07])(Value);
}

unsigned char __fastcall PPU::readMem(unsigned short Address)
{
    return (this->*readFuncs[Address & 0x07])();
}

void PPU::process(int cpuLastInstCycles)
{
    int cyclesLeft = cpuLastInstCycles*3;
    for (int i=0; i<cyclesLeft; i++)
    {
        mapper.clockPPU();
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
                //zeroHit = calculateZeroHit()+3 +(ticksInThisScanline-304);
                //printf("ZH: %d\n", zeroHit);
            }

		}

        if(ticks == ticksInThisScanline)
        {
            ticks = 0;
            scanlineNum++;
            if(scanlineNum == 240)
            {
                render();
                isRendering = false;
            }
            else if(scanlineNum == 241)
            {
                reg2002 |= 0x80;
                if(reg2000 & 0x80){
                    triggerNMI();
                    //printf ("\nTrigger: %d, %d", ticks, scanlineNum);
                }
                oamAddr = 0;
            }
            else if(scanlineNum == 261) //OJO cambiar para PAL
            {
                scanlineNum = -1;
                if(reg2001 & 0x18)
                    isRendering = true;
                isOdd = !isOdd;

                //INITs

            }
        }
        else if(scanlineNum == -1 && ticks == 1)
        {
            reg2002 = 0;
            //printf("CZH: %x\n",reg2001);
        }
        if(isRendering)
        {

            /*if (ticks < 326){ //UGLY HACK for faking the MMC3 IRQ counter...
               mapper.setPPUAddress(0x1FFF);
            } else if (ticks >= 324) {
               mapper.setPPUAddress(0x2FFF);
            }*/
            switch(ticks)
            {
            case 255:
                //coarseFineY();

                //Sprite Evaluation

                {
                    unsigned char secNum = 0;
                    for(unsigned i=0; i<8*4; i++)
                        secondary[i] = 0xFF;
                    for(int n=0; n<64;)
                    {
                        unsigned oamFineY = unsigned(scanlineNum)-oam[n << 2];
                        unsigned char height = 8;
                        if(reg2000 & 0x20) //8x15
                            height = 16;
                        if(oamFineY < height)
                        {
                            for(int m=0; m<4; m++)
                                secondary[secNum << 2 | m] = oam[n << 2 | m];
                            if(n == 0)
                                secondary[secNum << 2 | 2] |= 0x4;
                            else
                                secondary[secNum << 2 | 2] &= ~0x4;
                            secNum++;
                        }
                        if(++n == 64)
                            break;
                        if(secNum >= 8)
                            break;
                    }
                    //std::sort(std::begin(secondary), std::begin(secondary)+secNum);
                }
                break;
            case 257:
                {
                    //Take one bit from NT and coarseX from loopy_t
                    loopy_v &= ~0x41F;
                    loopy_v |= loopy_t & 0x41F;

                    for(int n=0; n<8; n++)
                    {
                        counterOamX[n] = secondary[(n << 2) | 3];
                        latchOamAt[n] = secondary[(n << 2) | 2];
                    }
                    break;
                }
            /*case 258:  case 266:  case 274:  case 282:  case 290:  case 298:  case 306:  case 314:
                if(oamNum < secNum)
                    generateOam(secondary[oamNum]);
            break;*/
            case 260:  case 268:  case 276:  case 284:  case 292:  case 300:  case 308:  case 316:
                //OAM FETCHES
                //if(reg2001 & 0x10) //Sprites Enabled
                {
                    const unsigned char n = (ticks >> 3) & 0x7;

                    unsigned char totalHeight;
                    unsigned char oamTile;

                    if(!(reg2000 & 0x20)) //8x8
                    {
                        setCurSubBank(reg2000 >> 3 & 0x1);
                        totalHeight = 7;
                        oamTile = secondary[(n << 2) | 1];
                    }
                    else //8x16
                    {
                        setCurSubBank(secondary[n << 2 | 1] & 0x1);
                        addressBus = ((secondary[n << 2 | 1] & 0x1 * 0x1000));
                        totalHeight = 15;
                        oamTile = secondary[(n << 2) | 1] & ~0x1;
                    }

                    if(secondary[n << 2] == 0xFF)
                    {
                        intReadMem((curSubBank << 12) | (0xFF << 4));
                        break;
                    }

                    auto fineY = scanlineNum - secondary[(n << 2)];
                    if(latchOamAt[n] & 0x80) //FlipY
                        fineY = totalHeight - fineY;
                    if(fineY >= 8)
                    {
                        oamTile++;
                        fineY -= 8;
                    }
                    curSprito = (curSubBank << 12) | (oamTile << 4) | fineY;
                    shiftRegisterOamLow[n] = intReadMem(curSprito);
                }
                break;
            case 262:  case 270:  case 278:  case 286:  case 294:  case 302:  case 310:  case 318:
                //
                //if(reg2001 & 0x10) //Sprites Enabled
                {
                    const unsigned char n = (ticks >> 3) & 0x7;
                    shiftRegisterOamHigh[n] = intReadMem(curSprito | 0b1000);
                    if(latchOamAt[n] & 0x40) //FlipX
                    {
                        shiftRegisterOamLow[n] = reverse(shiftRegisterOamLow[n]);
                        shiftRegisterOamHigh[n] = reverse(shiftRegisterOamHigh[n]);
                    }
                }
            break;
            case 266:  case 274:  case 282:  case 290:  case 298:  case 306:  case 314:  case 320:

            break;
                //
            case 327: case 335: case 7:   case 15:  case 23:  case 31:  case 39:  case 47:  case 55:  case 63:
            case 71:  case 79:  case 87:  case 95:  case 103: case 111: case 119: case 127: case 135: case 143:
            case 151: case 159: case 167: case 175: case 183: case 191: case 199: case 207: case 215: case 223:
            case 231: case 239: case 247:
                coarseX();
                break;
            case 321: case 329: case 1:   case 9:   case 17:  case 25:  case 33:  case 41:  case 49:  case 57:
            case 65:  case 73:  case 81:  case 89:  case 97:  case 105: case 113: case 121: case 129: case 137:
            case 145: case 153: case 161: case 169: case 177: case 185: case 193: case 201: case 209: case 217:
            case 225: case 233: case 241: case 249:
                //Fetch NT
                curTile = intReadMem(0x2000 | (loopy_v & 0x0FFF));
                break;
            case 322: case 330: case 2:   case 10:  case 18:  case 26:  case 34:  case 42:  case 50:  case 58:
            case 66:  case 74:  case 82:  case 90:  case 98:  case 106: case 114: case 122: case 130: case 138:
            case 146: case 154: case 162: case 170: case 178: case 186: case 194: case 202: case 210: case 218:
            case 226: case 234: case 242: case 250:
                loadPaletteFromAttributeTable(loopy_v);
                break;
            case 324: case 332: case 4:   case 12:  case 20:  case 28:  case 36:  case 44:  case 52:  case 60:
            case 68:  case 76:  case 84:  case 92:  case 100: case 108: case 116: case 124: case 132: case 140:
            case 148: case 156: case 164: case 172: case 180: case 188: case 196: case 204: case 212: case 220:
            case 228: case 236: case 244: case 252:
            {
                //if(reg2001 & 0x8) //Enable Background
                {
                    {
                        const auto fineY = (loopy_v >> 12) & 0x7;
                        setCurSubBank(reg2000 >> 4 & 0x1);
                        const auto pos = (curSubBank << 12) | (curTile << 4) | fineY;
                        curChrLow = intReadMem(pos);
                    }
                }
            }
            break;
            case 326: case 334: case 6:   case 14:  case 22:  case 30:  case 38:  case 46:  case 54:  case 62:
            case 70:  case 78:  case 86:  case 94:  case 102: case 110: case 118: case 126: case 134: case 142:
            case 150: case 158: case 166: case 174: case 182: case 190: case 198: case 206: case 214: case 222:
            case 230: case 238: case 246: case 254:
                //if(reg2001 & 0x8) //Enable Background
                {
                    {
                        const auto fineY = (loopy_v >> 12) & 0x7;
                        setCurSubBank(reg2000 >> 4 & 0x1);
                        const auto pos = (curSubBank << 12) | (curTile << 4) | 0b1000 | fineY;
                        curChrHigh = intReadMem(pos);
                        if(ticks == 254)
                            coarseFineY();
                    }
                }
            break;
            case 8:   case 16:  case 24:  case 32:  case 40:  case 48:  case 56:  case 64:  case 72:  case 80:
            case 88:  case 96:  case 104: case 112: case 120: case 128: case 136: case 144: case 152: case 160:
            case 168: case 176: case 184: case 192: case 200: case 208: case 216: case 224: case 232: case 240:
            case 248: case 336:

                //SHIFT REGISTERS
                {
                    shiftRegisterChrLow |= curChrLow;
                    shiftRegisterChrHigh |= curChrHigh;
                    latchPalette = curPalette;
                }
            break;
            case 328:
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
            break;

            }
        } //END if(isRendering)

        if(!isRendering)
        {
            addressBus = loopy_v;
        }
        mapper.setPPUAddress(addressBus);
        emulateRenderingBG();
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

void PPU::emulateRenderingBG()
{
    if(ticks < 256)
    {
        if(0 <= scanlineNum && scanlineNum <= 239)
        {
            const auto pos = (scanlineNum << 8) | ticks;

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
            if(!(reg2001 & 0x8) || ((reg2001&0x6) != 0x6 && ticks<8))
                colorBG = 0;
            const unsigned char pal = (palHigh << 1) | palLow;
            loadSetOf4Colors(pal);
            const Color32 bgPixel(setOf4ColorsPalette[colorBG].GetColor());

            //OAMS
            Color32 oamPixel;
            oamPixel.SetColor(0);
            unsigned char colorOam = 0;
            bool frontBack = true;

            for(int n=0; n<8; n++)
            {
                if(counterOamX[n] == 0)
                {
                    const auto oamFineX = ticks - secondary[(n << 2) | 3];
                    if(oamFineX >= 8)
                    {
                        counterOamX[n] = 255;
                        continue;
                    }
                    const unsigned char pal = (latchOamAt[n] & 0x3) + 4;
                    colorOam = ((shiftRegisterOamHigh[n] >> 7 & 0x1) << 1) | (shiftRegisterOamLow[n] >> 7 & 0x1);
                    loadSetOf4Colors(pal);
                    oamPixel.SetColor(setOf4ColorsPalette[colorOam].GetColor());
                    frontBack = (latchOamAt[n] >> 5) & 0x1;
                    if(!(reg2001 & 0x10) || ((reg2001 & 0x6) != 0x6 && ticks<8))
                        colorOam = 0;
                    if(latchOamAt[n] & 0x4) //Zero
                    {
                        if(ticks == 255)
                            colorOam = 0;
                        /*if(colorOam)
                            printf("ZERO: %d.%d\n", scanlineNum, ticks);*/
                        if(zeroHit == 0 && !(reg2002 & 0x40))
                            if(colorBG != 0 && colorOam != 0 && ticks < 255 )
                            {
                                zeroHit = 4;
                                //printf("AQUI: %d,%d,\n", scanlineNum, ticks);
                            }

                    }
                    if(colorOam != 0)
                        break;
                }
            }
            for(int n=0; n<8; n++)
            {
                if(counterOamX[n] == 0)
                {
                    shiftRegisterOamLow[n] <<= 1;
                    shiftRegisterOamHigh[n] <<= 1;
                }
                else if(counterOamX[n] < 0xFF)
                    counterOamX[n]--;
            }

            if(colorBG == 0 && colorOam == 0)
                framebuffer[pos].SetColor(colorPalette[palette[0]].GetColor());
            else if(colorBG == 0 && colorOam != 0)
                framebuffer[pos].SetColor(oamPixel.GetColor());
            else if(colorBG != 0 && colorOam == 0)
                framebuffer[pos].SetColor(bgPixel.GetColor());
            else
            {
                if(frontBack)
                    framebuffer[pos].SetColor(bgPixel.GetColor());
                else
                    framebuffer[pos].SetColor(oamPixel.GetColor());
            }
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
    ints.nmi = 1;
    //printf("NMI: %d,%d\n", scanlineNum, ticks);
}

void PPU::clearNMI()
{
    ints.nmi = 0;
}

unsigned char __fastcall PPU::readLatch()
{
    return generalLatch;
}

unsigned char __fastcall PPU::read2002()
{
    writeToggle = false;
    unsigned char t = reg2002 | (generalLatch & 0x1f);
    //if(t & 0x80)
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

unsigned char __fastcall PPU::read2004()
{
    return generalLatch = oam[oamAddr];
}

unsigned char __fastcall PPU::read2007()
{
    unsigned char retval = 0;
    postFetchAddr = loopy_v & 0x3FFF;

    if(!isRendering)
    {
        if(reg2000 & 0x04)
            loopy_v += 32;
        else
            loopy_v++;
    }
    else
    {
        coarseX();
        coarseFineY();
    }

        //loopy_v &= 0x7FFF;

    if((postFetchAddr & 0x3F00) == 0x3F00){
        retval = palette[postFetchAddr & 0x1F];
    }
    else{
        retval = buf2007;
    }
    buf2007 = intReadMemLean(postFetchAddr);
    return generalLatch = retval; //Falta grayscale
}

void __fastcall PPU::write2000(unsigned char Value)
{
    //if(isRendering)
        //printf("2002: %x\n", Value);
    if((Value & 0x80) && (reg2002 & 0x80) && !(reg2000 & 0x80)){
        triggerNMI();
        printf ("\nTrigger: %d, %d", ticks, scanlineNum);
    }
    //RC
    if(!(Value & 0x80) && scanlineNum == 241 && ticks < 3)
        clearNMI();

    if (scanlineNum == -1 && ticks <= 1)
        clearNMI();

    loopy_t = (loopy_t & 0x73FF) | (Value & 3) << 10;
    reg2000 = Value;
}

void __fastcall PPU::write2001(unsigned char Value)
{
    //printf("2001: %x, S: %d, T: %d\n", Value, scanlineNum, ticks);
    reg2001 = Value;
    if((Value & 0x18) && (scanlineNum < 240)){
        //printf("\nTURNED ON: %x %d,%d\n", Value&0x18, scanlineNum, ticks);
        //getchar();
        isRendering = true;

    }
    else
    {
        //printf("\nTURNED OFF: %x %d,%d\n", Value&0x18, scanlineNum, ticks);
        //getchar();
        isRendering = false;
    }

    //Intensify colors
    //Grayscale

}

void __fastcall PPU::write2002(unsigned char Value)
{
    //Nothing to do here
}

void __fastcall PPU::write2003(unsigned char Value)
{
    //printf("2003 %x\n", cpuState.pc);
    generalLatch = oamAddr = Value;
}

void __fastcall PPU::write2004(unsigned char Value)
{
    //printf("W2004: %X:%X\n", oamAddr, Value);
    if(isRendering)
        Value = 0xFF;
    oam[oamAddr++] = Value;
}

void __fastcall PPU::write2005(unsigned char Value)
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
            //printf("MFX: %x %d,%d\n", loopy_x, scanlineNum, ticks);
    }
    writeToggle = !writeToggle;
}

void __fastcall PPU::write2006(unsigned char Value)
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
		}
    }
    else // first
    {
        loopy_t &= 0x00FF;
		loopy_t |= (Value & 0x3F) << 8;
    }
    writeToggle = !writeToggle;
}

void __fastcall PPU::write2007(unsigned char Value)
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
    //mapper.setPPUAddress(Address);

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
    intWriteMemLeanAndDebug(Address, Value);
    //mapper.setPPUAddress(Address);

    return;
}

void PPU::intWriteMemLeanAndDebug(unsigned short Address, unsigned char Value)
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

unsigned char PPU::intReadMemLean(unsigned short Address)
{
    unsigned char ret = 0;
    addressBus = Address & 0x3FFF;
    unsigned char DivisionOf400 = Address >> 10;

    switch(DivisionOf400)
    {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
        ret = chr[DivisionOf400][Address & 0x3FF];
        break;
    case 8: case 12:
        ret = nametable[0][Address & 0x3FF];
        break;
    case 9: case 13:
        ret = nametable[1][Address & 0x3FF];
        break;
    case 10: case 14:
        ret = nametable[2][Address & 0x3FF];
        break;
    case 11: case 15:
        ret = nametable[3][Address & 0x3FF];
        break;
    }
    return ret;
}

void PPU::intWriteMemLean(unsigned short Address, unsigned char Value)
{
    //Address &= 0x3FFF;
    addressBus = Address & 0x3FFF;
    unsigned char DivisionOf400 = Address >> 10;

    if(mapper.ppuStatus.chrReadOnly && DivisionOf400 <= 7)
        return;

    switch(DivisionOf400)
    {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
        chr[DivisionOf400][Address & 0x3FF] = Value;
        break;
    case 8: case 12:
        nametable[0][Address & 0x3FF] = Value;
        break;
    case 9: case 13:
        nametable[1][Address & 0x3FF] = Value;
        break;
    case 10: case 14:
        nametable[2][Address & 0x3FF] = Value;
        break;
    case 11: case 15:
        nametable[3][Address & 0x3FF] = Value;
        break;
    }
    return;
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

























void PPU::render()
{
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
    //return;

    //printf("%015x %x\n", loopy_v, loopy_x);
    //printf("%x\n", reg2000 & 0x3);
    if(reg2001 & 0x10) //Sprites
    {

        //printf("2000 %x\n", reg2000);
        //for(int i=0; i<64; i++) //It has 64 sprites
        //{
        //    generateOam(i);
        //}

    }
    if(reg2001 & 0x8) //Background
    {
        /*for(int i=0; i<1023; ) //NT has 32x30 tiles, plus one column at the right and one line at bottom for scrolling
        {

        }*/
    }
    //printf("%x\n", palette[0]);
    const Color32 &backgroundColor = colorPalette[palette[0]];
    gfx.SetBackgroundColor(backgroundColor);
    gfx.DrawBegin();
    if(reg2001 & 0x8) //Background
    {
        //glPushMatrix();
        //glTranslatef(-loopy_x, -((loopy_v & fineYMask) >> 12), 0); //Moves the scene so the fineX and fineY are effective

        //glPopMatrix();
    }

    tex.LoadFromMemory((void*)framebuffer, 256*240*4);
    back.Draw();

    gfx.DrawEnd();

    //printf("\n\n");
    //system("pause");
}

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

unsigned char** PPU::getChr()
{
    return chr;
}

unsigned char** PPU::getNametables()
{
    return nametable;
}

void PPU::setCurSubBank(bool ChrSubBank)
{
    curSubBank = ChrSubBank;
}

/*void PPU::setMemoryMapper(Board* b)
{
    mapper = b;
}*/


void PPU::generateTileWithoutCache(int VAddress, Color32 chrImage[64])
{
    const unsigned char curTile = intReadMemLean(0x2000 | VAddress);
    loadPaletteFromAttributeTable(VAddress);
    setCurSubBank(reg2000 >> 4 & 0x1);

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
    //setCurSubBank(SubBank);

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
