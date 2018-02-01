#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <SDL2/SDL.h>
#include "CPUIO.hpp"

class Input{

    public:
        Input();
        ~Input();
        CPUIO * cpuIO;
        unsigned char read(int addr);
        void write (int addr, unsigned char val);
        void latchButtonStatus();
        void updateJoystickState();
    private:
        const unsigned char _A       = 0x01;
        const unsigned char _B       = 0x02;
        const unsigned char _Sel     = 0x04;
        const unsigned char _Sta     = 0x08;
        const unsigned char _Up      = 0x10;
        const unsigned char _Dn      = 0x20;
        const unsigned char _Lft     = 0x40;
        const unsigned char _Rgt     = 0x80;

        //Initialize P1 & P2 Controllers, eventually this will be done from the GUI
        /*
        documenting buttons[8] = {
            A, B, SELECT, START,
            UP, DOWN, LEFT, RIGHT
        }
        */
        unsigned char buttons[2][8] = {
            {  //Player 1 Buttons
                SDL_SCANCODE_S , SDL_SCANCODE_A   , SDL_SCANCODE_BACKSPACE, SDL_SCANCODE_RETURN,
                SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT     , SDL_SCANCODE_RIGHT
            },
            {  //Player 2 Buttons
                SDL_SCANCODE_W , SDL_SCANCODE_Q   , SDL_SCANCODE_SPACE    , SDL_SCANCODE_L,
                SDL_SCANCODE_U , SDL_SCANCODE_J   , SDL_SCANCODE_H        , SDL_SCANCODE_K
            }
        };

        int joyButtonsAndAxis[2][8] = {
            {
                1, 0, 6, 7,
                (1<<1)|1, (1<<1)|0, (0<<1)|1, (0<<1)|0, // (axisNum<<1)|axisIsNegative
            },
            {
                1, 0, 6, 7,
                (1<<1)|1, (1<<1)|0, (0<<1)|1, (0<<1)|0,
            }
        };
        int numberOfJoys;
        uint8_t joyState[2][8];
        SDL_Joystick* joy[2];

        int shftRegs[2];
};

#endif // INPUT_H_INCLUDED

