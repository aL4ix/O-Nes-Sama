#include "Logger.h"
#include <iostream>

Logger::Logger(const char * fileName){
    /*If no file name specified, then log to the standard output.*/
    if (fileName != NULL)
        out = new std::ofstream(fileName/*, std::ios_base::ate|std::ios_base::app*/);
    else
        out = &std::cout;
}

Logger::~Logger(){
    out->flush();
}
