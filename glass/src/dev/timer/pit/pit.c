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

static volatile uint64_t ticks;
static bool watching = false;

__attribute__ ((interrupt))
void __pit_builtin_handler(void* frame) {
    if (watching)
        ticks += pit_divisor;

    apic_local_send_eoi();

    return;
}

static uint8_t gsi = 0xFF;

void pit_enable() {
    if (gsi != 0xFF)
        return;

    pit_vector = idt_allocate_vector();
    gsi = apic_io_get_gsi(PIT_IRQ_LINE);

    apic_io_redirect_t existing = apic_io_get_redirect(gsi);

    apic_io_redirect_irq(gsi, pit_vector, existing.active_low, existing.level_triggered);
    
    pit_set_divisor(8);
    
    idt_set_descriptor(pit_vector, (uintptr_t)&__pit_builtin_handler, IDT_DESCRIPTOR_EXTERNAL, TSS_IST_ROUTINE);
    apic_io_unmask_irq(gsi);
}

void pit_disable() {
    apic_io_mask_irq(gsi);
    idt_free_vector(pit_vector);
    pit_vector = 0;
}

void pit_set_divisor(uint16_t divisor) {
    outb(PIT_REGISTER_COMMAND_MODE, 0x34);
    outb(PIT_REGISTER_CHANNEL0_DATA, divisor & 0xFF);
    outb(PIT_REGISTER_CHANNEL0_DATA, divisor >> 8);
    pit_divisor = divisor;
}

void pit_stopwatch_start() {
    ticks = 0;
    watching = true;
}

uint64_t pit_stopwatch_stop() {
    watching = false;
    return ticks;
}

void pit_deadline_wait(uint64_t delay_ticks) {
    ticks = 0;
    watching = true;
    
    while (ticks < delay_ticks)
        asm ("hlt");

    watching = false;
    return;
}
