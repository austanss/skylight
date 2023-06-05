#pragma once
#include <stdint.h>

#define TSS_MAX_CPUS 1

#define TSS_IST_EXCEPTION   001
#define TSS_IST_ROUTINE     002

typedef struct tss {
    uint32_t    rsv0;
    uint64_t    rsp[3];
    uint64_t    rsv1;
    uint64_t    ist[7];
    uint64_t    rsv2;
    uint16_t    rsv3;
    uint16_t    io_map;
} __attribute__((packed)) tss_t;

typedef struct {
    uint64_t    tss;
    uint64_t    pid;
    uint64_t    rsp;
} gs_kernel_base_t;

void    tss_install(int num_cpu);
uint8_t tss_add_stack(int num_cpu);
void    tss_reload(uint16_t selector);
