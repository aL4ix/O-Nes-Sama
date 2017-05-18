#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <stdexcept>

/*TODO: Support switching between std out and file or both at the same time*/

class Logger {
    public:
        /*Logger signature for CPU logging*/
        Logger();
        Logger(const char * fileName);
        ~Logger();
        void Init(const char * fileName = NULL);
        void LogWithFormat(const char* format, ...);
        void LogWithPrefix(const char* feature, const char* format, ...);
        void LogWithVarPrefix(const char* feature, const unsigned varPrefix, const char* format, ...);
        void Log(const char* message);
        void EnableFeature(const char* feature);
        void DisableFeature(const char* feature);
    protected:
        char buffer[1024];
        void LogVarArgs(const char* format, va_list args);
    private:
        std::ostream *out;
};

#endif // LOGGER_H_INCLUDED
