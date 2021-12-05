#include <stdint.h>
#include <stdbool.h>

#include "dev/timer/pit/pit.h"
#include "dev/apic/ioapic.h"
#include "dev/apic/lapic.h"
#include "cpu/interrupts/idt.h"
#include "cpu/tss/tss.h"
#include "dev/io.h"

uint8_t pit_vector = 0;
uint16_t pit_divisor = 0;

static uint64_t ticks;
static bool watching = false;

__attribute__ ((interrupt))
void __pit_builtin_handler(void* frame) {
    if (watching)
        ticks++;

    apic_local_send_eoi();

    return;
}

void pit_enable() {
    if (pit_vector > 0)
        return;

    pit_vector = idt_allocate_vector();
    apic_io_redirect_irq(0, pit_vector, false, false);
    idt_set_descriptor(pit_vector, (uintptr_t)&__pit_builtin_handler, IDT_DESCRIPTOR_X32_INTERRUPT, TSS_IST_ROUTINE);
}

void pit_disable() {
    apic_io_mask_irq(0);
    idt_free_vector(pit_vector);
    pit_vector = 0;
}

void pit_set_divisor(uint16_t divisor) {
    outb(PIT_REGISTER_COMMAND_MODE, 0x34);
    outb(PIT_REGISTER_CHANNEL0_DATA, divisor & 0xFF);
    outb(PIT_REGISTER_CHANNEL0_DATA, divisor >> 8);
}

void pit_stopwatch_start() {
    ticks = 0;
    watching = true;
}

uint64_t pit_stopwatch_stop() {
    watching = false;
    return ticks;
}
