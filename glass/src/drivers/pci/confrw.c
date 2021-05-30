#include "confrw.h"
#include "devices.h"
#include "../acpi/tables/mcfg.h"
#include <stdlib.h>

typedef struct {
    void* base;
    uint16_t segment;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
} pci_function_t;

void pci_conf_load_cache() {
    acpi_mcfg_header_t* mcfg = ACPI_MCFG_GET();
    uint64_t table_length = mcfg->common.length - sizeof(acpi_mcfg_header_t);
    uint64_t table_entries = table_length / sizeof(acpi_mcfg_entry_t);
    (void)table_entries;
}
