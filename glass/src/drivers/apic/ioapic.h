#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../acpi/tables/madt.h"

#define IOAPIC_REGISTER_ID              0x0000
#define IOAPIC_REGISTER_VERSION         0x0001
#define IOAPIC_REGISTER_ARBITRATION     0x0001
#define IOAPIC_REGISTER_REDIRECTION(n)  0x0010 + (2 * n)

#define IOAPIC_REGSEL                   0x0000
#define IOAPIC_DATA                     0x0010

typedef struct _apic_io_controller_node {
    void*                               base;
    uint64_t                            ioapic_id;
    struct _apic_io_controller_node*    next;
} apic_io_controller_node_t;

extern apic_io_controller_node_t* ioapics;

void    apic_io_register_controller(acpi_madt_record_ioapic_t controller);
void*   apic_io_get_base(uint64_t ioapic_id);

static
inline
void apic_io_write(uint64_t id, uint32_t reg, uint32_t value) {
    void* base = apic_io_get_base(id);
    uint32_t volatile* regs = (uint32_t *)((uintptr_t)base + IOAPIC_REGSEL);
    uint32_t volatile* data = (uint32_t *)((uintptr_t)base + IOAPIC_DATA);

    *regs = reg;
    *data = value;
}

static
inline
uint32_t apic_io_read(uint64_t id, uint32_t reg) {
    void* base = apic_io_get_base(id);
    uint32_t volatile* regs = (uint32_t *)((uintptr_t)base + IOAPIC_REGSEL);
    uint32_t volatile* data = (uint32_t *)((uintptr_t)base + IOAPIC_DATA);

    *regs = reg;
    uint32_t value = *data;
    return value;
}
