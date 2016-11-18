#ifndef MAPPER_HPP_INCLUDED
#define MAPPER_HPP_INCLUDED

#include "../Boards/BoardData.hpp"

/* General constants */
static const int _512k = 0x80000;
static const int _256k = 0x40000;
static const int _128k = 0x20000;
static const int _64k  = 0x10000;
static const int _32k  =  0x8000;
static const int _16k  =  0x4000;
static const int _8k   =  0x2000;
static const int _4k   =  0x1000;
static const int _2k   =  0x0800;
static const int _1k   =  0x0400;

class Mapper{
    public:
        Mapper(){
            /*prg = bData.cpuPrgSpace;
            chr = bData.ppuChrSpace;*/
        }
        virtual void write(int addr, unsigned char val) = 0;
        virtual unsigned char read(int addr) = 0;

    protected:
        /* Memory Pointers */
        unsigned char ** prg;
        unsigned char ** chr;
        //BoardData & bData;

        /* Memory Banking Functions */
        void switch32KPRG(int prgBank) {
            for (int x=0; x < 4; x++){
                switch8KPRG(x, (prgBank << 2) | x);
            }
        }
        inline void switch16KPRG(int cpuBank, int prgBank) {
            switch8KPRG(cpuBank << 1, prgBank << 1);
            switch8KPRG((cpuBank << 1) | 1, (prgBank << 1) | 1);
        }

        inline void switch8KPRG(int cpuBank, int prgBank){
            prg[cpuBank] = prgBuffer[prgBank << 13];
        }

        void switch8KCHR(int chrBank)  {
            for (int x=0; x < 8; x++){
                switch1KCHR(x, (chrBank << 3) | x);
            }
        }

        void switch4KCHR(int ppuBank, int chrBank){
            for (int x=0; x < 4; x++){
                switch1KCHR((ppuBank << 2) | x, (chrBank << 2) | x);
            }
        }

        inline void switch2KCHR(int ppuBank, int chrBank){
            switch1KCHR(ppuBank << 1, chrBank << 1);
            switch1KCHR((ppuBank << 1)|1, (chrBank << 1)|1);
        }

        inline void switch1KCHR(int ppuBank, int chrBank) {
            chr[ppuBank] = &bData.chrBuffer[chrBank << 10];
        }

        inline void switchToHorMirroring(){
            bData.ppuNTSpace[0] = bData.ppuNTSpace[1] = &bData.ntBuffer[0];
            bData.ppuNTSpace[2] = bData.ppuNTSpace[3] = &bData.ntBuffer[0x400];
        }

        inline void switchToVertMirroring(){
            bData.ppuNTSpace[0] = bData.ppuNTSpace[2] = &bData.ntBuffer[0];
            bData.ppuNTSpace[1] = bData.ppuNTSpace[3] = &bData.ntBuffer[0x400];
        }

        inline void switchToSingleScrLow(){
            bData.ppuNTSpace[0] = bData.ppuNTSpace[1] = bData.ppuNTSpace[2] = bData.ppuNTSpace[3] = &bData.ntBuffer[0];

            bData.ppuNTSpace[0] = bData.ppuNTSpace[1] = bData.ppuNTSpace[2] = bData.ppuNTSpace[3] = &bData.ntBuffer[0];
        }

        inline void switchToSingleScrHi(){
            bData.ppuNTSpace[0] = bData.ppuNTSpace[1] = bData.ppuNTSpace[2] = bData.ppuNTSpace[3] = &bData.ntBuffer[0x400];
        }
};

#endif // MAPPER_HPP_INCLUDED
