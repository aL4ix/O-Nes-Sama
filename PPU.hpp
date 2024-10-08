#ifndef PPU_HPP_INCLUDED
#define PPU_HPP_INCLUDED
#include <cstring>
#include <functional>
#include <unordered_map>
#include <vector>

#include "CPUIO.hpp"
#include "Cartridge/Cartridge.hpp"
#include "Logging/Logger.hpp"
#include "RetroEmu/RetroColor.hpp"

class PPU_State {
protected:
    int scanlineNum;
    bool isRendering;
    int ticks;
    int ticksInThisScanline;
    bool isOdd;
    unsigned char palette[0x20];
    unsigned char oam[64 * 4 + 8 * 4];
    unsigned char generalLatch;
    unsigned char reg2000;
    unsigned char reg2001;
    unsigned char reg2002;
    unsigned short loopy_v;
    bool writeToggle;
    unsigned char buf2007;
    unsigned short postFetchAddr;
    unsigned char oamAddress;
    unsigned oamPointer;
    unsigned short loopy_t;
    unsigned char loopy_x;
    int zeroHit;
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
    unsigned char spriteY;
    unsigned char spriteT;
    unsigned char spriteA;
    unsigned char spriteX;
    unsigned char spriteEvalN;
    unsigned char spriteEvalM;
    unsigned char spriteEvalS;
    bool oamCompleted;
    bool oamSecondaryFull;
    unsigned char secNum;
};

class PPU : public PPU_State {
public:
    PPU(struct CPUIO& cio, MemoryMapper& m);
    ~PPU();
    void writeMem(unsigned short address, unsigned char value);
    unsigned char readMem(unsigned short address);
    void process(int cpuCycles);
    void displayFrame();
    void reset();
    void powerOn();
    void saveState(FILE* file);
    bool loadState(FILE* file);
    unsigned char* getPalettedFrameBuffer();
    unsigned char* getDefaultPalette();

    static constexpr unsigned INTERNAL_WIDTH = 256;
    static constexpr unsigned INTERNAL_HEIGHT = 240;
    static constexpr unsigned INTERNAL_FPS = 60;

private:
    // INTERNALS
    unsigned char* nametable[4];
    unsigned char* chr[8];

    void triggerNMI();
    void clearNMI();
    unsigned char readLatch();
    unsigned char read2002();
    unsigned char read2004();
    unsigned char read2007();
    void write2000(unsigned char value);
    void write2001(unsigned char value);
    void write2002(unsigned char value);
    void write2003(unsigned char value);
    void write2004(unsigned char value);
    void write2005(unsigned char value);
    void write2006(unsigned char value);
    void write2007(unsigned char value);
    void (PPU::*writeFuncs[8])(unsigned char) = { &PPU::write2000, &PPU::write2001, &PPU::write2002, &PPU::write2003, &PPU::write2004, &PPU::write2005, &PPU::write2006, &PPU::write2007 };
    unsigned char (PPU::*readFuncs[8])(void) = { &PPU::readLatch, &PPU::readLatch, &PPU::read2002, &PPU::readLatch, &PPU::read2004, &PPU::readLatch, &PPU::readLatch, &PPU::read2007 };
    unsigned char intReadMem(unsigned short address);
    void intWriteMem(unsigned short address, unsigned char value);
    unsigned char intReadMemLean(unsigned short address, bool updateBus = true);
    void intWriteMemLean(unsigned short address, unsigned char value, bool updateBus = true);
    void coarseX();
    // void coarseY();
    // void deCoarseX();
    void coarseFineY();
    void renderTick();

    // RENDERING
    bool frameBufferReady = false;
    unsigned char palettedFrameBuffer[240 * 256];

    void loadPaletteFromAttributeTable(const unsigned short ntPos);
    void loadSetOf4Colors(const int pal);
    void generateCHRBitmapWithoutPalette(bool subBank, unsigned tile, unsigned char (&chr)[8][8]);
    void generateCHRBitmap(Color32 chrBitmap[], unsigned char (&chr)[8][8]);
    // void generateTileWithoutCache(int i, Color32 chrImage[64]);
    void generateCHR(Color32 chrImage[64], const unsigned char tile, const unsigned char palette, const bool subBank);

    // GFX
    unsigned char defaultPalette[192] = { 70, 70, 70, 0, 6, 90, 0, 6, 120, 2, 6, 115, 53, 3, 76, 87, 0, 14, 90, 0, 0, 65, 0, 0, 18, 2, 0, 0, 20, 0, 0, 30, 0, 0, 30, 0, 0, 21, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 157, 157, 157, 0, 74, 185, 5, 48, 225, 87, 24, 218, 159, 7, 167, 204, 2, 85, 207, 11, 0, 164, 35, 0, 92, 63, 0, 11, 88, 0, 0, 102, 0, 0, 103, 19, 0, 94, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 255, 255, 31, 158, 255, 83, 118, 255, 152, 101, 255, 252, 103, 255, 255, 108, 179, 255, 116, 102, 255, 128, 20, 196, 154, 0, 113, 179, 0, 40, 196, 33, 0, 200, 116, 0, 191, 208, 43, 43, 43, 0, 0, 0, 0, 0, 0, 254, 255, 255, 158, 213, 255, 175, 192, 255, 208, 184, 255, 254, 191, 255, 255, 192, 224, 255, 195, 189, 255, 202, 156, 231, 213, 139, 197, 223, 142, 166, 230, 163, 148, 232, 197, 146, 228, 235, 167, 167, 167, 0, 0, 0, 0, 0, 0 };
    // RetroGraphics gfx;

    Color32 setOf4ColorsPalette[4];

    // Other
    struct CPUIO& cpuIO;
    MemoryMapper& mapper;

    unsigned char getPaletteFromAttributeTable(const unsigned short ntPos);
    void generateNT(Color32 chrImage[64], unsigned short vAddress);
    void generateOam(Color32 chrImage[64], unsigned char oamNum);

    void setOam(const unsigned address, const unsigned char value);
    unsigned char getOam(const unsigned address);
    inline void setSecondary(const unsigned address, const unsigned char value);
    inline unsigned char getSecondary(const unsigned address);
    void (PPU::*spriteFuncs[342])();
    void spriteSecondaryClear();
    void spriteEvaluationStarts();
    void spriteEvaluationOdd();
    void spriteEvaluationEven();
    void spriteEvaluationBackRend();
    void spriteEvaluationTileLoading();
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

#endif // PPU_HPP_INCLUDED
