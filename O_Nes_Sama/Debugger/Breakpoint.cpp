#include "Breakpoint.h"


std::vector<PPUTime>* BreakpointPPUByTime::container = NULL;
std::vector<PPUAddress>* BreakpointPPUByAddress::container = NULL;
std::vector<PPUValue>* BreakpointPPUByValue::container = NULL;

///////////////////////////////////////////////////////////////////////////////

BreakpointPPUByTime::BreakpointPPUByTime(int Scanline, int Tick)
{
    scanline = Scanline;
    tick = Tick;
}
size_t BreakpointPPUByTime::hash() const
{
    return std::hash<int>()(scanline) ^ std::hash<int>()(tick);
}

bool BreakpointPPUByTime::equals(const Breakpoint* a) const
{
    const BreakpointPPUByTime* rhs = dynamic_cast<const BreakpointPPUByTime*>(a);
    if(rhs)
    {
        return (scanline == rhs->scanline) && (tick == rhs->tick);
    }
    return false;
}

std::string BreakpointPPUByTime::toString() const
{
    char buffer1[21];
    char buffer2[21];
    sprintf(buffer1, "%X", scanline);
    sprintf(buffer2, "%X", tick);
    return std::string("ppu time ")+buffer1+' '+buffer2;
}

bool BreakpointPPUByTime::deleteFromContainer()
{
    if(container)
    {
        for(auto it = container->begin(); it != container->end(); it++)
            if(it->scanline == this->scanline)
                if(it->tick == this->tick)
                {
                    container->erase(it);
                    return true;
                }
    }
    return false;
}

void BreakpointPPUByTime::SetContainer(std::vector<PPUTime>* Container)
{
    container = Container;
}

///////////////////////////////////////////////////////////////////////////////

BreakpointPPUByAddress::BreakpointPPUByAddress(unsigned Address)
{
    address = Address;
}

size_t BreakpointPPUByAddress::hash() const
{
    return std::hash<unsigned>()(address);
}

bool BreakpointPPUByAddress::equals(const Breakpoint* a) const
{
    const BreakpointPPUByAddress* rhs = dynamic_cast<const BreakpointPPUByAddress*>(a);
    if(rhs)
    {
        return (address == rhs->address);
    }
    return false;
}

std::string BreakpointPPUByAddress::toString() const
{
    char buffer[21];
    sprintf(buffer, "%X", address);
    return std::string("ppu address ")+buffer;
}

bool BreakpointPPUByAddress::deleteFromContainer()
{
    if(container)
    {
        for(auto it = container->begin(); it != container->end(); it++)
            if(it->address == this->address)
            {
                container->erase(it);
                return true;
            }
    }
    return false;
}

void BreakpointPPUByAddress::SetContainer(std::vector<PPUAddress>* Container)
{
    container = Container;
}

///////////////////////////////////////////////////////////////////////////////

BreakpointPPUByValue::BreakpointPPUByValue(unsigned Address, unsigned Value)
{
    address = Address;
    value = Value;
}

size_t BreakpointPPUByValue::hash() const
{
    return std::hash<unsigned>()(address) ^ std::hash<unsigned>()(value);
}

bool BreakpointPPUByValue::equals(const Breakpoint* a) const
{
    const BreakpointPPUByValue* rhs = dynamic_cast<const BreakpointPPUByValue*>(a);
    if(rhs)
    {
        return (address == rhs->address) && (value == rhs->value);
    }
    return false;
}

std::string BreakpointPPUByValue::toString() const
{
    char buffer1[21];
    char buffer2[21];
    sprintf(buffer1, "%X", address);
    sprintf(buffer2, "%X", value);
    return std::string("ppu value ")+buffer1+' '+buffer2;
}

bool BreakpointPPUByValue::deleteFromContainer()
{
    if(container)
    {
        for(auto it = container->begin(); it != container->end(); it++)
            if(it->address == this->address)
                if(it->value == this->value)
                {
                    container->erase(it);
                    return true;
                }
    }
    return false;
}

void BreakpointPPUByValue::SetContainer(std::vector<PPUValue>* Container)
{
    container = Container;
}
