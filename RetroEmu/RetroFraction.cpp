#include "RetroFraction.hpp"


RetroFraction::RetroFraction(unsigned total, unsigned dividedBy)
{
    slice = total/dividedBy; // Get the size of a normal slice
    fracAccumulated = 0; // Start the count at 0 so the remaining part will accumulate
    fracNumerator = total % dividedBy; // This is the remaining part, this will accumulate in fracAccumulated
    fracDenominator = dividedBy; //If you divide by 60FPS you will keep the fraction as 60 so you can know when you got a whole frame
}

unsigned RetroFraction::getNextSlice()
{
    unsigned wholePartAccumulated = 0;
    fracAccumulated += fracNumerator;
    if(fracAccumulated >= fracDenominator) // We got a whole part!
    {
        fracAccumulated -= fracDenominator;
        wholePartAccumulated = 1;
    }
    return slice+wholePartAccumulated;
}
