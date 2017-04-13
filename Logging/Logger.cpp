#include "Logger.h"

Logger::Logger() : out(NULL)
{

}

Logger::Logger(const char * fileName)
{
    Init(fileName);
}

Logger::~Logger(){
    if(out)
        out->flush();
}

void Logger::Init(const char * fileName)
{
    /*If no file name specified, then log to the standard output.*/
    if (fileName != NULL)
        out = new std::ofstream(fileName/*, std::ios_base::ate|std::ios_base::app*/);
    else
        out = &std::cout;
}

void Logger::Log(const char* message)
{
    if(out)
    {
        *out << message;
    }
}

void Logger::LogVarArgs(const char* format, va_list args)
{
    vsnprintf(buffer, sizeof(buffer), format, args);
    Log(buffer);
}

void Logger::LogWithPrefix(const char* feature, const char* format, ...)
{
    LogWithFormat("%s: ", feature);
    va_list args;
    va_start(args, format);
    LogVarArgs(format, args);
    va_end(args);
}

void Logger::LogWithFormat(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogVarArgs(format, args);
    va_end(args);
}
