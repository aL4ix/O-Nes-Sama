#include "Debugger.h"

Debugger::Debugger(CPU* Cpu, PPU* Ppu)
{
    cpu = Cpu;
    ppu = Ppu;
    ppu->debugProcess = [this](Breakpoint* breakpoint) { breakpointReached(breakpoint); };
    BreakpointPPUByTime::SetContainer(&ppu->breakpointByTime);
    BreakpointPPUByAddress::SetContainer(&ppu->breakpointByAddress);
    BreakpointPPUByValue::SetContainer(&ppu->breakpointByValue);
    lastGlobalBreakpoint = 0;
}

Debugger::~Debugger()
{
    // dtor
}

bool Debugger::pollHandshake()
{
    char* data;
    const char* handshake1 = "Ando buscando al debugger, aqui vive?";
    const char* handshake2 = "Si senor, aqui vive.";

    if (!(data = ReceiveString()))
        return false;
    if (strncmp(data, handshake1, 512) != 0)
        return false;

    SendString(handshake2);
    return true;
}

bool Debugger::addBreakpointPPUByTime(int Scanline, int Tick)
{
    if (!map.insert(std::pair<BreakpointPPUByTime*, unsigned>(new BreakpointPPUByTime(Scanline, Tick), lastGlobalBreakpoint)).second)
        return false;
    lastGlobalBreakpoint++;
    ppu->breakpointByTime.push_back((PPUTime) { Scanline, Tick });
    return true;
}

bool Debugger::addBreakpointPPUByAddress(unsigned Address)
{
    if (!map.insert(std::pair<BreakpointPPUByAddress*, unsigned>(new BreakpointPPUByAddress(Address), lastGlobalBreakpoint)).second)
        return false;
    lastGlobalBreakpoint++;
    ppu->breakpointByAddress.push_back((PPUAddress) { Address });
    ppu->breakpointByAddressNum = ppu->breakpointByAddress.size();
    return true;
}

bool Debugger::addBreakpointPPUByValue(unsigned Address, unsigned Value)
{
    if (!map.insert(std::pair<BreakpointPPUByValue*, unsigned>(new BreakpointPPUByValue(Address, Value), lastGlobalBreakpoint)).second)
        return false;
    lastGlobalBreakpoint++;
    ppu->breakpointByValue.push_back((PPUValue) { Address, Value });
    ppu->breakpointByValueNum = ppu->breakpointByValue.size();
    return true;
}

bool Debugger::deleteBreakpoint(unsigned Number)
{
    for (auto elem : map) {
        if (elem.second == Number) {
            Breakpoint* b = elem.first;
            bool ret = b->deleteFromContainer();
            map.erase(b);
            return ret;
        }
    }
    return false;
}

void Debugger::breakpointReached(Breakpoint* Breakpoint)
{
    unsigned globalBreakpoint = map.at(Breakpoint);
    constructMessageAndSend("REACHED %X", globalBreakpoint);
    return handleRequests();
}

unsigned char Debugger::getPPUMemValue(unsigned short Address)
{
    unsigned addressNum;
    unsigned valueNum;
    disableDebugger(addressNum, valueNum);
    unsigned char ret = ppu->intReadMem(Address);
    enableDebugger(addressNum, valueNum);
    return ret;
}

unsigned char Debugger::getPPUOamValue(unsigned char Address)
{
    return ppu->oam[Address];
}

void Debugger::setPPUMemValue(unsigned short Address, unsigned char Value)
{
    unsigned addressNum;
    unsigned valueNum;
    disableDebugger(addressNum, valueNum);
    ppu->intWriteMem(Address, Value);
    enableDebugger(addressNum, valueNum);
}

void Debugger::setPPUOamValue(unsigned char Address, unsigned char Value)
{
    ppu->oam[Address] = Value;
}

void Debugger::generateBitmapChr(unsigned short Address)
{
    unsigned char chr[8][8];
    bool subBank = (Address & 0x100) >> 8;
    unsigned char tile = (Address & 0xFF);
    ppu->GenerateCHRBitmapWithoutPalette(subBank, tile, chr);
    std::stringstream stream;
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            int i = chr[y][x];
            stream << std::hex << i;
        }
    constructMessageAndSend("REPLY %s", stream.str().c_str());
}

void Debugger::generateRendered(unsigned Address, Color32 bitmap[8 * 8])
{
    std::stringstream stream;

    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            int i = bitmap[y * 8 + x].GetR();
            stream << std::hex << std::setw(2) << std::uppercase << std::setfill('0') << i;
            i = bitmap[y * 8 + x].GetG();
            stream << std::hex << std::setw(2) << std::uppercase << std::setfill('0') << i;
            i = bitmap[y * 8 + x].GetB();
            stream << std::hex << std::setw(2) << std::uppercase << std::setfill('0') << i;
        }

    constructMessageAndSend("REPLY %s", stream.str().c_str());
}

