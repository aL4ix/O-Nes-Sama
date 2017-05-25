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

void Logger::LogVAList(const char* format, va_list args)
{
    VAListToBuffer(format, args);
    Log(buffer);
}

bool Logger::IsRegexMatchesThenLog(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    VAListToBuffer(format, args);
    va_end(args);

    for(std::map<const char*, std::regex>::iterator it=regexFeaturesEnabled.begin();
        it != regexFeaturesEnabled.end(); ++it)
    {
        if(std::regex_match(buffer, it->second))
        {
            Log(buffer);
            return true;
        }
    }
    return false;
}

void Logger::VAListToBuffer(const char* format, va_list args)
{
    vsnprintf(buffer, sizeof(buffer), format, args);
}

void Logger::LogFeature(const char* feature, const char* format, ...)
{
    if(IsRegexMatchesThenLog("%s: ", feature))
    {
        va_list args;
        va_start(args, format);
        LogVAList(format, args);
        va_end(args);
    }
}

void Logger::LogVariableFeature(const char* feature, const unsigned varPrefix, const char* format, ...)
{
    if(IsRegexMatchesThenLog("%s%d: ", feature, varPrefix))
    {
        va_list args;
        va_start(args, format);
        LogVAList(format, args);
        va_end(args);
    }
}

void Logger::LogWithFormat(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogVAList(format, args);
    va_end(args);
}

void Logger::EnableFeature(const char* featureRegex)
{
    LogWithFormat("Enabled Feature %s\n", featureRegex);
    regexFeaturesEnabled.insert(
        std::pair<const char*, std::regex>(featureRegex, std::regex(featureRegex)));
}

void Logger::DisableFeature(const char* featureRegex)
{
    LogWithFormat("Disabled Feature %s\n", featureRegex);
    regexFeaturesEnabled.erase(featureRegex);
}
