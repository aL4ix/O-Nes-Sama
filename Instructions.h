#ifndef INSTRUCTIONS_H_INCLUDED
#define INSTRUCTIONS_H_INCLUDED

Registers regs;
ProcessorFlags flags;
enum addrMode  { IMP, IMM, ACC, ZP, ZPX, ZPY, ABS, ABSX, ABSY, INDX, INDY, IND, REL };
typedef void          (*genFn)(int);
typedef void          (*rdFn) (int);
typedef void          (*wrFn) (int);
typedef unsigned char (*rwmFn)(int);

/*********************************/
/*********************************/

char opAddrModes [0x100] = {
    /*      0x0   0x1   0x2   0x3   0x4   0x5   0x6   0x7   0x8   0x9   0xA   0xB   0xC   0xD   0xE   0xF*/
    /*0x0*/ IMP,  INDX, -1,   INDX, ZP,   ZP,   ZP,   ZP,   IMP,  IMM,  ACC,  -1,   ABS,  ABS,  ABS,  ABS,
    /*0x1*/ REL,  INDY, -1,   INDY, ZPX,  ZPX,  ZPX,  ZPX,  IMP,  ABSY, IMP,  ABSY, ABSX, ABSX, ABSX, ABSX,
    /*0x2*/ IMP,  INDX, -1,   INDX, ZP,   ZP,   ZP,   ZP,   IMP,  IMM,  ACC,  -1,   ABS,  ABS,  ABS,  ABS,
    /*0x3*/ REL,  INDY, -1,   INDY, ZPX,  ZPX,  ZPX,  ZPX,  IMP,  ABSY, IMP,  ABSY, ABSX, ABSX, ABSX, ABSX,
    /*0x4*/ IMP,  INDX, -1,   INDX, ZP,   ZP,   ZP,   ZP,   IMP,  IMM,  ACC,  -1,   IMP,  ABS,  ABS,  ABS,
    /*0x5*/ REL,  INDY, -1,   INDY, ZPX,  ZPX,  ZPX,  ZPX,  IMP,   ABSY, IMP,  ABSY, ABSX, ABSX, ABSX, ABSX,
    /*0x6*/ IMP,  INDX, -1,   INDX, ZP,   ZP,   ZP,   ZP,   IMP,  IMM,  ACC,  -1,   IMP,  ABS,  ABS,  ABS,
    /*0x7*/ REL,  INDY, -1,   INDY, ZPX,  ZPX,  ZPX,  ZPX,  IMP,  ABSY, IMP,  ABSY, ABSX, ABSX, ABSX, ABSX,
    /*0x8*/ IMM,  INDX, IMM,  INDX, ZP,   ZP,   ZP,   ZP,   IMP,  IMM,  IMP,  -1,   ABS,  ABS,  ABS,  ABS,
    /*0x9*/ REL,  INDY, -1,   -1,   ZPX,  ZPX,  ZPY,  ZPY,  IMP,  ABSY, IMP,  -1,   -1,   ABSX, -1,  -1,
    /*0xA*/ IMM,  INDX, IMM,  INDX, ZP,   ZP,   ZP,   ZP,   IMP,  IMM,  IMP,  -1,   ABS,  ABS,  ABS,  ABS,
    /*0xB*/ REL,  INDY, -1,   INDY, ZPX,  ZPX,  ZPY,  ZPY,  IMP,  ABSY,  IMP, -1,   ABSX, ABSX, ABSY, ABSY,
    /*0xC*/ IMM,  INDX, IMM,  INDX, ZP,   ZP,   ZP,   ZP,   IMP,  IMM,  IMP,  -1,   ABS,  ABS,  ABS,  ABS,
    /*0xD*/ REL,  INDY, -1,   INDY, ZPX,  ZPX,  ZPX,  ZPX,  IMP,  ABSY, IMP,  ABSY, ABSX, ABSX, ABSX, ABSX,
    /*0xE*/ IMM,  INDX, IMM,  INDX, ZP,   ZP,   ZP,   ZP,   IMP,  IMM,  IMP,  IMM,  ABS,  ABS,  ABS,  ABS,
    /*0xF*/ REL,  INDY, -1,   INDY, ZPX,  ZPX,  ZPX,  ZPX,  IMP,  ABSY, IMP,  ABSY, ABSX, ABSX, ABSX, ABSX,
    };