void Debugger::generateRenderedNt(unsigned short Address)
{
    Color32 bitmap[8 * 8];
    ppu->generateNT(bitmap, Address);

    generateRendered(Address, bitmap);
}

void Debugger::generateRenderedOam(unsigned short Address)
{
    Color32 bitmap[8 * 8];
    ppu->generateOam(bitmap, Address);

    generateRendered(Address, bitmap);
}

void Debugger::disableDebugger(unsigned& AddressNum, unsigned& ValueNum)
{
    AddressNum = ppu->breakpointByAddressNum;
    ValueNum = ppu->breakpointByValueNum;
    ppu->breakpointByAddressNum = 0;
    ppu->breakpointByValueNum = 0;
}

void Debugger::enableDebugger(unsigned& AddressNum, unsigned& ValueNum)
{
    ppu->breakpointByAddressNum = AddressNum;
    ppu->breakpointByValueNum = ValueNum;
}

void Debugger::handleRequests()
{
    while (true) {
        char* msg = NULL;
        while ((msg = ReceiveString()) == NULL) {
        }
        switch (execute(msg)) {
        case PARSER_QUIT:
        case PARSER_RELEASE:
            return;
        case PARSER_MALFORMED:
            SendString("FAIL malformed instruction");
            break;
        }
    }
    return;
}

// PARSER
int Debugger::execute(char* Instruction)
{
    // printf("EXECUTING: %s\n", Instruction);
    ss.clear();
    ss.str(Instruction);

    std::string s = getNext();
    do {
        if (s == "QUIT") {
            quit();
            return PARSER_QUIT;
        } else if (s == "CONTINUE") {
            return PARSER_RELEASE;
        } else if (s == "SET") {
            return set();
        } else if (s == "GET") {
            return get();
        } else {
            return PARSER_MALFORMED;
        }
    } while ((s = getNext()) != "");

    return PARSER_OK;
}

bool Debugger::quit()
{
    SendString("OK");
    ppu->breakpointByAddressNum = 0;
    ppu->breakpointByValueNum = 0;
    ppu->breakpointByTime.resize(0);
    ppu->breakpointByAddress.resize(0);
    ppu->breakpointByValue.resize(0);
    return PARSER_OK;
}

bool Debugger::set()
{
    std::string str = getNext();
    if (str == "breakpoint") {
        return setBreakpoint();
    } else if (str == "ppumem") {
        return setPpumem();
    } else if (str == "ppuoam") {
        return setPpuoam();
    }
    return PARSER_MALFORMED;
}

std::string Debugger::getNext()
{
    std::string str;
    getline(ss, str, ' ');
    return str;
}

bool Debugger::getNumber(unsigned& Number)
{
    std::string str = getNext();
    if (str == "")
        return PARSER_MALFORMED;
    return str2Number(str, Number);
}

bool Debugger::str2Number(const std::string& Str, unsigned& Number)
{
    std::stringstream param(Str);
    param >> std::hex >> Number;
    if (!param)
        return PARSER_MALFORMED;
    return true;
}

bool Debugger::get()
{
    std::string str = getNext();
    if (str == "breakpoint") {
        return getBreakpoint();
    } else if (str == "ppumem") {
        return getPpumem();
    } else if (str == "ppuoam") {
        return getPpuoam();
    } else if (str == "bitmap") {
        return getBitmap();
    } else if (str == "rendered") {
        return getRendered();
    } else if (str == "ppureg") {
        return getPpureg();
    }
    return PARSER_MALFORMED;
}

void Debugger::constructMessageAndSend(const char* format, ...)
{
    char msg[512];
    va_list arg;

    va_start(arg, format);
    vsprintf(msg, format, arg);
    va_end(arg);
    SendString(msg);
}

