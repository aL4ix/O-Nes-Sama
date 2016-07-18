#include "Input.h"

Input::Input(){}

void Input::latchButtonStatus(){
    const uint8_t* state = SDL_GetKeyboardState(NULL);
    shftRegs[0] = 0;
    shftRegs[1] = 0;
    for (int i=0; i<8; i++){
        if ((shftRegs[0] & _Up_Dn) == _Up_Dn)     shftRegs[0] &= ~ _Up; //Don't allow Up + Down
        if ((shftRegs[0] & _Lft_Rgt) == _Lft_Rgt) shftRegs[0] &= ~ _Lft; //Don't allow Left + Right;
        if ((shftRegs[1] & _Up_Dn) == _Up_Dn)     shftRegs[1] &= ~ _Up;
        if ((shftRegs[1] & _Lft_Rgt) == _Lft_Rgt) shftRegs[1] &= ~ _Lft;
        shftRegs[0] = shftRegs[0] | (state[P1Buttons[i]] << i);
        shftRegs[1] = shftRegs[1] | (state[P2Buttons[i]] << i);
    }
}

unsigned char Input::read(int addr){
    int shftRegBit = 0;
    shftRegBit =  shftRegs[addr & 1] & 0x1;
    shftRegs[addr & 1] >>= 1;
    return (shftRegBit | ((addr & 0xE000) >> 8));
}

void Input::write(int addr, unsigned char val){
    //Detect the controller strobe sequence
    strobeHigh = val & 0x1;
    if (strobeHigh)
        latchButtonStatus();
}
