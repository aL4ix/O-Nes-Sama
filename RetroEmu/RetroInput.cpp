#include "RetroInput.hpp"

RetroInput::RetroInput()
{
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
        printf("RetroInput SDL Joystick could not initialize: %s\n", SDL_GetError());
        return;
    }
    numberOfJoys = SDL_NumJoysticks();
    if (numberOfJoys < 0) {
        printf("RetroInput Number of joysticks is less than 0: %s\n", SDL_GetError());
        numberOfJoys = 0;
    } else if (numberOfJoys > 2) {
        numberOfJoys = 2;
    }
    SDL_JoystickEventState(SDL_IGNORE);

    for (int i = 0; i < numberOfJoys; i++) {
        joy[i] = SDL_JoystickOpen(i);
        SDL_JoystickGetAttached(joy[i]);
    }
    for (int i = numberOfJoys; i < 2; i++) {
        joy[i] = NULL;
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            joyState[i][j] = 0;
            playerInput[i][j] = false;
        }
    }
}

RetroInput::~RetroInput()
{
    for (int i = 0; i < numberOfJoys; i++) {
        if (SDL_JoystickGetAttached(joy[i])) {
            SDL_JoystickClose(joy[i]);
            joy[i] = NULL;
        }
    }
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

bool (*RetroInput::updateAndGetInputs())[8]
{
    SDL_PumpEvents();
    updateJoystickState();

    const uint8_t* state = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            playerInput[i][j] = state[buttons[i][j]];
            playerInput[i][j] |= joyState[i][j];
        }
    }
    return playerInput;
}

void RetroInput::updateJoystickState()
{
    SDL_JoystickUpdate();
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            if (j >= 4) {
                int axisNum = joyButtonsAndAxis[i][j] >> 1;
                bool negative = joyButtonsAndAxis[i][j] & 0x1;
                int ret = SDL_JoystickGetAxis(joy[i], axisNum);
                if (negative) {
                    ret *= -1;
                }
                joyState[i][j] = (ret > 12000);
            } else {
                joyState[i][j] = SDL_JoystickGetButton(joy[i], joyButtonsAndAxis[i][j]);
            }
        }
    }
}
