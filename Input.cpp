#include "Input.h"
//#include <stdio.h>

int strobe = 0;

Input::Input() {
    if(SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
		printf("SDL Joystick could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
	}
    numberOfJoys = SDL_NumJoysticks();
    if(numberOfJoys < 0) {
        printf("%s\n", SDL_GetError());
        numberOfJoys = 0;
    } else if(numberOfJoys > 2) {
        numberOfJoys = 2;
    }
    SDL_JoystickEventState(SDL_IGNORE);

    for(int i=0; i<numberOfJoys; i++) {
        joy[i] = SDL_JoystickOpen(i);
        SDL_JoystickGetAttached(joy[i]);
    }
    for(int i=numberOfJoys; i<2; i++) {
        joy[i] = NULL;
    }
    for(int i=0; i<2; i++) {
        for(int j=0; j<8; j++) {
            joyState[i][j] = 0;
        }
    }
}

Input::~Input() {
    for(int i=0; i<numberOfJoys; i++) {
        if(SDL_JoystickGetAttached(joy[i])) {
            SDL_JoystickClose(joy[i]);
            joy[i] = NULL;
        }
    }
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}


void Input::latchButtonStatus(){
    const uint8_t* state = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < 2; i++){
        shftRegs[i] = 0;
        for (int j = 0; j < 8; j++){
            shftRegs[i] |= (state[buttons[i][j]] << j);
            shftRegs[i] |= (joyState[i][j] << j);

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

void Input::updateJoystickState() {
    SDL_JoystickUpdate();
    for(int i=0; i<2; i++)
        for(int j=0; j<8; j++){
            if(j>=4) {
                int axisNum = joyButtonsAndAxis[i][j] >> 1;
                bool negative = joyButtonsAndAxis[i][j] & 0x1;
                int ret = SDL_JoystickGetAxis(joy[i], axisNum);
                if(negative) {
                    ret *= -1;
                }
                joyState[i][j] = (ret > 12000);
            } else {
                joyState[i][j] = SDL_JoystickGetButton(joy[i], joyButtonsAndAxis[i][j]);
            }
        }
}

