#include "MMC1.hpp"

unsigned char shiftReg = 0b10000;
int countM2, lastCountM2 = 0;
int isPRGLarge = 0;
int isCHRLarge = 0;
int superPRGBank = 0;

MMC1::MMC1(unsigned char * header) : Board(header){}

void MMC1::init(){
    Board::init();
    ntBuffer = new unsigned char [MapperUtils::_2K];
    wramBuffer = new unsigned char [MapperUtils::_32K];
    prg = &cpuCartSpace[4];
    control = 0xC;
    prgSizeMask = prgSize16K - 1;

    if (chrSize8K > 0)
        chrSizeMask = (chrSize8K << 1) - 1;
    else
        chrSizeMask = 1;

    isPRGLarge = (prgSize16K >= 32);
    isCHRLarge =  (chrSize8K >= 2);

    printf ("\nPRG Large: %d", isPRGLarge);
    printf ("\nCHR Large: %d", isCHRLarge);

    sync();
}

unsigned char MMC1::read(int addr){
    switch (addr >> 12){
        case 0x6: case 0x7: /* WRAM Space */
            return wramBuffer[addr & 0x1FFF];
        case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            return prg[(addr & 0x7FFF) >> 12][addr & 0xFFF];
    }
    return -1;
}

void MMC1::write(int addr, unsigned char val){

    switch (addr >> 12){
        case 0x6: case 0x7: /* WRAM Space */
            wramBuffer[addr & 0x1FFF] = val;
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
                        int regsel = (addr >> 13) & 3; //Select register from bits 13 & 14 of the address
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

void MMC1::setNTMirroring(){
    switch (control & 0x3){
        case 0: //One Screen, lower bank
            MapperUtils::switchToSingleScrLow(ntBuffer, ppuNTSpace);
            return;
        case 1: //One Screen, upper bank
            MapperUtils::switchToSingleScrHi(ntBuffer, ppuNTSpace);
            return;
        case 2: //Vertical
            MapperUtils::switchToVertMirroring(ntBuffer, ppuNTSpace);
            return;
        case 3: //Horizontal
            MapperUtils::switchToHorMirroring(ntBuffer, ppuNTSpace);
            return;
    }
}

void MMC1::clockCPU(){
    countM2++;
}

void MMC1::sync(){
    /* Sync Mirroring */
    setNTMirroring();


    /* Sync CHR state... */


    superPRGBank = (chrBanks[0] >> 4);
    if (isPRGLarge){ //Treat the HIGH CHR lines as SXROM behaviour.

        //printf ("\nPRG Supaa banku: %x, %x, %x", superPRGBank, (control & 0xC) >> 2, control & 0x10);
        MapperUtils::switch256KOuterBank(superPRGBank);
        if ((control & 0x10) && ((chrBanks[0] & 0x10) != (chrBanks[1] & 0x10)))
            printf ("\nPRG Supaa banku mismatches!!");

        prgSizeMask = 0xF; //256 KB maximum PRG size (16Kb multiples)
    }

    if (!isCHRLarge){
      //Put "small CHR" behavior here
    }



    if(control & 0x10){
        /*Switch the 4K bank (val) into the $1000-$1FFF PPU space...*/
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 0, chrBanks[0] & chrSizeMask);
        MapperUtils::switchCHR4K(chrBuffer, ppuChrSpace, 1, chrBanks[1] & chrSizeMask);

    } else {
        MapperUtils::switchCHR8K(chrBuffer, ppuChrSpace, (chrBanks[0] >> 1) & ((chrSizeMask >> 1) | 1));
    }


    /* Sync PRG state... jeez what a PITA */
    switch((control & 0xC) >> 2){
        case 0: case 1:
            // No game I know switches between mode
            MapperUtils::switchPRG32K(prgBuffer, prg, (prgBank & prgSizeMask) >> 1);
            break;
        case 2: //Fix first bank at $8000
            MapperUtils::switchPRG16K(prgBuffer, prg, 0, 0);
            MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgBank & prgSizeMask);
            break;
        case 3: //Fix last bank at $C000
            MapperUtils::switchPRG16K(prgBuffer, prg, 0, prgBank & prgSizeMask);
            MapperUtils::switchPRG16K(prgBuffer, prg, 1, prgSizeMask);
            break;
    }
}

void MMC1::saveSRAM(FILE * batteryFile) {
    MapperUtils::saveSRAM(wramBuffer, batteryFile);
}

void MMC1::loadSRAM(FILE * batteryFile) {
    MapperUtils::loadSRAM(wramBuffer, batteryFile);
}

bool MMC1::loadState(FILE * file){
    int ret = Board::loadState(file);
    control     = tempR[0];
    chrBanks[0] = tempR[1];
    chrBanks[1] = tempR[2];
    prgBank     = tempR[3];
    shiftReg    = tempR[4];
    return ret;
}

void MMC1::saveState(FILE * file){
    tempR[0] = control;
    tempR[1] = chrBanks[0];
    tempR[2] = chrBanks[1];
    tempR[3] = prgBank;
    tempR[4] = shiftReg;
    Board::saveState(file);
}

MMC1::~MMC1(){}
