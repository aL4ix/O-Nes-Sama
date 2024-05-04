#include "MMC1.hpp"

int isPRGLarge = 0;
int isCHRLarge = 0;
int superPRGBank = 0;
int chrSizeMask = 0;
int prgSizeMask = 0;
unsigned char control = 0xC;
unsigned char shiftReg = 0b10000;
unsigned char wRamBank = 0;
unsigned char prgBank = 0;
unsigned char chrBanks[2];
unsigned countM2, lastCountM2 = 0;

MMC1::MMC1(CartIO& ioRef)
    : BasicMapper(ioRef)
{
    io.wRam = new unsigned char[0x8000]; // 32K
    io.switch8K(0, 0, io.wRam, io.wRamSpace);
    prgSizeMask = io.iNESHeader.prgSize16k - 1;
    isPRGLarge = (io.iNESHeader.prgSize16k >= 32);
    isCHRLarge = (io.iNESHeader.chrSize8k >= 2);

    if (io.iNESHeader.chrSize8k > 0)
        chrSizeMask = (io.iNESHeader.chrSize8k << 1) - 1;
    else
        chrSizeMask = 1;
    sync();
}

void MMC1::writeCPU(int address, unsigned char val)
{
    switch (address >> 12) {
    case 0x6:
    case 0x7: /* WRAM Space */
    {
        io.wRamSpace[wRamBank][address & 0x1FFF] = val;
    } break;
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0xF: /* Mapper space */
        if ((countM2 - lastCountM2) > 1) { // If M2 count is less <= 1 ignore writes to mapper.
            if (val & 0x80) { // Reset the MMC1 on bit 7 set.
                shiftReg = 0x10;
                control |= 0xC;
            } else {
                if (shiftReg & 1) { // If shift register is full
                    int writeVal = (shiftReg >> 1) | ((val & 1) << 4); // Copy last bit and the SR contents to reg val
                    int regsel = (address >> 13) & 3; // Select register from bits 13 & 14 of the address
                    switch (regsel) {
                    case 0:
                        control = writeVal & 0x1F;
                        break;
                    case 1:
                        chrBanks[0] = writeVal & 0x1F;
                        break;
                    case 2:
                        chrBanks[1] = writeVal & 0x1F;
                        break;
                    case 3:
                        prgBank = writeVal & 0x1F;
                        break;
                    }
                    shiftReg = 0b10000; // Reset SR
                } else {
                    shiftReg = (shiftReg >> 1) | ((val & 1) << 4); // Push bit 1 into shift register
                }
            }
        }
        lastCountM2 = countM2;
        sync();

        break;
    }
}

void MMC1::clockCPU()
{
    countM2++;
}

void MMC1::setNTMirroring()
{
    switch (control & 0x3) {
    case 0: // One Screen, lower bank
        io.switchToSingleScrLo();
        return;
    case 1: // One Screen, upper bank
        io.switchToSingleScrHi();
        return;
    case 2: // Vertical
        io.switchVerMirroring();
        return;
    case 3: // Horizontal
        io.switchHorMirroring();
        return;
    }
}

void MMC1::sync()
{
    /* Sync Mirroring */
    setNTMirroring();

    if (isPRGLarge) { // Treat the HIGH CHR lines as SXROM behavior.
        superPRGBank = (chrBanks[0] >> 4);

        if ((control & 0x10) && ((chrBanks[0] & 0x10) != (chrBanks[1] & 0x10)))
            Log.debug(LogCategory::mapperMMC1, "PRG Supaa banku mismatches!!");

        prgSizeMask = 0xF; // 256 KB maximum PRG size (16Kb multiples)
    }

    if (!isCHRLarge) {
        // wRamMask = 0x7FFF;
        wRamBank = (chrBanks[0] & 0xC) >> 2;
        io.switch8K(0, wRamBank, io.wRam, io.wRamSpace);
        Log.debug(LogCategory::mapperMMC1, "Wram Bank: %d", wRamBank);
    }

    /* Sync CHR state... */
    if (control & 0x10) {

        /*Switch the 4K bank (val) into the $1000-$1FFF PPU space...*/
        io.switch4K(0, chrBanks[0] & chrSizeMask, io.chrBuffer, io.chrSpace);
        io.switch4K(1, chrBanks[1] & chrSizeMask, io.chrBuffer, io.chrSpace);

    } else {
        io.switch8K(0, (chrBanks[0] >> 1) & ((chrSizeMask >> 1) | 1), io.chrBuffer, io.chrSpace);
    }

    /* Sync PRG state... jeez what a PITA */
    switch ((control & 0xC) >> 2) {
    case 0:
    case 1:
        // No game I know switches between modes
        io.switch32K((prgBank & prgSizeMask) >> 1, io.prgBuffer, io.prgSpace, superPRGBank * 0x40000);
        break;
    case 2: // Fix first bank at $8000
        io.switch16K(0, 0, io.prgBuffer, io.prgSpace);
        io.switch16K(1, prgBank & prgSizeMask, io.prgBuffer, io.prgSpace, superPRGBank * 0x40000);
        break;
    case 3: // Fix last bank at $C000
        io.switch16K(0, prgBank & prgSizeMask, io.prgBuffer, io.prgSpace, superPRGBank * 0x40000);
        io.switch16K(1, prgSizeMask, io.prgBuffer, io.prgSpace, superPRGBank * 0x40000);
        break;
    }
}

void MMC1::saveSRAM(FILE* batteryFile)
{
    fwrite(io.wRam, 0x8000, 1, batteryFile);
}

void MMC1::loadSRAM(FILE* batteryFile)
{
    if (fread(io.wRam, 0x8000, 1, batteryFile)) { } // Suppressed warning
}
