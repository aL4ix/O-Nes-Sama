#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <stdio.h>
#include <string>
#include <vector>


class Breakpoint
{
public:
    virtual ~Breakpoint()
    {
    }
    virtual size_t hash() const = 0;
    virtual bool equals(const Breakpoint* a) const = 0;
    virtual std::string toString() const = 0;
    virtual bool deleteFromContainer() = 0;

protected:
    Breakpoint()
    {
    }
};

struct hash_breakpoint
{
    size_t operator()(const Breakpoint* a ) const
    {
        return a->hash();
    }
};

struct pred_breakpoint
{
    bool operator()(const Breakpoint* a, const Breakpoint *b) const
    {
        return a->equals(b);
    }
};

class PPUTime
{
public:
    int scanline;
    int tick;
};

class PPUAddress
{
public:
    unsigned address;
};

class PPUValue
{
public:
    unsigned address;
    unsigned value;
};

class BreakpointPPUByTime : public Breakpoint, PPUTime
{
public:
    BreakpointPPUByTime(int Scanline, int Tick);
    size_t hash() const;
    bool equals(const Breakpoint* a) const;
    std::string toString() const;
    bool deleteFromContainer();
    static void SetContainer(std::vector<PPUTime>* Container);

private:
    static std::vector<PPUTime>* container;
};

class BreakpointPPUByAddress : public Breakpoint, PPUAddress
{
public:
    explicit BreakpointPPUByAddress(unsigned Address);
    size_t hash() const;
    bool equals(const Breakpoint* a) const;
    std::string toString() const;
    bool deleteFromContainer();
    static void SetContainer(std::vector<PPUAddress>* Container);

private:
    static std::vector<PPUAddress>* container;
};

class BreakpointPPUByValue : public Breakpoint, PPUValue
{
public:
    BreakpointPPUByValue(unsigned Address, unsigned Value);
    size_t hash() const;
    bool equals(const Breakpoint* a) const;
    std::string toString() const;
    bool deleteFromContainer();
    static void SetContainer(std::vector<PPUValue>* Container);

private:
    static std::vector<PPUValue>* container;
};

#endif // BREAKPOINT_H
