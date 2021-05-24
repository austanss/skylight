#include "../acpi/tables/madt.h"
#include "../uart/serial.h"

void apic_initialize() {
    acpi_madt_header_t* madt = ACPI_MADT_GET();
}