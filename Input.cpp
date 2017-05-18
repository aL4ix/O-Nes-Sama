#include "Input.h"
//#include <stdio.h>

int strobe = 0;

Input::Input(){ }

void Input::latchButtonStatus(){
    const uint8_t* state = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < 2; i++){
        shftRegs[i] = 0;
        for (int j = 0; j < 8; j++){
            shftRegs[i] |= (state[buttons[i][j]] << j);
            //Don't allow left + right, up + down
            if ((shftRegs[i] & 0x30) == 0x30){
                shftRegs[i] &= ~0x30;
            }

            if ((shftRegs[i] & 0xC0) == 0xC0) {
                shftRegs[i] &= ~0xC0;
            }
        }
    }
}

unsigned char Input::read(int addr){

    unsigned char shftRegBit = shftRegs[addr & 1] & 1;
    if (strobe)
        latchButtonStatus();
    else
        shftRegs[addr & 1] >>= 1;
    return (shftRegBit | cpuIO->dataBus);
}

void Input::write(int addr, unsigned char val){
    //Detect the controller strobe sequence
    strobe = val & 0x1;

    if (strobe){
        latchButtonStatus();
    }
}
