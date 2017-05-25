#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <stdexcept>
#include <regex>
#include <map>

/*TODO: Support switching between std out and file or both at the same time*/

class Logger {
    public:
        /*Logger signature for CPU logging*/
        Logger();
        Logger(const char * fileName);
        ~Logger();
        void Init(const char * fileName = NULL);
        void LogWithFormat(const char* format, ...);
        void LogFeature(const char* feature, const char* format, ...);
        void LogVariableFeature(const char* feature, const unsigned varPrefix, const char* format, ...);
        void Log(const char* message);
        void EnableFeature(const char* featureRegex);
        void DisableFeature(const char* featureRegex);
    protected:
        char buffer[1024];
        void LogVAList(const char* format, va_list args);
        std::map<const char*, std::regex> regexFeaturesEnabled;
        bool IsRegexMatchesThenLog(const char* format, ...);
        void VAListToBuffer(const char* format, va_list args);
    private:
        std::ostream *out;
};

#endif // LOGGER_H_INCLUDED
