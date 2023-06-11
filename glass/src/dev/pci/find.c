#include <string.h>
#include "dev/uart/uartsh.h"
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

void __pci_dump() {
    char itoa_buffer[67];
    memset(itoa_buffer, 0, 67);
    serial_print_quiet("\nPCI devices:\n\n");
    for (size_t i = 0; i < pci_function_cache_entries; i++) {
        pci_function_t* function = &pci_function_cache[i];
        pci_dev_header_t* header = (pci_dev_header_t *)function->base;

        serial_print_quiet("\t");
        serial_print_quiet(utoa(function->segment, itoa_buffer, 16));
        serial_print_quiet(":");
        serial_print_quiet(utoa(function->bus, itoa_buffer, 16));
        serial_print_quiet(":");
        serial_print_quiet(utoa(function->device, itoa_buffer, 16));
        serial_print_quiet(".");
        serial_print_quiet(utoa(function->function, itoa_buffer, 16));
        serial_print_quiet(", id: ");
        serial_print_quiet(utoa(header->vendor_id, itoa_buffer, 16));
        serial_print_quiet(":");
        serial_print_quiet(utoa(header->device_id, itoa_buffer, 16));
        serial_print_quiet(", class: ");
        serial_print_quiet(utoa(header->device_class, itoa_buffer, 16));
        serial_print_quiet(":");
        serial_print_quiet(utoa(header->device_subclass, itoa_buffer, 16));
        serial_print_quiet(":");
        serial_print_quiet(utoa(header->program_interface, itoa_buffer, 16));
        serial_print_quiet("\n");
    }
}
