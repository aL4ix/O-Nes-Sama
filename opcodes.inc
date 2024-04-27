switch (instData.opcode) {
case 0x00: // BRK
    BRK();
    break;
case 0x01: // ORA INDX
    addr = getAddress(INDX);
    rd(&CPU::ORA, addr);
    break;
case 0x03: //*SLO INDX
    addr = getAddress(INDX);
    rmw(&CPU::SLO, addr);
    break;
case 0x04: //*NOP ZP
    addr = getAddress(ZP);
    rd(&CPU::NOP, addr);
    break;
case 0x05: // ORA ZP
    addr = getAddress(ZP);
    rd(&CPU::ORA, addr);
    break;
case 0x06: // ASL ZP
    addr = getAddress(ZP);
    rmw(&CPU::ASL, addr);
    break;
case 0x07: // SLO ZP
    addr = getAddress(ZP);
    rmw(&CPU::SLO, addr);
    break;
case 0x08: // PHP
    read(regs.pc);
    PHP();
    break;
case 0x09: // ORA IMM
    addr = getAddress(IMM);
    rd(&CPU::ORA, addr);
    break;
case 0x0A: // ASL ACC
    pollForInterrupts();
    read(regs.pc);
    ASL(regs.a);
    break;
case 0x0B: //*AAC IMM
    addr = getAddress(IMM);
    rd(&CPU::AAC, addr);
    break;
case 0x0C: //*NOP ABS
    addr = getAddress(ABS);
    rd(&CPU::NOP, addr);
    break;
case 0x0D: // ORA ABS
    addr = getAddress(ABS);
    rd(&CPU::ORA, addr);
    break;
case 0x0E: // ASL ABS
    addr = getAddress(ABS);
    rmw(&CPU::ASL, addr);
    break;
case 0x0F: //*SLO ABS
    addr = getAddress(ABS);
    rmw(&CPU::SLO, addr); // 6
    break;
case 0x10: // BPL
    bra(!(regs.p & flags.N_FLAG));
    break;
case 0x11: // ORA INDY
    addr = getAddress(INDY);
    rd(&CPU::ORA, addr);
    break;
case 0x13: //*SLO INDY
    addr = getAddress(INDY);
    rmw(&CPU::SLO, addr);
    break;
case 0x14: //*NOP ZPX
    addr = getAddress(ZPX);
    rd(&CPU::NOP, addr);
    break;
case 0x15: // ORA ZPX
    addr = getAddress(ZPX);
    rd(&CPU::ORA, addr);
    break;
case 0x16: // ASL ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::ASL, addr);
    break;
case 0x17: //*SLO ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::SLO, addr);
    break;
case 0x18: // CLC
    pollForInterrupts();
    read(regs.pc);
    CLC();
    break;
case 0x19: // ORA ABSY
    addr = getAddress(ABSY);
    rd(&CPU::ORA, addr);
    break;
case 0x1A: //*NOP
    read(regs.pc);
    break;
case 0x1B: //*SLO ABSY
    addr = getAddress(ABSY);
    rmw(&CPU::SLO, addr);
    break;
case 0x1C: //*NOP ABSX
    addr = getAddress(ABSX);
    rd(&CPU::NOP, addr);
    break;
case 0x1D: // ORA ABSX
    addr = getAddress(ABSX);
    rd(&CPU::ORA, addr);
    break;
case 0x1E: // ASL ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::ASL, addr);
    break;
case 0x1F: //*SLO ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::SLO, addr);
    break;
case 0x20: // JSR
    // addr = getAddress(ABS);
    JSR(addr);
    break;
case 0x21: // AND INDX
    addr = getAddress(INDX);
    rd(&CPU::AND, addr);
    break;
case 0x23: //*RLA INDX
    addr = getAddress(INDX);
    rmw(&CPU::RLA, addr);
    break;
case 0x24: // BIT ZP
    addr = getAddress(ZP);
    rd(&CPU::BIT, addr);
    break;
case 0x25: // AND ZP
    addr = getAddress(ZP);
    rd(&CPU::AND, addr);
    break;
case 0x26: // ROL ZP
    addr = getAddress(ZP);
    rmw(&CPU::ROL, addr);
    break;
case 0x27: // RLA ZP
    addr = getAddress(ZP);
    rmw(&CPU::RLA, addr);
    break;
case 0x28: // PLP
    read(regs.pc);
    PLP();
    break;
case 0x29: // AND IMM
    addr = getAddress(IMM);
    rd(&CPU::AND, addr);
    break;
case 0x2A: // ROL ACC
    pollForInterrupts();
    read(regs.pc);
    ROL(regs.a);
    break;
case 0x2B: //*AAC IMM
    addr = getAddress(IMM);
    rd(&CPU::AAC, addr);
    break;
case 0x2C: // BIT ABS
    addr = getAddress(ABS);
    rd(&CPU::BIT, addr);
    break;
case 0x2D: // AND ABS
    addr = getAddress(ABS);
    rd(&CPU::AND, addr);
    break;
case 0x2E: // ROL ABS
    addr = getAddress(ABS);
    rmw(&CPU::ROL, addr);
    break;
case 0x2F: // RLA ABS
    addr = getAddress(ABS);
    rmw(&CPU::RLA, addr);
    break;
case 0x30: // BMI
    bra(regs.p & flags.N_FLAG);
    break;
case 0x31: // AND INDY
    addr = getAddress(INDY);
    rd(&CPU::AND, addr);
    break;
case 0x33: //*RLA INDY
    addr = getAddress(INDY);
    rmw(&CPU::RLA, addr);
    break;
case 0x34: //*NOP ZPX
    addr = getAddress(ZPX);
    rd(&CPU::NOP, addr);
    break;
case 0x35: // AND ZPX
    addr = getAddress(ZPX);
    rd(&CPU::AND, addr);
    break;
case 0x36: // ROL ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::ROL, addr);
    break;
case 0x37: //*RLA ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::RLA, addr);
    break;
case 0x38: // SEC
    pollForInterrupts();
    read(regs.pc);
    SEC();
    break;
case 0x39: // AND ABSY
    addr = getAddress(ABSY);
    rd(&CPU::AND, addr);
    break;
case 0x3A: //*NOP
    read(regs.pc);
    break;
case 0x3B: //*RLA ABSY
    addr = getAddress(ABSY);
    rmw(&CPU::RLA, addr);
    break;
case 0x3C: //*NOP ABSX
    addr = getAddress(ABSX);
    rd(&CPU::NOP, addr);
    break;
case 0x3D: // AND ABSX
    addr = getAddress(ABSX);
    rd(&CPU::AND, addr);
    break;
case 0x3E: // ROL ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::ROL, addr);
    break;
case 0x3F: // RLA ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::RLA, addr);
    break;
case 0x40: // RTI
    // read(regs.pc);
    RTI();
    break;
case 0x41: // EOR INDX
    addr = getAddress(INDX);
    rd(&CPU::EOR, addr);
    break;
case 0x43: //*SRE INDX
    addr = getAddress(INDX);
    rmw(&CPU::SRE, addr);
    break;
case 0x44: //*NOP ZP
    addr = getAddress(ZP);
    rd(&CPU::NOP, addr);
    break;
case 0x45: // EOR ZP
    addr = getAddress(ZP);
    rd(&CPU::EOR, addr);
    break;
case 0x46: // LSR ZP
    addr = getAddress(ZP);
    rmw(&CPU::LSR, addr);
    break;
case 0x47: //*SRE ZP
    addr = getAddress(ZP);
    rmw(&CPU::SRE, addr);
    break;
case 0x48:
    read(regs.pc);
    PHA();
    break;
case 0x49: // EOR IMM
    addr = getAddress(IMM);
    rd(&CPU::EOR, addr);
    break;
case 0x4A: // LSR ACC
    pollForInterrupts();
    read(regs.pc);
    LSR(regs.a);
    break;
case 0x4B: //*ASR IMM
    addr = getAddress(IMM);
    rd(&CPU::ASR, addr);
    break;
case 0x4C: // JMP
    JMPAbs();
    break;
case 0x4D: // EOR ABS
    addr = getAddress(ABS);
    rd(&CPU::EOR, addr);
    break;
case 0x4E: // LSR ABS
    addr = getAddress(ABS);
    rmw(&CPU::LSR, addr);
    break;
case 0x4F: //*SRE ABS
    addr = getAddress(ABS);
    rmw(&CPU::SRE, addr);
    break;
case 0x50: // BVC
    bra(!(regs.p & flags.V_FLAG));
    break;
case 0x51: // EOR INDY
    addr = getAddress(INDY);
    rd(&CPU::EOR, addr);
    break;
case 0x53: //*SRE INDY
    addr = getAddress(INDY);
    rmw(&CPU::SRE, addr);
    break;
case 0x54: //*NOP ZPX
    addr = getAddress(ZPX);
    rd(&CPU::NOP, addr);
    break;
case 0x55: // EOR ZPX
    addr = getAddress(ZPX);
    rd(&CPU::EOR, addr);
    break;
case 0x56: // LSR ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::LSR, addr);
    break;
case 0x57: //*SRE ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::SRE, addr);
    break;
case 0x58: // CLI
    pollForInterrupts();
    read(regs.pc);
    CLI();
    break;
case 0x59: // EOR ABSY
    addr = getAddress(ABSY);
    rd(&CPU::EOR, addr);
    break;
case 0x5A: //*NOP
    read(regs.pc);
    break;
case 0x5B: //*SRE ABSY
    addr = getAddress(ABSY);
    rmw(&CPU::SRE, addr);
    break;
case 0x5C: //*NOP ABSX
    addr = getAddress(ABSX);
    rd(&CPU::NOP, addr);
    break;
case 0x5D: // EOR ABSX
    addr = getAddress(ABSX);
    rd(&CPU::EOR, addr);
    break;
case 0x5E: // LSR ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::LSR, addr);
    break;
case 0x5F: //*SRE ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::SRE, addr);
    break;
case 0x60: // RTS
    read(regs.pc);
    RTS();
    break;
case 0x61: // ADC INDX
    addr = getAddress(INDX);
    rd(&CPU::ADC, addr);
    break;
case 0x63: //*RRA INDX
    addr = getAddress(INDX);
    rmw(&CPU::RRA, addr);
    break;
case 0x64: //*NOP ZP
    addr = getAddress(ZP);
    rd(&CPU::NOP, addr);
    break;
case 0x65: // ADC ZP
    addr = getAddress(ZP);
    rd(&CPU::ADC, addr);
    break;
case 0x66: // ROR ZP
    addr = getAddress(ZP);
    rmw(&CPU::ROR, addr);
    break;
case 0x67: //*RRA ZP
    addr = getAddress(ZP);
    rmw(&CPU::RRA, addr);
    break;
case 0x68: // PLA
    read(regs.pc);
    PLA();
    break;
case 0x69: // ADC IMM
    addr = getAddress(IMM);
    rd(&CPU::ADC, addr);
    break;
case 0x6A: // ROR ACC
    pollForInterrupts();
    read(regs.pc);
    ROR(regs.a);
    break;
case 0x6B: //*ARR IMM
    addr = getAddress(IMM);
    rd(&CPU::ARR, addr);
    break;
case 0x6C: // JMP IND
    JMPInd();
    break;
case 0x6D: // ADC ABS
    addr = getAddress(ABS);
    rd(&CPU::ADC, addr);
    break;
case 0x6E: // ROR ABS
    addr = getAddress(ABS);
    rmw(&CPU::ROR, addr);
    break;
case 0x6F: //*RRA ABS
    addr = getAddress(ABS);
    rmw(&CPU::RRA, addr);
    break;
case 0x70: // BVS
    bra(regs.p & flags.V_FLAG);
    break;
case 0x71: // ADC INDY
    addr = getAddress(INDY);
    rd(&CPU::ADC, addr);
    break;
case 0x73: //*RRA INDY
    addr = getAddress(INDY);
    rmw(&CPU::RRA, addr);
    break;
case 0x74: //*NOP ZPX
    addr = getAddress(ZPX);
    rd(&CPU::NOP, addr);
    break;
case 0x75: // ADC ZPX
    addr = getAddress(ZPX);
    rd(&CPU::ADC, addr);
    break;
case 0x76: // ROR ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::ROR, addr);
    break;
case 0x77: //*RRA ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::RRA, addr);
    break;
case 0x78: // SEI
    pollForInterrupts();
    read(regs.pc);
    SEI();
    break;
case 0x79: // ADC ABSY
    addr = getAddress(ABSY);
    rd(&CPU::ADC, addr);
    break;
case 0x7A: //*NOP
    read(regs.pc);
    break;
case 0x7B: //*RRA ABSY
    addr = getAddress(ABSY);
    rmw(&CPU::RRA, addr);
    break;
case 0x7C: //*NOP ABSX
    addr = getAddress(ABSX);
    rd(&CPU::NOP, addr);
    break;
case 0x7D: // ADC ABSX
    addr = getAddress(ABSX);
    rd(&CPU::ADC, addr);
    break;
case 0x7E: // ROR ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::ROR, addr);
    break;
case 0x7F: //*RRA ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::RRA, addr);
    break;
case 0x80: //*NOP IMM
    addr = getAddress(IMM);
    rd(&CPU::NOP, addr);
    break;
case 0x81: // STA INDX
    addr = getAddress(INDX);
    pollForInterrupts();
    write(addr, regs.a);
    break;
case 0x82: //*NOP IMM
    addr = getAddress(IMM);
    rd(&CPU::NOP, addr);
    break;
case 0x83: //*SAX INDX
    addr = getAddress(INDX);
    write(addr, regs.a & regs.x);
    break;
case 0x84: // STY ZP
    addr = getAddress(ZP);
    pollForInterrupts();
    write(addr, regs.y);
    break;
case 0x85: // STA ZP
    addr = getAddress(ZP);
    pollForInterrupts();
    write(addr, regs.a);
    break;
case 0x86: // STX ZP
    addr = getAddress(ZP);
    pollForInterrupts();
    write(addr, regs.x);
    break;
case 0x87: //*SAX ZP
    addr = getAddress(ZP);
    write(addr, regs.a & regs.x);
    break;
case 0x88: // DEY
    pollForInterrupts();
    read(regs.pc);
    DEY();
    break;
case 0x89: //*NOP IMM
    addr = getAddress(IMM);
    rd(&CPU::NOP, addr);
    break;
case 0x8A: // TXA
    pollForInterrupts();
    read(regs.pc);
    TXA();
    break;
case 0x8B: //* ANE IMM
    addr = getAddress(IMM);
    rd(&CPU::ANE, addr);
    break;
case 0x8C: // STY ABS
    addr = getAddress(ABS);
    pollForInterrupts();
    write(addr, regs.y);
    break;
case 0x8D: // STA ABS
    addr = getAddress(ABS);
    pollForInterrupts();
    write(addr, regs.a);
    break;
case 0x8E: // STX ABS
    addr = getAddress(ABS);
    pollForInterrupts();
    write(addr, regs.x);
    break;
case 0x8F: //*SAX ABS
    addr = getAddress(ABS);
    write(addr, regs.a & regs.x);
    break;
case 0x90: // BCC
    bra(!(regs.p & flags.C_FLAG));
    break;
case 0x91: // STA INDY
    addr = getAddress(INDY);
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    pollForInterrupts();
    write(addr & 0xFFFF, regs.a);
    break;
case 0x93: // * AXA INDY
{
    unsigned char res = regs.a & regs.x & ((addr >> 8) + 1);
    addr = getAddress(INDY);
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    pollForInterrupts();
    write(addr, res);
    break;
}
case 0x94: // STY ZPX
    addr = getAddress(ZPX);
    pollForInterrupts();
    write(addr, regs.y);
    break;
case 0x95: // STA ZPX
    addr = getAddress(ZPX);
    pollForInterrupts();
    write(addr, regs.a);
    break;
case 0x96: // STX ZPY
    addr = getAddress(ZPY);
    pollForInterrupts();
    write(addr, regs.x);
    break;
case 0x97: //*SAX ZPY
    addr = getAddress(ZPY);
    write(addr, regs.a & regs.x);
    break;
case 0x98: // TYA
    pollForInterrupts();
    read(regs.pc);
    TYA();
    break;
case 0x99: // STA ABSY
    addr = getAddress(ABSY);
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    pollForInterrupts();
    write(addr & 0xFFFF, regs.a);
    break;
case 0x9A: // TXS
    pollForInterrupts();
    read(regs.pc);
    TXS();
    break;
case 0x9B: // * SHS ABSY
    addr = getAddress(ABSY);
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    regs.sp = regs.a & regs.x;
    pollForInterrupts();
    write(addr, regs.sp & ((addr >> 8) + 1));
    break;
case 0x9C: // *SHY ABSX
    addr = getAddress(ABSX);
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    pollForInterrupts();
    write(addr, regs.y & ((addr >> 8) + 1));
    break;
case 0x9D: // STA ABSX
    addr = getAddress(ABSX);
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    pollForInterrupts();
    write(addr & 0xFFFF, regs.a);
    break;
case 0x9E: // *SHX ABSX
    addr = getAddress(ABSY);
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    pollForInterrupts();
    write(addr, regs.x & ((addr >> 8) + 1));
    break;
case 0x9F: // *AXA ABSY
{
    unsigned char res = regs.a & regs.x & ((addr >> 8) + 1);
    addr = getAddress(ABSY);
    if (needsDummy) {
        read(addr); // The dummiest cycle ever.
        addr = addr + (pageCrossed << 8); // Fix high byte
    }
    pollForInterrupts();
    write(addr, res);
} break;
case 0xA0: // LDY IMM
    addr = getAddress(IMM);
    rd(&CPU::LDY, addr);
    break;
case 0xA1: // LDA INDX
    addr = getAddress(INDX);
    rd(&CPU::LDA, addr);
    break;
case 0xA2: // LDX IMM
    addr = getAddress(IMM);
    rd(&CPU::LDX, addr);
    break;
case 0xA3: //*LAX INDX
    addr = getAddress(INDX);
    rd(&CPU::LAX, addr);
    break;
case 0xA4: // LDY ZP
    addr = getAddress(ZP);
    rd(&CPU::LDY, addr);
    break;
case 0xA5: // LDA ZP
    addr = getAddress(ZP);
    rd(&CPU::LDA, addr);
    break;
case 0xA6: // LDX ZP
    addr = getAddress(ZP);
    rd(&CPU::LDX, addr);
    break;
case 0xA7: //*LAX ZP
    addr = getAddress(ZP);
    rd(&CPU::LAX, addr);
    break;
case 0xA8: // TAY
    pollForInterrupts();
    read(regs.pc);
    TAY();
    break;
case 0xA9: // LDA IMM
    addr = getAddress(IMM);
    rd(&CPU::LDA, addr);
    break;
case 0xAA: // TAX
    pollForInterrupts();
    read(regs.pc);
    TAX();
    break;
case 0xAB: //*ATX IMM
    addr = getAddress(IMM);
    rd(&CPU::ATX, addr);
    break;
case 0xAC: // LDY ABS
    addr = getAddress(ABS);
    rd(&CPU::LDY, addr);
    break;
case 0xAD: // LDA ABS
    addr = getAddress(ABS);
    rd(&CPU::LDA, addr);
    break;
case 0xAE: // LDX ABS
    addr = getAddress(ABS);
    rd(&CPU::LDX, addr);
    break;
case 0xAF: // LAX ABS
    addr = getAddress(ABS);
    rd(&CPU::LAX, addr);
    break;
case 0xB0: // BCS
    bra(regs.p & flags.C_FLAG);
    break;
case 0xB1: // LDA INDY
    addr = getAddress(INDY);
    rd(&CPU::LDA, addr);
    break;
case 0xB3: //*LAX INDY
    addr = getAddress(INDY);
    rd(&CPU::LAX, addr);
    break;
case 0xB4: // LDY ZPX
    addr = getAddress(ZPX);
    rd(&CPU::LDY, addr);
    break;
case 0xB5: // LDA ZPX
    addr = getAddress(ZPX);
    rd(&CPU::LDA, addr);
    break;
case 0xB6: // LDX ZPY
    addr = getAddress(ZPY);
    rd(&CPU::LDX, addr);
    break;
case 0xB7: //*LAX ZPY
    addr = getAddress(ZPY);
    rd(&CPU::LAX, addr);
    break;
case 0xB8: // CLV
    pollForInterrupts();
    read(regs.pc);
    CLV();
    break;
case 0xB9: // LDA ABSY
    addr = getAddress(ABSY);
    rd(&CPU::LDA, addr);
    break;
case 0xBA: // TSX
    pollForInterrupts();
    read(regs.pc);
    TSX();
    break;
case 0xBB: // *LAE ABSY
    addr = getAddress(ABSY);
    rd(&CPU::LAE, addr);
    break;
case 0xBC: // LDY ABSX
    addr = getAddress(ABSX);
    rd(&CPU::LDY, addr);
    break;
case 0xBD: // LDA ABSX
    addr = getAddress(ABSX);
    rd(&CPU::LDA, addr);
    break;
case 0xBE: // LDX ABSY
    addr = getAddress(ABSY);
    rd(&CPU::LDX, addr);
    break;
case 0xBF: //*LAX ABSY
    addr = getAddress(ABSY);
    rd(&CPU::LAX, addr);
    break;
case 0xC0: // CPY IMM
    addr = getAddress(IMM);
    rd(&CPU::CPY, addr);
    break;
case 0xC1: // CMP INDX
    addr = getAddress(INDX);
    rd(&CPU::CMP, addr);
    break;
case 0xC2: //*NOP IMM
    addr = getAddress(IMM);
    rd(&CPU::NOP, addr);
    break;
case 0xC3: //*DCP INDX
    addr = getAddress(INDX);
    rmw(&CPU::DCP, addr);
    break;
case 0xC4: // CPY ZP
    addr = getAddress(ZP);
    rd(&CPU::CPY, addr);
    break;
case 0xC5: // CMP ZP
    addr = getAddress(ZP);
    rd(&CPU::CMP, addr);
    break;
case 0xC6: // DEC ZP
    addr = getAddress(ZP);
    rmw(&CPU::DEC, addr);
    break;
case 0xC7: //*DCP ZP
    addr = getAddress(ZP);
    rmw(&CPU::DCP, addr);
    break;
case 0xC8: // INY
    pollForInterrupts();
    read(regs.pc);
    INY();
    break;
case 0xC9: // CMP IMM
    addr = getAddress(IMM);
    rd(&CPU::CMP, addr);
    break;
case 0xCA: // DEX
    pollForInterrupts();
    read(regs.pc);
    DEX();
    break;
case 0xCB: //*AXS IMM
    addr = getAddress(IMM);
    rd(&CPU::AXS, addr);
    break;
case 0xCC: // CPY ABS
    addr = getAddress(ABS);
    rd(&CPU::CPY, addr);
    break;
case 0xCD: // CMP ABS
    addr = getAddress(ABS);
    rd(&CPU::CMP, addr);
    break;
case 0xCE: // DEC ABS
    addr = getAddress(ABS);
    rmw(&CPU::DEC, addr);
    break;
case 0xCF: //*DCP ABS
    addr = getAddress(ABS);
    rmw(&CPU::DCP, addr);
    break;
case 0xD0: // BNE
    bra(!(regs.p & flags.Z_FLAG));
    break;
case 0xD1: // CMP INDY
    addr = getAddress(INDY);
    rd(&CPU::CMP, addr);
    break;
case 0xD3: //*DCP INDY
    addr = getAddress(INDY);
    rmw(&CPU::DCP, addr);
    break;
case 0xD4: //*NOP ZPX
    addr = getAddress(ZPX);
    rd(&CPU::NOP, addr);
    break;
case 0xD5: // CMP ZPX
    addr = getAddress(ZPX);
    rd(&CPU::CMP, addr);
    break;
case 0xD6: // DEC ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::DEC, addr);
    break;
case 0xD7: //*DCP ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::DCP, addr);
    break;
case 0xD8: // CLD
    pollForInterrupts();
    read(regs.pc);
    CLD();
    break;
case 0xD9: // CMP ABSY
    addr = getAddress(ABSY);
    rd(&CPU::CMP, addr);
    break;
case 0xDA: //*NOP
    pollForInterrupts();
    read(regs.pc);
    break;
case 0xDB: //*DCP ABSY
    addr = getAddress(ABSY);
    rmw(&CPU::DCP, addr);
    break;
case 0xDC: //*NOP ABSX
    addr = getAddress(ABSX);
    rd(&CPU::NOP, addr);
    break;
case 0xDD: // CMP ABSX
    addr = getAddress(ABSX);
    rd(&CPU::CMP, addr);
    break;
case 0xDE: // DEC ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::DEC, addr);
    break;
case 0xDF: //*DCP ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::DCP, addr);
    break;
case 0xE0: // CPX IMM
    addr = getAddress(IMM);
    rd(&CPU::CPX, addr);
    break;
case 0xE1: // SBC INDX
    addr = getAddress(INDX);
    rd(&CPU::SBC, addr);
    break;
case 0xE2: //*NOP IMM
    addr = getAddress(IMM);
    rd(&CPU::NOP, addr);
    break;
case 0xE3: //*ISB INDX
    addr = getAddress(INDX);
    rmw(&CPU::ISB, addr);
    break;
case 0xE4: // CPX ZP
    addr = getAddress(ZP);
    rd(&CPU::CPX, addr);
    break;
case 0xE5: // SBC ZP
    addr = getAddress(ZP);
    rd(&CPU::SBC, addr);
    break;
case 0xE6: // INC ZP
    addr = getAddress(ZP);
    rmw(&CPU::INC, addr);
    break;
case 0xE7: //*ISB ZP
    addr = getAddress(ZP);
    rmw(&CPU::ISB, addr);
    break;
case 0xE8: // INX
    pollForInterrupts();
    read(regs.pc);
    INX();
    break;
case 0xE9: // SBC IMM
    addr = getAddress(IMM);
    rd(&CPU::SBC, addr);
    break;
case 0xEA: // NOP
    pollForInterrupts();
    read(regs.pc);
    break;
case 0xEB: //*SBC IMM
    addr = getAddress(IMM);
    rd(&CPU::SBC, addr);
    break;
case 0xEC: // CPX ABS
    addr = getAddress(ABS);
    rd(&CPU::CPX, addr);
    break;
case 0xED: // SBC ABS
    addr = getAddress(ABS);
    rd(&CPU::SBC, addr);
    break;
case 0xEE: // INC ABS
    addr = getAddress(ABS);
    rmw(&CPU::INC, addr);
    break;
case 0xEF: //*ISB ABS
    addr = getAddress(ABS);
    rmw(&CPU::ISB, addr);
    break;
case 0xF0: // BEQ
    bra(regs.p & flags.Z_FLAG);
    break;
case 0xF1: // SBC INDY
    addr = getAddress(INDY);
    rd(&CPU::SBC, addr);
    break;
case 0x02:
case 0x12:
case 0x22:
case 0x32:
case 0x42:
case 0x52:
case 0x62:
case 0x72:
case 0x92:
case 0xB2:
case 0xD2:
case 0xF2:
    pollForInterrupts();
    // read(regs.pc--);
    Log.debug(LogCategory::cpuOpcodes, "HLT Opcode %02X", instData.opcode);
    break;
case 0xF3: //*ISB INDY
    addr = getAddress(INDY);
    rmw(&CPU::ISB, addr);
    break;
case 0xF4: //*NOP ZPX
    addr = getAddress(ZPX);
    rd(&CPU::NOP, addr);
    break;
case 0xF5: // SBC ZPX
    addr = getAddress(ZPX);
    rd(&CPU::SBC, addr);
    break;
case 0xF6: // INC ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::INC, addr);
    break;
case 0xF7: //*ISB ZPX
    addr = getAddress(ZPX);
    rmw(&CPU::ISB, addr);
    break;
case 0xF8: // SED
    pollForInterrupts();
    read(regs.pc);
    SED();
    break;
case 0xF9: // SBC ABSY
    addr = getAddress(ABSY);
    rd(&CPU::SBC, addr);
    break;
case 0xFA: //*NOP
    pollForInterrupts();
    read(regs.pc);
    break;
case 0xFB: //*ISB ABSY
    addr = getAddress(ABSY);
    rmw(&CPU::ISB, addr);
    break;
case 0xFC: //*NOP ABSX
    addr = getAddress(ABSX);
    rd(&CPU::NOP, addr);
    break;
case 0xFD: // SBC ABSX
    addr = getAddress(ABSX);
    rd(&CPU::SBC, addr);
    break;
case 0xFE: // INC ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::INC, addr);
    break;
case 0xFF: //*ISB ABSX
    addr = getAddress(ABSX);
    rmw(&CPU::ISB, addr);
    break;
}
