#include "Input.h"

Input::Input(){}

void Input::latchButtonStatus(){
    const uint8_t* state = SDL_GetKeyboardState(NULL);

    for (int j = 0; j < 8; j++){
        shftRegs[0] |= (state[buttons[0][j]] << j);
        shftRegs[1] |= (state[buttons[1][j]] << j);
    }
}

unsigned char Input::read(int addr){

    unsigned char shftRegBit = shftRegs[addr & 1] & 1;

    if (strobeHigh){
        latchButtonStatus();
    } else {
        shftRegs[addr & 1] >>= 1;
    }

    return (shftRegBit | ((addr & 0xE000) >> 8));
}

void Input::write(int addr, unsigned char val){
    //Detect the controller strobe sequence
    strobeHigh = val & 0x1;
    if (strobeHigh){
        latchButtonStatus();
    }
}
