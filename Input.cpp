#include "Input.h"

int strobe = 0;

Input::Input()
{
    memset(input, 0, sizeof(input));
}

void Input::latchButtonStatus()
{
    for (int i = 0; i < 2; i++) {
        shftRegs[i] = 0;
        for (int j = 0; j < 8; j++) {
            shftRegs[i] |= (input[i][j] << j);

            // Don't allow left + right, up + down
            if ((shftRegs[i] & 0x30) == 0x30) {
                shftRegs[i] &= ~0x30;
            }

            if ((shftRegs[i] & 0xC0) == 0xC0) {
                shftRegs[i] &= ~0xC0;
            }
        }
    }
}

unsigned char Input::read(int addr)
{

    unsigned char shftRegBit = shftRegs[addr & 1] & 1;
    if (strobe)
        latchButtonStatus();
    else
        shftRegs[addr & 1] >>= 1;
    return (shftRegBit | cpuIO->dataBus);
}

void Input::write(int addr, unsigned char val)
{
    // Detect the controller strobe sequence
    strobe = val & 0x1;

    if (strobe) {
        latchButtonStatus();
    }
}

void Input::updatePlayersInput(bool (*input)[8])
{
    for (int player = 0; player < 2; player++) {
        for (int button = 0; button < 8; button++) {
            this->input[player][button] = input[player][button];
        }
    }
}
