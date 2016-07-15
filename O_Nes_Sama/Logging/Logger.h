#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED
#include <fstream>
#include <stdexcept>

/*TODO: Support switching between std out and file or both at the same time*/

class Logger {
    public:
        std::ostream *out;
        /*Logger signature for CPU logging*/
        Logger(const char * fileName = NULL);
        ~Logger();
        template<typename T>
        /*Overloading the '<<' operator for logging convenience*/
        Logger & operator << (const T& object){
            if(!out)
                throw std::runtime_error("No stream set for Logger class");
            *out << object;
            out->flush();
            return *this;
        }
};

#endif // LOGGER_H_INCLUDED
