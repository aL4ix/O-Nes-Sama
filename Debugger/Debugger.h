#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iomanip>
#include <sstream>

#include "../PPU.h"
#include "../CPU.h"
#include "Breakpoint.h"
#include "SocketServer.h"

enum ParserState {PARSER_RELEASE = -2, PARSER_QUIT = -1, PARSER_MALFORMED = 0, PARSER_OK};

class Debugger : public SocketServer
{
public:
    Debugger(CPU *Cpu, PPU* Ppu);
    ~Debugger();
    bool pollHandshake();
    void breakpointReached(Breakpoint* breakpoint);
    bool addBreakpointPPUByTime(int Scanline, int Tick);
    bool addBreakpointPPUByAddress(unsigned Address);
    bool addBreakpointPPUByValue(unsigned Address, unsigned Value);
    bool deleteBreakpoint(unsigned Number);
    void handleRequests();
    unsigned char getPPUMemValue(unsigned short Address);
    unsigned char getPPUOamValue(unsigned char Address);
    void setPPUMemValue(unsigned short Address, unsigned char Value);
    void setPPUOamValue(unsigned char Address, unsigned char Value);
    void generateBitmapChr(unsigned short Address);
    void generateRendered(unsigned Address, Color32 bitmap[8*8]);
    void generateRenderedNt(unsigned short Address);
    void generateRenderedOam(unsigned short Address);
    void disableDebugger(unsigned& AddressNum, unsigned& ValueNum);
    void enableDebugger(unsigned& AddressNum, unsigned& ValueNum);

protected:
private:
    CPU* cpu;
    PPU* ppu;
    std::unordered_map<Breakpoint*, unsigned, hash_breakpoint, pred_breakpoint> map;
    unsigned lastGlobalBreakpoint;
    stringstream ss;

    std::string getNext();
    bool getNumber(unsigned& Number);
    bool str2Number(const std::string& Str, unsigned& Number);
    void constructMessageAndSend(const char* fmt, ...);
    int execute(char* Instruction);
    bool quit();
    bool set();
    bool setBreakpoint();
    bool get();
    bool getBreakpoint();
    bool getPpumem();
    bool getPpuoam();
    bool setPpumem();
    bool setPpuoam();
    bool getBitmap();
    bool getRendered();
    bool getPpureg();
};

#endif // DEBUGGER_H
