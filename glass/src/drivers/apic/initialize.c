#include "../acpi/tables/tables.h"
#include "../uart/serial.h"

void apic_initialize(acpi_sdt_header_t* madt) {
    serial_terminal()->putul((uint64_t)madt)->putc('\n');
}