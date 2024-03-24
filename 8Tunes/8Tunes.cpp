#define SDL_MAIN_HANDLED

#include <iostream>

#include "../CPU.h"
#include "DummyPPU.h"
#include "NSFLoader.h"
#include <unistd.h>

using namespace std;

int main()
{
    // NSFLoader loader("music.nsf");
    if (!loader.mapper)
        return -1;

    CPU cpu(*loader.mapper);
    PPU ppu;
    cpu.setPPUPtr(&ppu);
    loader.initializingATune(cpu, 5); // song number
    loader.play(cpu);

    return 0;
}
