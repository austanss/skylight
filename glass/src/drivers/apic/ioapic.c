#include "ioapic.h"
#include <stdlib.h>

apic_io_controller_node_t* ioapics = NULL;

void apic_io_register_controller(acpi_madt_record_ioapic_t controller) {
    apic_io_controller_node_t* new_ioapic;

    if (!ioapics) {
        ioapics = (apic_io_controller_node_t *)malloc(sizeof(apic_io_controller_node_t));
        new_ioapic = ioapics;
        new_ioapic->next = NULL;
    }
    else {
        apic_io_controller_node_t* before_new;
        for (before_new = ioapics; !!before_new->next; before_new = before_new->next);
        before_new->next = (apic_io_controller_node_t *)malloc(sizeof(apic_io_controller_node_t));
        new_ioapic = before_new->next;
    }

    new_ioapic->base = (void *)(uintptr_t)controller.ioapic_address;
    new_ioapic->ioapic_id = controller.ioapic_id;
}

void* apic_io_get_base(uint64_t ioapic_id) {
    apic_io_controller_node_t* ioapic;

    for (ioapic = ioapics; !!ioapic->next; ioapic = ioapic->next)
        if (ioapic->ioapic_id == ioapic_id)
            return (void *)ioapic->base;

    return NULL;
}
