#include "../acpi/tables/madt.h"
#include "../8259/pic.h"
#include "../uart/serial.h"
#include "cpu/interrupts/idt.h"
#include "cpu/tss/tss.h"
#include "debug.h"
#include "lapic.h"
#include <cpuid.h>
#include <stdbool.h>

extern void spurious_handler(void);

static uint8_t default_apic_ist = 0;

void apic_initialize(void);
void apic_initialize() {
    acpi_madt_header_t* madt = ACPI_MADT_GET();

    pic_disable();

    uint32_t erax, erbx, ercx, erdx;
    __cpuid(1, erax, erbx, ercx, erdx);

    bool apic = (erdx >> 9) & 1;

    if (!apic) {
        serial_terminal()->puts("ERROR: Hardware unsupported (cpuid.01h.edx.9): APIC missing; get a new computer");
        __asm__ volatile ("cli; hlt");
    }

    apic_local_set_base((void *)(uint64_t)madt->lapic_address);

    default_apic_ist = tss_add_stack(0);

    uint8_t spurious_vector = idt_allocate_vector();
    idt_set_descriptor(spurious_vector, (uint64_t)&spurious_handler, IDT_DESCRIPTOR_EXTERNAL, default_apic_ist);

    apic_local_write(APIC_LOCAL_REGISTER_SPURIOUS_INT_VECTOR, 0x100 | spurious_vector);
}
