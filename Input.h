#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <SDL2/SDL.h>

class Input{

    public:
        Input();
        unsigned char read(int addr);
        void write (int addr, unsigned char val);
        bool strobeHigh;
        void latchButtonStatus();
    private:
        const unsigned char _A       = 0x01;
        const unsigned char _B       = 0x02;
        const unsigned char _Sel     = 0x04;
        const unsigned char _Sta     = 0x08;
        const unsigned char _Up      = 0x10;
        const unsigned char _Dn      = 0x20;
        const unsigned char _Lft     = 0x40;
        const unsigned char _Rgt     = 0x80;
        const unsigned char _Up_Dn   = 0x30;
        const unsigned char _Lft_Rgt = 0xC0;
        //Initialize P1 & P2 Controllers, eventually this will be done from the GUI
        unsigned char Buttons[2][8] = {
            {  //Player 1 Buttons
                SDL_SCANCODE_S , SDL_SCANCODE_A   , SDL_SCANCODE_BACKSPACE, SDL_SCANCODE_RETURN,
                SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT     , SDL_SCANCODE_RIGHT
            },
            {  //Player 2 Buttons
                SDL_SCANCODE_W , SDL_SCANCODE_Q   , SDL_SCANCODE_SPACE    , SDL_SCANCODE_L,
                SDL_SCANCODE_U , SDL_SCANCODE_J   , SDL_SCANCODE_H        , SDL_SCANCODE_K
            }
        };

        unsigned char shftRegs[2];
};

#endif // INPUT_H_INCLUDED
