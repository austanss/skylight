#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct _apic_io_controller_node {
    void*                               base;
    uint64_t                            ioapic_id;
    struct _apic_io_controller_node*    next;
} apic_io_controller_node_t;

extern apic_io_controller_node_t* ioapics;
