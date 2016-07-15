#include "Input.h"

Input::Input(){
    strobeHigh = false;
    shftRegValP1 = 0;
    shftRegValP2 = 0;
}

void Input::latchButtonStatus(){
    const uint8_t* state = SDL_GetKeyboardState(NULL);
    /* Player one */
    shftRegValP1 = 0;
    shftRegValP1 = shftRegValP1 | (state[SDL_SCANCODE_S]);
    shftRegValP1 = shftRegValP1 | (state[SDL_SCANCODE_A] << 1);
    shftRegValP1 = shftRegValP1 | (state[SDL_SCANCODE_BACKSPACE] << 2);
    shftRegValP1 = shftRegValP1 | (state[SDL_SCANCODE_RETURN] << 3);
    shftRegValP1 = shftRegValP1 | (state[SDL_SCANCODE_UP] << 4);
    if(!state[SDL_SCANCODE_UP])
        shftRegValP1 = shftRegValP1 | (state[SDL_SCANCODE_DOWN] << 5);
    shftRegValP1 = shftRegValP1 | (state[SDL_SCANCODE_LEFT] << 6);
    if(!state[SDL_SCANCODE_LEFT])
        shftRegValP1 = shftRegValP1 | (state[SDL_SCANCODE_RIGHT] << 7);

    /* Player two */
    shftRegValP2 = 0;
    shftRegValP2 = shftRegValP2 | (state[SDL_SCANCODE_W]);
    shftRegValP2 = shftRegValP2 | (state[SDL_SCANCODE_Q] << 1);
    shftRegValP2 = shftRegValP2 | (state[SDL_SCANCODE_SPACE] << 2);
    shftRegValP2 = shftRegValP2 | (state[SDL_SCANCODE_L] << 3);
    shftRegValP2 = shftRegValP2 | (state[SDL_SCANCODE_U] << 4);
    shftRegValP2 = shftRegValP2 | (state[SDL_SCANCODE_J] << 5);
    shftRegValP2 = shftRegValP2 | (state[SDL_SCANCODE_H] << 6);
    shftRegValP2 = shftRegValP2 | (state[SDL_SCANCODE_K] << 7);
}

unsigned char Input::read(int addr){
    unsigned char shftRegBit = 0;

    if (addr == 0x4016){
        shftRegBit = shftRegValP1 & 0x1;
        shftRegValP1 >>= 1;
    }
    if (addr == 0x4017){
        shftRegBit = shftRegValP2 & 0x1;
        shftRegValP2 >>= 1;
    }
    return (shftRegBit | ((addr & 0xE000) >> 8));
}

void Input::write(int addr, unsigned char val){
    //Detect the controller strobe sequence
    strobeHigh = val & 0x1;
    if (strobeHigh)
        latchButtonStatus();
}
