#ifndef PPU_H_INCLUDED
#define PPU_H_INCLUDED
#include <unordered_map>
#include <functional>
#include <vector>
#include <cstring>

#include "x6502Interrupts.h"
#include "RetroGraphix/GraphixEngine.hpp"
#include "RetroGraphix/Texture.hpp"
#include "RetroGraphix/Color.hpp"
#include "RetroGraphix/Sprite.hpp"
#include "Debugger/Breakpoint.h"
#include "Cartridge/Cartridge.hpp"


class PPU_State
{
friend class Debugger;

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
    unsigned char curTile;
    unsigned char curPalette;
    unsigned char curChrLow;
    unsigned char curChrHigh;
    unsigned char latchPalette;
    unsigned short shiftRegisterChrLow;
    unsigned short shiftRegisterChrHigh;
    unsigned char shiftRegisterPalLow;
    unsigned char shiftRegisterPalHigh;
    unsigned char shiftRegisterSpriteLow[8];
    unsigned char shiftRegisterSpriteHigh[8];
    unsigned char latchSpriteAt[8];
    unsigned char counterSpriteX[8];
    unsigned short addressBus;
};

class PPU : public PPU_State
{
friend class Debugger;

public:
    PPU(struct InterruptLines &ints, Board &m);
    ~PPU();
    void writeMem(unsigned short Address, unsigned char Value);
    unsigned char readMem(unsigned short Address);
    void process(int cpuCycles);
    void displayFrame();
    void reset();
    void powerOn();
    void saveState(FILE* File);
    bool loadState(FILE* File);
    //bool loadColorPaletteFromFile(const char* FileName);
    bool loadColorPaletteFromArray(const unsigned char* Palette);
    unsigned char ** ppuCartSpace[16];
    unsigned char ** getChr();
    unsigned char ** getNametables();

private:
    //INTERNALS
    unsigned char * nametable[4];
    unsigned char * chr[8];

    void triggerNMI();
    void clearNMI();
    unsigned char readLatch();
    unsigned char read2002();
    unsigned char read2004();
    unsigned char read2007();
    void write2000(unsigned char Value);
    void write2001(unsigned char Value);
    void write2002(unsigned char Value);
    void write2003(unsigned char Value);
    void write2004(unsigned char Value);
    void write2005(unsigned char Value);
    void write2006(unsigned char Value);
    void write2007(unsigned char Value);
    void (PPU::*writeFuncs[8])(unsigned char) = {&PPU::write2000, &PPU::write2001, &PPU::write2002, &PPU::write2003, &PPU::write2004, &PPU::write2005, &PPU::write2006, &PPU::write2007};
    unsigned char (PPU::*readFuncs[8])(void) = {&PPU::readLatch, &PPU::readLatch, &PPU::read2002, &PPU::readLatch, &PPU::read2004, &PPU::readLatch, &PPU::readLatch, &PPU::read2007};
    unsigned char intReadMem(unsigned short Address);
    void intWriteMem(unsigned short Address, unsigned char Value);
    unsigned char intReadMemLean(unsigned short Address);
    void intWriteMemLean(unsigned short Address, unsigned char Value);
    void coarseX();
    //void coarseY();
    //void deCoarseX();
    void coarseFineY();
    void renderTick();

    //RENDERING
    Color32 framebuffer[240*256];
    Sprite back;
    Texture tex;

    void loadPaletteFromAttributeTable(const unsigned short NtPos);
    void loadSetOf4Colors(const int Pal);
    void GenerateCHRBitmapWithoutPalette(bool SubBank, unsigned Tile, unsigned char (&Chr)[8][8]);
    void GenerateCHRBitmap(Color32 ChrBitmap[], unsigned char (&Chr)[8][8]);
    //void generateTileWithoutCache(int i, Color32 chrImage[64]);
    void generateCHR(Color32 chrImage[64], const unsigned char Tile, const unsigned char Palette, const bool SubBank);

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
    void generateNT(Color32 chrImage[64], unsigned short VAddress);
    void generateOam(Color32 chrImage[64], unsigned char OamNum);

    void (PPU::*tickFuncs[342])();
    void tick255();
    void tick257();
    void tickOamFetchesLow();
    void tickOamFetchesHigh();
    void tickFetchNT();
    void tickFetchAT();
    void tickFetchTileLow();
    void tickFetchTileHigh();
    void tickShiftRegisters();
    void tick328();
};

#endif // PPU_H_INCLUDED
