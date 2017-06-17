#include "MMC1.hpp"


MMC1::MMC1(CartIO &ioRef) : BasicMapper(ioRef){
    isPRGLarge = 0;
    isCHRLarge = 0;
    superPRGBank = 0;
    chrSizeMask = 0;
    prgSizeMask = 0;
    control = 0xC;
    shiftReg = 0b10000;
    wRamBank = 0;
    prgBank = 0;
    chrBanks[0] = 0;
    chrBanks[1] = 0;
    countM2 = 0;
    lastCountM2 = 0;

    io.wRam = new unsigned char [0x8000]; //32K
    io.swapPRGRAM(0, 1);
    prgSizeMask = io.iNESHeader.prgSize16k - 1;
    isPRGLarge = (io.iNESHeader.prgSize16k >= 32);
    isCHRLarge =  (io.iNESHeader.chrSize8k >= 2);

    if (io.iNESHeader.chrSize8k > 0)
        chrSizeMask = (io.iNESHeader.chrSize8k << 1) - 1;
    else
        chrSizeMask = 1;
    sync();
}

void MMC1::writeCPU(int address, unsigned char val){
    switch (address >> 12){
        case 0x6: case 0x7: /* WRAM Space */
            {
                io.wRamSpace[address & 0x1FFF] = val;
            }
            break;
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF: /* Mapper space */
            if ((countM2 - lastCountM2) > 1){ //If M2 count is less <= 1 ignore writes to mapper.
                if (val & 0x80){ //Reset the MMC1 on bit 7 set.
                    shiftReg = 0x10;
                    control |= 0xC;
                }
                else {
                    if (shiftReg & 1){ //If shift register is full
                        int writeVal = (shiftReg >> 1) | ((val & 1) << 4); //Copy last bit and the SR contents to reg val
                        int regsel = (address >> 13) & 3; //Select register from bits 13 & 14 of the address
                        switch (regsel){
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
                        shiftReg = 0b10000; //Reset SR
                    }
                    else {
                        shiftReg = (shiftReg >> 1) | ((val & 1) << 4); //Push bit 1 into shift register
                    }
                }
            }
            lastCountM2 = countM2;
            sync();

            break;
    }
}

void MMC1::clockCPU(){
    countM2++;
}

void MMC1::setNTMirroring(){
    switch (control & 0x3){
        case 0: //One Screen, lower bank
            io.switchToSingleScrLo();
            return;
        case 1: //One Screen, upper bank
            io.switchToSingleScrHi();
            return;
        case 2: //Vertical
            io.switchVerMirroring();
            return;
        case 3: //Horizontal
            io.switchHorMirroring();
            return;
    }
}


void MMC1::sync(){
        /* Sync Mirroring */
    setNTMirroring();


    if (isPRGLarge){ //Treat the HIGH CHR lines as SXROM behavior.
        superPRGBank = (chrBanks[0] >> 4);

        if ((control & 0x10) && ((chrBanks[0] & 0x10) != (chrBanks[1] & 0x10)))
            printf ("\nPRG Supaa banku mismatches!!");

        prgSizeMask = 0xF; //256 KB maximum PRG size (16Kb multiples)
    }

    if (!isCHRLarge){
        //wRamMask = 0x7FFF;
        wRamBank = (chrBanks[0] & 0xC) >> 2;
        io.swapPRGRAM(wRamBank, 1);
        //printf("\nWram Bank: %d", wRam8KBank);
    }

    /* Sync CHR state... */
    if(control & 0x10){

        /*Switch the 4K bank (val) into the $1000-$1FFF PPU space...*/
        io.swapCHR(4, 0, chrBanks[0] & chrSizeMask, io.chrBuffer);
        io.swapCHR(4, 1, chrBanks[1] & chrSizeMask, io.chrBuffer);

    } else {
        io.swapCHR(8, 0, (chrBanks[0] >> 1) & ((chrSizeMask >> 1) | 1), io.chrBuffer);
    }

    /* Sync PRG state... jeez what a PITA */


    switch((control & 0xC) >> 2){
        case 0: case 1:
            // No game I know switches between modes
            io.swapPRGROM(32, 0, ((superPRGBank << 4) | (prgBank & prgSizeMask)) >> 1, io.prgBuffer, 0);
            break;
        case 2: //Fix first bank at $8000
            io.swapPRGROM(16, 0, 0, io.prgBuffer, 0);
            io.swapPRGROM(16, 1, ((superPRGBank << 4) | prgBank & prgSizeMask), io.prgBuffer, 0);
            break;
        case 3: //Fix last bank at $C000
            io.swapPRGROM(16, 0, ((superPRGBank << 4) | prgBank & prgSizeMask) , io.prgBuffer, 0);
            io.swapPRGROM(16, 1, (superPRGBank << 4) | prgSizeMask, io.prgBuffer, 0);
            break;
    }
}

void MMC1::saveSRAM(FILE * batteryFile){
    fwrite(io.wRam, 0x8000, 1, batteryFile);
}

void MMC1::loadSRAM(FILE * batteryFile){
    fread(io.wRam, 0x8000, 1, batteryFile);
}

void MMC1::loadState(FILE * file){

    fread(&isPRGLarge  , sizeof(int *), 1, file);
    fread(&isCHRLarge  , sizeof(int *), 1, file);
    fread(&superPRGBank, sizeof(int *), 1, file);
    fread(&control    , sizeof(unsigned char *), 1, file);
    fread(&shiftReg   , sizeof(unsigned char *), 1, file);
    fread(&wRamBank   , sizeof(unsigned char *), 1, file);
    fread(&prgBank    , sizeof(unsigned char *), 1, file);
    fread(chrBanks    , sizeof(unsigned char)  , 2, file);
    fread(&countM2    , sizeof(unsigned *)     , 1, file);
    fread(&lastCountM2, sizeof(unsigned *)     , 1, file);

    if (io.wRam)
        fread(io.wRam, sizeof (unsigned char), 0x2000, file);
    if (io.iNESHeader.chrSize8k == 0)
        fread(io.chrBuffer, sizeof (unsigned char), 0x2000, file);
    fread(io.ntSystemRam, sizeof (unsigned char), 0x800, file);
    sync();

}

void MMC1::saveState(FILE * file){

    fwrite(&isPRGLarge  , sizeof(int *), 1, file);
    fwrite(&isCHRLarge  , sizeof(int *), 1, file);
    fwrite(&superPRGBank, sizeof(int *), 1, file);
    fwrite(&control    , sizeof(unsigned char *), 1, file);
    fwrite(&shiftReg   , sizeof(unsigned char *), 1, file);
    fwrite(&wRamBank   , sizeof(unsigned char *), 1, file);
    fwrite(&prgBank    , sizeof(unsigned char *), 1, file);
    fwrite(chrBanks    , sizeof(unsigned char)  , 2, file);
    fwrite(&countM2    , sizeof(unsigned *)     , 1, file);
    fwrite(&lastCountM2, sizeof(unsigned *)     , 1, file);

    if (io.wRam)
        fwrite(io.wRam, sizeof (unsigned char), 0x2000, file);
    if (io.iNESHeader.chrSize8k == 0)
        fwrite(io.chrBuffer, sizeof (unsigned char), 0x2000, file);
    fwrite(io.ntSystemRam, sizeof (unsigned char), 0x800, file);

}
