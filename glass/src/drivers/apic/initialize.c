#include "../acpi/tables/tables.h"
#include "../uart/serial.h"

char* itoa(long int value, unsigned int base);

void apic_initialize(acpi_sdt_header_t* madt) {
    serial_terminal()->puts(itoa((unsigned long)madt, 16))->putc('\n');
}