#include <string.h>
#include "dev/uart/uartsh.h"
#include "devices.h"
#include <stdio.h>

pci_function_t* find_device(uint8_t class, uint8_t subclass, uint8_t interface) {
    for (size_t i = 0; i < pci_function_cache_entries; i++) {
        pci_dev_header_t* device = (pci_dev_header_t *)pci_function_cache[i].base;
        if (device->device_class != class) continue;
        if (device->device_subclass != subclass) continue;
        if (device->program_interface != interface) continue;

        return &pci_function_cache[i];
    }
    return NULL;
}

void __pci_dump() {
    printf("\nPCI devices:\n\n");
    for (size_t i = 0; i < pci_function_cache_entries; i++) {
        pci_function_t* function = &pci_function_cache[i];
        pci_dev_header_t* header = (pci_dev_header_t *)function->base;
        printf("\t%x:%x:%x:%x, id: %x:%x, class: %x:%x:%x\n", function->segment, function->bus, function->device, function->function, header->vendor_id, header->device_id, header->device_class, header->device_subclass, header->program_interface);
    }
}
