#include "tss.h"
#include "../gdt/gdt.h"
#include "mm/paging/paging.h"
#include "mm/pmm/pmm.h"
#include "misc/conv.h"
#include "dev/uart/uartsh.h"
#include <string.h>

extern tss_t tss_descriptors[];
tss_t tss_descriptors[TSS_MAX_CPUS];

static uint8_t ist_index = 0;

uint8_t tss_add_stack(int num_cpu) {
    if (ist_index >= 7)
        return 1;

    void* stack = pmm_alloc_pool(IST_STACK_PAGES);

    tss_descriptors[num_cpu].ist[ist_index] = (uint64_t)stack + (PAGING_PAGE_SIZE * IST_STACK_PAGES);
    ist_index++;
    return ist_index;
}

uint8_t tss_get_num_stacks(int num_cpu) {
    return ist_index - 1;
}

void tss_install(int num_cpu) {
    uint64_t tss_base = (uint64_t)&tss_descriptors[num_cpu];
    memset((void *)tss_base, 0, sizeof(tss_t));
    
    tss_add_stack(num_cpu);
    tss_add_stack(num_cpu);
    tss_add_stack(num_cpu);

    tss_descriptors[num_cpu].rsp[0] = tss_descriptors[num_cpu].ist[0];

    tss_descriptors[num_cpu].io_map = sizeof(tss_t);

    tss_reload(gdt_install_tss(tss_base));
}

tss_t* tss_get(int num_cpu) {
    return &tss_descriptors[num_cpu];
}

void* tss_get_stack(int num_cpu, uint8_t stack) {
    return (void*)tss_descriptors[num_cpu].ist[stack];
}

void __tss_dump() {
    serial_print_quiet("\nTSS dump:\n");
    for (int cpu = 0; cpu < TSS_MAX_CPUS; ++cpu) {
        tss_t* tss = tss_get(cpu);
        char itoa_buffer[67];
        memset(itoa_buffer, 0, 67);
        serial_print_quiet("\tTSS for CPU ");
        serial_print_quiet(utoa(cpu, itoa_buffer, 10));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tRSP0: ");
        serial_print_quiet(utoa(tss->rsp[0], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tRSP1: ");
        serial_print_quiet(utoa(tss->rsp[1], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tRSP2: ");
        serial_print_quiet(utoa(tss->rsp[2], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tIST1: ");
        serial_print_quiet(utoa(tss->ist[0], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tIST2: ");
        serial_print_quiet(utoa(tss->ist[1], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tIST3: ");
        serial_print_quiet(utoa(tss->ist[2], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tIST4: ");
        serial_print_quiet(utoa(tss->ist[3], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tIST5: ");
        serial_print_quiet(utoa(tss->ist[4], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tIST6: ");
        serial_print_quiet(utoa(tss->ist[5], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tIST7: ");
        serial_print_quiet(utoa(tss->ist[6], itoa_buffer, 16));
        serial_print_quiet("\n");
        serial_print_quiet("\t\tIO Map: ");
        serial_print_quiet(utoa(tss->io_map, itoa_buffer, 16));
        serial_print_quiet("\n");
    }
}
