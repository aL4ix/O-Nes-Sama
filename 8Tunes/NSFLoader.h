#ifndef NSFLOADER_H_INCLUDED
#define NSFLOADER_H_INCLUDED

#include <cstdio>

#include "NSFMapper.hpp"
#include "../CPU.h"

class NSFLoader
{
public:
    NSFLoader(const char* fileName);
    ~NSFLoader();

    void loadRomFile();
    void initializingATune(CPU& cpu);
    void play(CPU& cpu);

    NSFMapper* mapper;
    CartIO io;

private:
    const char* romFileName;
    unsigned char totalSongs;
    unsigned char startingSong;
    unsigned short loadAddress;
    unsigned short initAddress;
    unsigned short playAddress;
    char nameSong[33];
    char artist[33];
    char copyright[33];
    unsigned short playSpeedNTSC;
    unsigned char bankswitch[8];
    unsigned short playSpeedPAL;
    unsigned char palOrNtsc;
    unsigned char extraChips;
    unsigned char expansion[4];
    long int fileSize;
    long int prgSize;
    unsigned char prg[8][0x1000];
    bool isBankswitched;

};

#endif // NSFLOADER_H_INCLUDED
