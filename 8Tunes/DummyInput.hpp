#ifndef INPUT_HPP_INCLUDED
#define INPUT_HPP_INCLUDED

class Input {
public:
    CPUIO* cpuIO;

    unsigned char read(int addr)
    {
        return 0;
    }
    void write(int addr, unsigned char val)
    {
    }
};

#endif // INPUT_HPP_INCLUDED
