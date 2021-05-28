#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../acpi/tables/madt.h"

typedef struct _apic_io_controller_node {
    void*                               base;
    uint64_t                            ioapic_id;
    struct _apic_io_controller_node*    next;
} apic_io_controller_node_t;

extern apic_io_controller_node_t* ioapics;

void    apic_io_register_controller(acpi_madt_record_ioapic_t controller);
void*   apic_io_get_base(uint64_t ioapic_id);
