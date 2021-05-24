#include "pic.h"
#include "../io.h"

void pic_mask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t masks;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
    }
    else {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    masks = inb(port);
    masks |= (1 << irq);
    outb(port, masks);
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t masks;

    if (irq < 8) {
        port = PIC_MASTER_DATA;
    }
    else {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    masks = inb(port);
    masks &= ~(1 << irq);
    outb(port, masks);
}