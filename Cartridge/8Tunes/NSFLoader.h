#ifndef NSFLOADER_H_INCLUDED
#define NSFLOADER_H_INCLUDED

#include <cstdio>

#include "../../CPU.h"
#include "../../RetroEmu/RetroFraction.hpp"
#include "../ROMLoader.hpp"
#include "NSFMapper.hpp"
#include <string>

class NSFLoader : public ROMLoader {
public:
    NSFLoader(std::string fileName);

    bool loadRomFile();
    void initializingATune(CPU& cpu, unsigned short int song = 0);
    void saveState(FILE* file);
    bool loadState(FILE* file);
    void nfsDidARts(CPU& cpu);

    CartIO io;

private:
    std::string romFileName;
    char nsfVersion;
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
