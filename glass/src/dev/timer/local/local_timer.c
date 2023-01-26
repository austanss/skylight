#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "dev/timer/pit/pit.h"
#include "dev/timer/local/local_timer.h"
#include "dev/apic/lapic.h"
#include "cpu/interrupts/idt.h"
#include "cpu/tss/tss.h"
#include "sys/events/echoes.h"

static uint64_t tpms;
static bool calibrated = false;
static uint8_t vector;

uint8_t _packet[sizeof(echoes_packet_t)];

__attribute__ ((interrupt))
void __local_timer_builtin_handler(void* frame) {
    apic_local_send_eoi();
    echoes_packet_t* packet = (echoes_packet_t *)_packet;
    packet->id = SCHEDULER_TICK_EVENT_ID;
    packet->data_length = 0;
    echoes_broadcast_event(packet);
    return;
}

void local_timer_calibrate() {
    apic_local_write(APIC_LOCAL_REGISTER_DIVIDE_CONFIG, LOCAL_TIMER_DIVISOR_001);

    uint32_t lvt_descriptor = 0x00000000 | (0x01 << 17);
    vector = idt_allocate_vector();

    lvt_descriptor |= vector;

    apic_local_write(APIC_LOCAL_REGISTER_LVT_TIMER, lvt_descriptor);

    apic_local_write(APIC_LOCAL_REGISTER_INITIAL_COUNT, 0xFFFFFFFF);

    local_timer_set_handler(__local_timer_builtin_handler);

    pit_enable();
    pit_deadline_wait(PIT_FREQUENCY / 1000);

    tpms = 0xFFFFFFFF - apic_local_read(APIC_LOCAL_REGISTER_CURRENT_COUNT);

    pit_disable();

    calibrated = true;

    local_timer_set_frequency(20);
}

uint64_t local_timer_get_tpms() {
    return tpms;
}

void local_timer_set_handler(void (*handler)) {
    idt_set_descriptor(vector, (uintptr_t)handler, IDT_DESCRIPTOR_EXTERNAL, TSS_IST_ROUTINE);
}

void local_timer_set_frequency(uint64_t hz) {
    if (hz == 0) {
        apic_local_write(APIC_LOCAL_REGISTER_INITIAL_COUNT, 0);
        return;
    }

    uint64_t counts_per_tick = (tpms * 1000) / hz;
    apic_local_write(APIC_LOCAL_REGISTER_INITIAL_COUNT, counts_per_tick);
}
