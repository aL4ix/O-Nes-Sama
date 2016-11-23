#include "Input.h"

Input::Input(){
    shftRegs[0] = 0;
    shftRegs[1] = 0;
}

void Input::latchButtonStatus(){
    const uint8_t* state = SDL_GetKeyboardState(NULL);

    for (int j=0; j<2; j++){
        for (int i=7; i>=0; i--){
            shftRegs[j] <<= 1;
            shftRegs[j] = shftRegs[j] &~ state[buttons[j][i]];
            shftRegs[j] = shftRegs[j] | (state[buttons[j][i]]);
            if ((shftRegs[j] & _Up_Dn) == _Up_Dn)     shftRegs[j] &= ~ _Up; //Don't allow Up + Down
            if ((shftRegs[j] & _Lft_Rgt) == _Lft_Rgt) shftRegs[j] &= ~ _Lft; //Don't allow Left + Right;
        }
    }
}

unsigned char Input::read(int addr){
    int shftRegBit =  shftRegs[addr & 1] & 0x1;
    shftRegs[addr & 1] >>= 1;
    return (shftRegBit | ((addr & 0xE000) >> 8));
}

void Input::write(int addr, unsigned char val){
    //Detect the controller strobe sequence
    strobeHigh = val & 0x1;
    if (strobeHigh){
        latchButtonStatus();
    }
}
