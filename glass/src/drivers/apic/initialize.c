#include "../acpi/tables/madt.h"
#include "../8259/pic.h"
#include "../uart/serial.h"
#include "cpuid.h"
#include <stdbool.h>

void apic_initialize() {
    acpi_madt_header_t* madt = ACPI_MADT_GET();

    pic_disable();

    uint32_t erax, erbx, ercx, erdx;
    __cpuid(1, erax, erbx, ercx, erdx);

    bool apic = (erdx >> 9) & 1;

    if (!apic) {
        serial_terminal()->puts("ERROR: Hardware unsupported (cpuid.01h.edx.9): APIC missing");
        asm volatile ("cli; hlt");
    }

    
}