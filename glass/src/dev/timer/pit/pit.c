#include "dev/timer/pit/pit.h"
#include "dev/apic/ioapic.h"
#include "cpu/interrupts/idt.h"
#include "cpu/tss/tss.h"
#include "dev/io.h"

uint8_t pit_vector = 0;
uint16_t pit_divisor = 0;

__attribute__ ((interrupt))
void __pit_builtin_handler(void* frame) {
    return;
}

void pit_enable(void) {
    if (pit_vector > 0)
        return;

    pit_vector = idt_allocate_vector();
    apic_io_redirect_irq(0, pit_vector, false, false);
    idt_set_descriptor(pit_vector, (uintptr_t)&__pit_builtin_handler, IDT_DESCRIPTOR_X32_INTERRUPT, tss_add_stack(0));
}

void pit_set_divisor(uint16_t divisor) {
    outb(PIT_REGISTER_COMMAND_MODE, 0x34);
    outb(PIT_REGISTER_CHANNEL0_DATA, divisor & 0xFF);
    outb(PIT_REGISTER_CHANNEL0_DATA, divisor >> 8);
}