bool Debugger::setBreakpoint()
{
    std::string str = getNext();
    unsigned bp = -1;
    if (str == "new")
        bp = lastGlobalBreakpoint;
    else {
        if (!str2Number(str, bp))
            return PARSER_MALFORMED;
    }
    str = getNext();
    if (str == "ppu") {
        str = getNext();
        if (str == "time") {
            unsigned scanline;
            unsigned tick;
            if (!getNumber(scanline))
                return PARSER_MALFORMED;
            if (!getNumber(tick))
                return PARSER_MALFORMED;
            if (!addBreakpointPPUByTime(scanline, tick)) {
                SendString("FAIL while setting breakpoint");
                return PARSER_OK;
            }
            constructMessageAndSend("REPLY %X", bp);
            return PARSER_OK;
        } else if (str == "address") {
            unsigned address;
            if (!getNumber(address))
                return PARSER_MALFORMED;
            if (!addBreakpointPPUByAddress(address)) {
                SendString("FAIL while setting breakpoint");
                return PARSER_OK;
            }
            constructMessageAndSend("REPLY %X", bp);
            return PARSER_OK;
        } else if (str == "value") {
            unsigned address;
            unsigned value;
            if (!getNumber(address))
                return PARSER_MALFORMED;
            if (!getNumber(value))
                return PARSER_MALFORMED;
            if (!addBreakpointPPUByValue(address, value)) {
                SendString("FAIL while setting breakpoint");
                return PARSER_OK;
            }
            constructMessageAndSend("REPLY %X", bp);
            return PARSER_OK;
        }
    } else if (str == "delete") {
        if (deleteBreakpoint(bp))
            SendString("OK");
        else
            SendString("FAIL when deleting breakpoint");
        return PARSER_OK;
    }
    return PARSER_MALFORMED;
}

bool Debugger::getBreakpoint()
{
    std::string str = getNext();
    unsigned bp = -1;
    if (str2Number(str, bp)) {
        // FOUND Number
    } else
        return PARSER_MALFORMED;
    const Breakpoint* breakpoint = NULL;
    for (auto& pos : map) {
        if (pos.second == bp)
            breakpoint = pos.first;
    }
    if (breakpoint == NULL) {
        SendString("FAIL breakpoint doesn't exist");
        return PARSER_OK;
    }
    constructMessageAndSend("REPLY %s", breakpoint->toString().c_str());
    return PARSER_OK;
}

bool Debugger::getPpumem()
{
    unsigned address = -1;
    if (!getNumber(address))
        return PARSER_MALFORMED;
    unsigned char data = getPPUMemValue(address);
    constructMessageAndSend("REPLY %X", data);
    return PARSER_OK;
}

bool Debugger::getPpuoam()
{
    unsigned address = -1;
    if (!getNumber(address))
        return PARSER_MALFORMED;
    unsigned char data = getPPUOamValue(address);
    constructMessageAndSend("REPLY %X", data);
    return PARSER_OK;
}

bool Debugger::setPpumem()
{
    unsigned address = -1;
    if (!getNumber(address))
        return PARSER_MALFORMED;
    unsigned value = -1;
    if (!getNumber(value))
        return PARSER_MALFORMED;
    setPPUMemValue(address, value);
    SendString("OK");
    return PARSER_OK;
}

bool Debugger::setPpuoam()
{
    unsigned address = -1;
    if (!getNumber(address))
        return PARSER_MALFORMED;
    unsigned value = -1;
    if (!getNumber(value))
        return PARSER_MALFORMED;
    setPPUOamValue(address, value);
    SendString("OK");
    return PARSER_OK;
}

bool Debugger::getBitmap()
{
    std::string next = getNext();
    if (next == "chr") {
        unsigned address = -1;
        if (!getNumber(address))
            return PARSER_MALFORMED;
        generateBitmapChr(address);
        return PARSER_OK;
    }
    return PARSER_MALFORMED;
}

bool Debugger::getRendered()
{
    std::string next = getNext();
    if (next == "nt") {
        unsigned address = -1;
        if (!getNumber(address))
            return PARSER_MALFORMED;
        generateRenderedNt(address);
        return PARSER_OK;
    }
    if (next == "oam") {
        unsigned address = -1;
        if (!getNumber(address))
            return PARSER_MALFORMED;
        generateRenderedOam(address);
        return PARSER_OK;
    }
    return PARSER_MALFORMED;
}

bool Debugger::getPpureg()
{
    std::string next = getNext();
    if (next == "reg2000") {
        constructMessageAndSend("REPLY %X", ppu->reg2000);
        return PARSER_OK;
    } else if (next == "reg2001") {
        constructMessageAndSend("REPLY %X", ppu->reg2001);
        return PARSER_OK;
    } else if (next == "reg2002") {
        constructMessageAndSend("REPLY %X", ppu->reg2002);
        return PARSER_OK;
    } else if (next == "scanline") {
        constructMessageAndSend("REPLY %X", ppu->scanlineNum);
        return PARSER_OK;
    } else if (next == "ticks") {
        constructMessageAndSend("REPLY %X", ppu->ticks);
        return PARSER_OK;
    } else if (next == "loopy_v") {
        constructMessageAndSend("REPLY %X", ppu->loopy_v);
        return PARSER_OK;
    } else if (next == "loopy_t") {
        constructMessageAndSend("REPLY %X", ppu->loopy_t);
        return PARSER_OK;
    } else if (next == "loopy_x") {
        constructMessageAndSend("REPLY %X", ppu->loopy_x);
        return PARSER_OK;
    }
    return PARSER_MALFORMED;
}