char opAccModes[0x100] = {
          /*0x0  0x1  0x2   0x3   0x4  0x5  0x6  0x7  0x8   0x9  0xA  0xB   0xC   0xD  0xE   0xF*/
    /*0x0*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, RMW, -1  , RDB,  RDB, RMW,  RMW,
    /*0x1*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, GEN, RMW,  RDB,  RDB, RMW,  RMW,
    /*0x2*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, RMW, -1  , RDB,  RDB, RMW,  RMW,
    /*0x3*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, GEN, RMW,  RDB,  RDB, RMW,  RMW,
    /*0x4*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, RMW, -1  , GEN,  RDB, RMW,  RMW,
    /*0x5*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, GEN, RMW,  RDB,  RDB, RMW,  RMW,
    /*0x6*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, RMW, -1  , GEN,  RDB, RMW,  RMW,
    /*0x7*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, GEN, RMW,  RDB,  RDB, RMW,  RMW,
    /*0x8*/ RDB, WR,  RDB,  WR,   WR,  WR,  WR,  WR,  GEN,  RDB, GEN, -1  , WR,   WR,  WR,   WR,
    /*0x9*/ GEN, WR,  -1  , -1  , WR,  WR,  WR,  WR,  GEN,  WR,  GEN, -1  , -1  , WR,  -1  , -1  ,
    /*0xA*/ RDB, RDB, RDB,  RDB,  RDB, RDB, RDB, RDB, GEN,  RDB, GEN, -1  , RDB,  RDB, RDB,  RDB,
    /*0xB*/ GEN, RDB, -1  , RDB,  RDB, RDB, RDB, RDB, GEN,  RDB, GEN, -1  , RDB,  RDB, RDB,  RDB,
    /*0xC*/ RDB, RDB, RDB,  RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, GEN, -1  , RDB,  RDB, RMW,  RMW,
    /*0xD*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, GEN, RMW,  RDB,  RDB, RMW,  RMW,
    /*0xE*/ RDB, RDB, RDB,  RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, GEN, RDB,  RDB,  RDB, RMW,  RMW,
    /*0xF*/ GEN, RDB, -1  , RMW,  RDB, RDB, RMW, RMW, GEN,  RDB, GEN, RMW,  RDB,  RDB, RMW,  RMW
    };

/* Common routines */
/**********************************************/
/*   Actual 6502 Functions                    */
/**********************************************/
/* Fetch instructions, based on opcode */
void executeInstruction(unsigned char opcode){

}

/* Setting ZERO and NEGATIVE flags */
void setZN(unsigned char reg){
    regs.p &= ~(flags.N_FLAG | flags.Z_FLAG);
    regs.p |= ((reg & flags.N_FLAG) | ((reg == 0) << 1));
}
/* Common routine for loading value into general purpose registers or accumulator */
void LDReg(unsigned char &reg, unsigned char val){
	reg = val;
	setZN(reg);
}
/* Common routine for comparing value to general purpose registers or accumulator */
void CMPReg(unsigned char reg, unsigned char val){
    setZN((reg - val) & 0xFF);
    regs.p &= ~flags.C_FLAG;
    regs.p |= (((reg - val) >> 8) & flags.C_FLAG) ^ flags.C_FLAG;
}

/********************************/
/*  Read Functions              */
/********************************/

/*LDA: Load into accumulator*/
void LDA (unsigned char val){
    LDReg(regs.a, val);
    setZN(regs.a);
}

/*LDX: Load into register X*/
void LDX (unsigned char val){
    LDReg(regs.x, val);
    setZN(regs.x);
}

/*LDY: Load into register Y*/
void LDY (unsigned char val){
    LDReg(regs.y, val);
    setZN(regs.y);
}

/*CMP: Compare to Accumulator*/
void CMP(unsigned char val){ CMPReg(regs.a, val); }

/*CMP: Compare to Reg X*/
void CPX(unsigned char val){ CMPReg(regs.x, val); }

/*CMP: Compare to Reg Y*/
void CPY(unsigned char val){ CMPReg(regs.y, val); }

/*BIT: Check bit and change flags*/
void BIT(unsigned char val){
    regs.p &= ~(flags.V_FLAG|flags.N_FLAG|flags.Z_FLAG);
    setZN(regs.a & val);
    regs.p |= val & (flags.V_FLAG|flags.N_FLAG);
}

#endif // INSTRUCTIONS_H_INCLUDED
