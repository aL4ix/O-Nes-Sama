#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <cstdarg>
#include <fstream>
#include <iostream>
#include <set>

#include "LogCategory.hpp"

/*TODO: Support switching between std out and file or both at the same time*/

class Logger {
public:
    Logger();
    ~Logger();
    void setOutput(const char* fileName = nullptr);
    void debug(LogCategory category, const char* format, ...);
    void enableDebugCategory(LogCategory category);
    void disableDebugCategory(LogCategory category);
    void disableAllDebugCategories();
    void debugWithFilter(LogCategory category, int variableUsedForFilter, const char* format, ...);
    void addToDebugWhitelistFilter(int variableUsedForFilter);
    void removeFromDebugWhitelistFilter(int variableUsedForFilter);
    void clearDebugWhitelistFilter();
    void setPrintDebugCategory(bool printDebugCategory);
    void info(const char* format, ...);
    void error(const char* format, ...);

private:
    char buffer[1024];
    std::set<LogCategory> enabledCategories;
    bool debugWithVarArgs(LogCategory category, const char* format, va_list args);
    std::set<int> whitelistFilter;
    bool printDebugCategory;
    void logWithVarArgs(const char* format, va_list args);

private:
    std::ostream* out;
};

extern Logger Log;

#endif // LOGGER_H_INCLUDED
