#include "devices.h"

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
