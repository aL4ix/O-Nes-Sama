#ifndef PPU_H_INCLUDED
#define PPU_H_INCLUDED
#include <unordered_map>
#include <functional>
#include <vector>
#include <cstring>

//#include "Mappers/Boards/Board.h"
#include "x6502Interrupts.h"
#include "RetroGraphix/GraphixEngine.hpp"
#include "RetroGraphix/Texture.hpp"
#include "RetroGraphix/Color.hpp"
#include "RetroGraphix/Sprite.hpp"
#include "Debugger/Breakpoint.h"
#include "Cartridge/Cartridge.hpp"

#ifndef _WIN32
    #define __fastcall
#endif // _WIN32


class PPU_State
{
friend class Debugger;
friend class CPU;

protected:
    int scanlineNum;
    bool isRendering;
    int ticks;
    int ticksInThisScanline;
    bool isOdd;
    unsigned char palette[0x20];
    unsigned char oam[64*4];
    unsigned char generalLatch;
    unsigned char reg2000;
    unsigned char reg2001;
    unsigned char reg2002;
    unsigned short loopy_v;
    bool writeToggle;
    unsigned char buf2007;
    unsigned short postFetchAddr;
    unsigned char oamAddr;
    unsigned short loopy_t;
    unsigned char loopy_x;
    int zeroHit;
    int secondary[8*4];
    unsigned curSprito;
    unsigned char curTile;
    unsigned char curPalette;
    unsigned char curChrLow;
    unsigned char curChrHigh;
    unsigned char latchPalette;
    unsigned short shiftRegisterChrLow;
    unsigned short shiftRegisterChrHigh;
    unsigned char shiftRegisterPalLow;
    unsigned char shiftRegisterPalHigh;
    unsigned char shiftRegisterOamLow[8];
    unsigned char shiftRegisterOamHigh[8];
    unsigned char latchOamAt[8];
    unsigned char counterOamX[8];
    unsigned short addressBus;
};

class PPU : public PPU_State
{
friend class Debugger;

public:
    PPU(struct InterruptLines &ints, Board &m);
    ~PPU();
    void __fastcall writeMem(unsigned short Address, unsigned char Value);
    unsigned char __fastcall readMem(unsigned short Address);
    void process(int);
    void render();
    void reset();
    void powerOn();
    void saveState(FILE* File);
    bool loadState(FILE* File);
    bool loadColorPaletteFromFile(const char* FileName);
    unsigned char ** ppuCartSpace[16];
    unsigned char ** getChr();
    unsigned char ** getNametables();
//    void setMemoryMapper(Board * b);

private:
    //INTERNALS
    unsigned char * nametable[4];
    unsigned char * chr[8];

    bool curSubBank;

    void triggerNMI();
    void clearNMI();
    unsigned char __fastcall readLatch();
    unsigned char __fastcall read2002();
    unsigned char __fastcall read2004();
    unsigned char __fastcall read2007();
    void __fastcall write2000(unsigned char Value);
    void __fastcall write2001(unsigned char Value);
    void __fastcall write2002(unsigned char Value);
    void __fastcall write2003(unsigned char Value);
    void __fastcall write2004(unsigned char Value);
    void __fastcall write2005(unsigned char Value);
    void __fastcall write2006(unsigned char Value);
    void __fastcall write2007(unsigned char Value);
    void (__fastcall PPU::*writeFuncs[8])(unsigned char) = {&PPU::write2000, &PPU::write2001, &PPU::write2002, &PPU::write2003, &PPU::write2004, &PPU::write2005, &PPU::write2006, &PPU::write2007};
    unsigned char (__fastcall PPU::*readFuncs[8])(void) = {&PPU::readLatch, &PPU::readLatch, &PPU::read2002, &PPU::readLatch, &PPU::read2004, &PPU::readLatch, &PPU::readLatch, &PPU::read2007};
    unsigned char intReadMem(unsigned short Address);
    void intWriteMem(unsigned short Address, unsigned char Value);
    unsigned char intReadMemLean(unsigned short Address);
    void intWriteMemLean(unsigned short Address, unsigned char Value);
    void intWriteMemLeanAndDebug(unsigned short Address, unsigned char Value);
    void coarseX();
    void coarseY();
    void deCoarseX();
    void coarseFineY();
    void emulateRenderingBG();

    //RENDERING
    Color32 framebuffer[240*256];
    Sprite back;
    Texture tex;

    void loadPaletteFromAttributeTable(const unsigned short NtPos);
    void loadSetOf4Colors(const int Pal);
    void GenerateCHRBitmapWithoutPalette(bool SubBank, unsigned Tile, unsigned char (&Chr)[8][8]);
    void GenerateCHRBitmap(Color32 ChrBitmap[], unsigned char (&Chr)[8][8]);
    void generateTileWithoutCache(int i, Color32 chrImage[64]);
    void setCurSubBank(bool SubBank);

    //GFX
    GraphixEngine gfx;
    Color32 colorPalette[64];
    Color32 setOf4ColorsPalette[4];

    //Other
    struct InterruptLines &ints;
    Board &mapper;

    //Debugger
    std::function<void(Breakpoint*)> debugProcess;
    bool debugNextTick;
    std::vector<PPUTime> breakpointByTime;
    std::vector<PPUAddress> breakpointByAddress;
    unsigned breakpointByAddressNum;
    std::vector<PPUValue> breakpointByValue;
    unsigned breakpointByValueNum;

    unsigned char getPaletteFromAttributeTable(const unsigned short NtPos);
    void generateCHR(Color32 chrImage[64], const unsigned char Tile, const unsigned char Palette, const bool SubBank);
    void generateNT(Color32 chrImage[64], unsigned short VAddress);
    void generateOam(Color32 chrImage[64], unsigned char OamNum);
};

#endif // PPU_H_INCLUDED
