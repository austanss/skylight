#include "lapic.h"
#include <stddef.h>

uint32_t* apic_virtual_base = NULL;

void apic_local_send_eoi() {
    if (apic_virtual_base nullvptr)
        return;

    apic_local_write(APIC_LOCAL_REGISTER_EOI, 0x00000000);
}