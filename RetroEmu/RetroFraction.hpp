#ifndef RETRO_FRACTION_HPP_INCLUDED
#define RETRO_FRACTION_HPP_INCLUDED

class RetroFraction {
public:
    RetroFraction(unsigned total, unsigned dividedBy);
    unsigned getNextSlice();

private:
    unsigned slice;
    unsigned fracAccumulated;
    unsigned fracNumerator;
    unsigned fracDenominator;
};

#endif // RETRO_FRACTION_HPP_INCLUDED
