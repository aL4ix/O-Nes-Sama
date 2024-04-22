#ifndef LOG_CATEGORY_HPP_INCLUDED
#define LOG_CATEGORY_HPP_INCLUDED

enum class LogCategory : unsigned short {
    none = 0,
    apuAll = 0x100,
    apuTriangle,
    apuPulse1,
    apuPulse2,
    apuNoise,
    coreAll = 0x200,
    coreMain,
    cpuAll = 0x300,
    cpuLogger,
    inputAll = 0x400,
    loaderAll = 0x500,
    loaderCart,
    loaderNSF,
    mapperAll = 0x600,
    mapperAXROM,
    mapperBasic,
    mapperCNROM,
    mapperGXROM,
    mapperMemoryMapper,
    mapperMMC1,
    mapperMMC2,
    mapperMMC3,
    mapperUXROM,
    mapperNSF,
    ppuAll = 0x700,
    ppuRend,
    ppuRendSprColor,
    ppuRendTransp,
    ppuRendSprPalFront,
    ppuSetOAM,
    ppuGetOAM,
    ppuSprEvalSecClear,
    ppuSprEvalStarts,
    ppuSprEvalOdd,
    ppuSprEvalEven,
    ppuFetchOamLow,
    ppuFetchOamHigh,
    otherAll = 0x800,
    otherLogger

    // SUBSECTION_MASK = 0x00FF,
    // SECTION_MASK = 0xF00,
};

const char* logCategoryToString(LogCategory category) noexcept;

#endif // LOG_CATEGORY_HPP_INCLUDED
