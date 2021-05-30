#include "confrw.h"
#include "devices.h"
#include "../acpi/tables/mcfg.h"
#include "mm/paging/paging.h"
#include <stdlib.h>

typedef struct {
    void* base;
    uint16_t segment;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint32_t : 24;
} pci_function_t;

static size_t cache_entries = 0;
static pci_function_t* cache = NULL;

void pci_conf_load_cache() {
    acpi_mcfg_header_t* mcfg = ACPI_MCFG_GET();
    uint64_t table_length = mcfg->common.length - sizeof(acpi_mcfg_header_t);
    uint64_t table_entries = table_length / sizeof(acpi_mcfg_entry_t);
    
    for (uint16_t seg = 0; seg < table_entries; seg++) {
        acpi_mcfg_entry_t* segment = ACPI_MCFG_GET_ENTRY(mcfg, seg);
        for (uint8_t bus = segment->start_bus; bus < segment->end_bus; bus++) {
            uint64_t bus_offset = (uint64_t)bus << 20;
            void* bus_address = (void *)((uint64_t)segment->base + bus_offset);
            pci_dev_header_t* bus_header = (pci_dev_header_t *)paging_map_page((void *)((uintptr_t)bus_address + PAGING_VIRTUAL_OFFSET), bus_address, PAGING_FLAGS_KERNEL_PAGE);

            if (bus_header->device_id == 0x0000 || bus_header->device_id == 0xFFFF)
                continue;

            for (uint8_t device = 0; device < 32; device++) {
                uint64_t device_offset = (uint64_t)device << 15;
                void* device_address = (void *)((uint64_t)bus_address + device_offset);
                pci_dev_header_t* device_header = (pci_dev_header_t *)paging_map_page((void *)((uintptr_t)device_address + PAGING_VIRTUAL_OFFSET), device_address, PAGING_FLAGS_KERNEL_PAGE);

                if (device_header->device_id == 0x0000 || device_header->device_id == 0xFFFF)
                    continue;

                for (uint8_t function = 0; function < 8; function++) {
                    uint64_t function_offset = (uint64_t)function << 12;
                    void* function_address = (void *)((uint64_t)device_address + function_offset);
                    pci_dev_header_t* function_header = (pci_dev_header_t *)paging_map_page((void *)((uintptr_t)function_address + PAGING_VIRTUAL_OFFSET), function_address, PAGING_FLAGS_KERNEL_PAGE);

                    if (function_header->device_id == 0x0000 || function_header->device_id == 0xFFFF)
                        continue;

                    cache_entries++;

                    if (!cache)
                        cache = (pci_function_t *)malloc(sizeof(pci_function_t) * cache_entries);
                    else
                        cache = (pci_function_t *)realloc(cache, sizeof(pci_function_t) * cache_entries);
                    
                    pci_function_t* cached_function = &cache[cache_entries - 1];

                    cached_function->base = (void *)function_header;
                    cached_function->function = function;
                    cached_function->device = device;
                    cached_function->bus = bus;
                    cached_function->segment = seg;
                }
            }
        }
    }
}
