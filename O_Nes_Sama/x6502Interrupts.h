#ifndef X6502INTERRUPTS_H_INCLUDED
#define X6502INTERRUPTS_H_INCLUDED

struct InterruptLines{
    /* All are active-low */
    int nmi;
    int irq;
    int reset;
};

#endif // X6502INTERRUPTS_H_INCLUDED
