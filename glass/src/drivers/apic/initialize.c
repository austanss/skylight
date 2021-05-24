#include "../acpi/tables/madt.h"
#include "../8259/pic.h"

void apic_initialize() {
    acpi_madt_header_t* madt = ACPI_MADT_GET();

    pic_disable();
}