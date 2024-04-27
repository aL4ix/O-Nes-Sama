#include "LogCategory.hpp"

/* SERIOUSLY i could not find a better way unless a new lib is added or higher cpp version.
   I miss you rust...
   https://belaycpp.com/2021/08/24/best-ways-to-convert-an-enum-to-a-string/
 */

#define CASE(elem)          \
    case LogCategory::elem: \
        return #elem;

const char* logCategoryToString(LogCategory category) noexcept
{
    switch (category) {
        CASE(none)
        CASE(apuAll)
        CASE(apuTriangle)
        CASE(apuPulse1)
        CASE(apuPulse2)
        CASE(apuNoise)
        CASE(apuDMC)
        CASE(apuWriteMem)
        CASE(apuReadMem)
        CASE(apuWrite4001)
        CASE(apuWrite4008)
        CASE(apuWrite400A)
        CASE(apuWrite400B)
        CASE(apuWrite400E)
        CASE(apuWrite4010)
        CASE(apuWrite4015)
        CASE(apuMemReader)
        CASE(apuHalfFrame)
        CASE(apuQuarterFrame)
        CASE(cpuAll)
        CASE(cpuLogger)
        CASE(cpuPreciseTime)
        CASE(cpuRun)
        CASE(cpuOpcodes)
        CASE(inputAll)
        CASE(loaderAll)
        CASE(loaderCart)
        CASE(loaderNSF)
        CASE(mapperAll)
        CASE(mapperAXROM)
        CASE(mapperBasic)
        CASE(mapperCNROM)
        CASE(mapperGXROM)
        CASE(mapperMemoryMapper)
        CASE(mapperMMC1)
        CASE(mapperMMC2)
        CASE(mapperMMC3)
        CASE(mapperUXROM)
        CASE(mapperNSF)
        CASE(onessamaAll)
        CASE(onessamaMain)
        CASE(onessama8Tunes)
        CASE(onessamaCore)
        CASE(onessamaLibretro)
        CASE(ppuAll)
        CASE(ppuRender)
        CASE(ppuRendSprColor)
        CASE(ppuRendTransp)
        CASE(ppuRendSprPalFront)
        CASE(ppuSetOAM)
        CASE(ppuGetOAM)
        CASE(ppuSprEvalSecClear)
        CASE(ppuSprEvalStarts)
        CASE(ppuSprEvalOdd)
        CASE(ppuSprEvalEven)
        CASE(ppuFetchOamLow)
        CASE(ppuFetchOamHigh)
        CASE(ppuWriteMem)
        CASE(ppuReadMem)
        CASE(ppuProcess)
        CASE(ppuTriggerNMI)
        CASE(ppuWrite2000)
        CASE(ppuWrite2001)
        CASE(ppuRead2002)
        CASE(ppuWrite2004)
        CASE(ppuWrite2005)
        CASE(ppuWrite2006)
        CASE(ppuWrite2007)
        CASE(otherAll)
        CASE(otherLogger)
        CASE(otherRetroAudio)
        CASE(otherRetroGraphics)
    }
    return "ADD_THE_STRING_FOR_THIS_LOG_CATEGORY";
}
