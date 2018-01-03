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
    //NSFLoader loader("Battletoads (NTSC - US) (PCM Drums) (Nintendulator + Foobar 1.3.8) (SFX).nsf");
    NSFLoader loader("Battletoads.nsf");
    //NSFLoader loader("battletoads_pcm.nsf");
    if(!loader.mapper)
        return -1;

    CPU cpu(*loader.mapper);
    PPU ppu;
    cpu.setPPUPtr(&ppu);
    loader.initializingATune(cpu, 4); //Battletoads 23
    loader.play(cpu);

    return 0;
}
