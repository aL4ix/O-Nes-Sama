#ifndef RETRO_INPUT_HPP_INCLUDED
#define RETRO_INPUT_HPP_INCLUDED

#include "../Logging/Logger.hpp"
#include "CPUIO.hpp"
#include <SDL2/SDL.h>

class RetroInput {
public:
    RetroInput();
    ~RetroInput();
    bool (*updateAndGetInputs())[8];

private:
    // Initialize P1 & P2 Controllers, eventually this will be done from the GUI
    /*
    documenting buttons[8] = {
        A, B, SELECT, START,
        UP, DOWN, LEFT, RIGHT
    }
    */
    unsigned char buttons[2][8] = {
        { // Player 1 Buttons
            SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_BACKSPACE, SDL_SCANCODE_RETURN,
            SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT },
        { // Player 2 Buttons
            SDL_SCANCODE_W, SDL_SCANCODE_Q, SDL_SCANCODE_SPACE, SDL_SCANCODE_L,
            SDL_SCANCODE_U, SDL_SCANCODE_J, SDL_SCANCODE_H, SDL_SCANCODE_K }
    };

    int joyButtonsAndAxis[2][8] = {
        {
            1, 0, 6, 7,
            (1 << 1) | 1, (1 << 1) | 0, (0 << 1) | 1, (0 << 1) | 0, // (axisNum<<1)|axisIsNegative
        },
        {
            1, 0, 6, 7,
            (1 << 1) | 1, (1 << 1) | 0, (0 << 1) | 1, (0 << 1) | 0, // Comment to avoid clang-format
        }
    };
    int numberOfJoys;
    uint8_t joyState[2][8];
    SDL_Joystick* joy[2];
    bool playerInput[2][8];

    void updateJoystickState();
};

#endif // RETRO_INPUT_HPP_INCLUDED
