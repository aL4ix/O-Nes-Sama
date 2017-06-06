#ifndef CPUIO_HPP_INCLUDED
#define CPUIO_HPP_INCLUDED

struct CPUIO {
    int nmi;
    int irq;
    int reset;
    int wr;
    unsigned char dataBus;
    int addressBus;
};

#endif // CPUIO_HPP_INCLUDED
