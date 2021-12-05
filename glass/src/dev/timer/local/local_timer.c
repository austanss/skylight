#include <stdint.h>
#include <stddef.h>
#include "dev/timer/pit/pit.h"
#include "dev/timer/local/local_timer.h"
#include "dev/apic/lapic.h"
#include "cpu/interrupts/idt.h"

static uint64_t tpms;
static bool calibrated = false;

__attribute__ ((interrupt))
void __local_timer_builtin_handler(void* frame) {
    apic_local_send_eoi();
    return;
}

void local_timer_calibrate() {
    apic_local_write(APIC_LOCAL_REGISTER_DIVIDE_CONFIG, 1);

    uint32_t lvt_descriptor = 0x00000000;
    uint8_t vector = idt_allocate_vector();

    lvt_descriptor |= vector;

    apic_local_write(APIC_LOCAL_REGISTER_LVT_TIMER, lvt_descriptor);

    apic_local_write(APIC_LOCAL_REGISTER_INITIAL_COUNT, 0xFFFFFFFF);

    pit_enable();
    pit_deadline_wait(PIT_FREQUENCY / 1000);

    tpms = 0xFFFFFFFF - apic_local_read(APIC_LOCAL_REGISTER_CURRENT_COUNT);

    pit_disable();

    asm ("hlt");
}

uint64_t local_timer_get_tpms() {
    return tpms;
}


