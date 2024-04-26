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
        CASE(none);
        CASE(apuAll);
        CASE(apuTriangle);
        CASE(apuPulse1);
        CASE(apuPulse2);
        CASE(apuNoise);
        CASE(coreAll);
        CASE(coreMain);
        CASE(cpuAll);
        CASE(cpuLogger);
        CASE(inputAll);
        CASE(loaderAll);
        CASE(loaderCart);
        CASE(loaderNSF);
        CASE(mapperAll);
        CASE(mapperAXROM);
        CASE(mapperBasic);
        CASE(mapperCNROM);
        CASE(mapperGXROM);
        CASE(mapperMemoryMapper);
        CASE(mapperMMC1);
        CASE(mapperMMC2);
        CASE(mapperMMC3);
        CASE(mapperUXROM);
        CASE(mapperNSF);
        CASE(ppuAll);
        CASE(ppuRend);
        CASE(ppuRendSprColor);
        CASE(ppuRendTransp);
        CASE(ppuRendSprPalFront);
        CASE(ppuSetOAM);
        CASE(ppuGetOAM);
        CASE(ppuSprEvalSecClear);
        CASE(ppuSprEvalStarts);
        CASE(ppuSprEvalOdd);
        CASE(ppuSprEvalEven);
        CASE(ppuFetchOamLow);
        CASE(ppuFetchOamHigh);
        CASE(otherAll);
        CASE(otherLogger);
    }
    return "ADD_THE_STRING_FOR_THIS_LOG_CATEGORY";
}
