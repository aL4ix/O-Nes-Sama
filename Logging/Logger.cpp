#include "Logger.h"

Logger::Logger()
    : printDebugCategory(false)
    , out(nullptr)

{
    setOutput();
}

Logger::~Logger()
{
    if (out)
        out->flush();
}

void Logger::setOutput(const char* fileName)
{
    /*If no file name specified, then log to the standard output.*/
    if (fileName != nullptr)
        out = new std::ofstream(fileName /*, std::ios_base::ate|std::ios_base::app*/);
    else
        out = &std::cout;
}

bool Logger::debugWithVarArgs(LogCategory category, const char* format, va_list args)
{
    if (out) {
        for (auto it = enabledCategories.cbegin(); it != enabledCategories.cend(); ++it) {
            /*
            If both values are exactly the same then the *bitwise_xor* will be 0, no difference.
            If *it* has a category from the same section than *category*, let's say APU_PULSE1 and APU_PULSE2
            the *bitwise_xor* will return less than 0x100. (All sections are like APU 0x100, CPU 0x200, etc).
            If they are from the same section and at least one of them is a *ALL* section like APU_ALL then
            it should be logged, because if we enabled APU_ALL then APU_TRIANGLE should be logged, or if we
            enabled APU_TRIANGLE but if we want to log a message for APU_ALL then it should be logged too.
            */
            int it_value = static_cast<short>(*it);
            int cat_value = static_cast<short>(category);
            auto bitwise_xor = it_value ^ cat_value;
            auto isAnyOfThemASection = it_value & cat_value & 0xFF;
            if (bitwise_xor == 0 || (bitwise_xor < 0x100 && isAnyOfThemASection == 0)) {
                // Finally print the message
                if (printDebugCategory) {
                    *out << logCategoryToString(category) << ": ";
                }
                logWithVarArgs(format, args);
                return true;
            }
        }
    }
    return false;
}

#ifdef ENABLE_DEBUG_LOGGER
void Logger::debug(LogCategory category, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    debugWithVarArgs(category, format, args);
    va_end(args);
}

void Logger::debugWithFilter(LogCategory category, int variableUsedForFilter, const char* format, ...)
{
    for (auto it = whitelistFilter.cbegin(); it != whitelistFilter.cend(); it++) {
        int it_value = *it;
        if (it_value == variableUsedForFilter) {
            va_list args;
            va_start(args, format);
            if (debugWithVarArgs(category, format, args)) {
                return;
            }
            va_end(args);
        }
    }
}
#endif

void Logger::enableDebugCategory(LogCategory category)
{
    enabledCategories.insert(category);
    debug(LogCategory::otherLogger, "Enabled");
}

void Logger::disableDebugCategory(LogCategory category)
{
    debug(LogCategory::otherLogger, "Disabled");
    enabledCategories.erase(category);
}

void Logger::disableAllDebugCategories()
{
    debug(LogCategory::otherLogger, "Disabled All");
    enabledCategories.clear();
}

void Logger::addToDebugWhitelistFilter(int variableUsedForFilter)
{
    whitelistFilter.insert(variableUsedForFilter);
}

void Logger::removeFromDebugWhitelistFilter(int variableUsedForFilter)
{
    whitelistFilter.erase(variableUsedForFilter);
}

void Logger::clearDebugWhitelistFilter()
{
    whitelistFilter.clear();
}

void Logger::setPrintDebugCategory(bool printDebugCategory)
{
    this->printDebugCategory = printDebugCategory;
}

void Logger::error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    logWithVarArgs(format, args);
    va_end(args);
}

void Logger::info(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    logWithVarArgs(format, args);
    va_end(args);
}

void Logger::logWithVarArgs(const char* format, va_list args)
{
    vsnprintf(buffer, sizeof(buffer), format, args);
    *out << buffer << std::endl;
}

Logger Log;
