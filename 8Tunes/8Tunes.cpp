#include <iostream>

#include "NSFLoader.h"
#include "../CPU.h"
#include "DummyPPU.h"
#include <unistd.h>

using namespace std;

int main()
{
    //NSFLoader loader("Donkey Kong.nsf");
    //NSFLoader loader("Super Mario Bros. (NTSC) (SFX).nsf");
    //NSFLoader loader("Metroid.nsf");
    NSFLoader loader("Battletoads.nsf");
    //NSFLoader loader("Jurassic Park (NTSC) (SFX).nsf");
    if(!loader.mapper)
        return -1;

    CPU cpu(*loader.mapper);
    PPU ppu;
    cpu.setPPUPtr(&ppu);
    loader.initializingATune(cpu, 4); //Battletoads 4, Jurassic 3, SMB1 8
    loader.play(cpu);

    return 0;
}
