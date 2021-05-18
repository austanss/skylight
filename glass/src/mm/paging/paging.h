#pragma once
#include <stdint.h>
#include <stdbool.h>

#define PAGING_FLAG_PRESENT         0x001
#define PAGING_FLAG_WRITE           0x002
#define PAGING_FLAG_USER            0x004
#define PAGING_FLAG_WRITE_THROUGH   0x008
#define PAGING_FLAG_CACHE_DISABLE   0x010
#define PAGING_FLAG_ACCESSED        0x020
#define PAGING_FLAG_LARGER_PAGES    0x040
#define PAGING_FLAG_OS_AVAILABLE    0xE00

#define PAGING_FLAGS_KERNEL_PAGE    (PAGING_FLAG_PRESENT | PAGING_FLAG_WRITE)
#define PAGING_FLAGS_USER_PAGE      (PAGING_FLAG_PRESENT | PAGING_FLAG_WRITE | PAGING_FLAG_USER)

#define PAGING_PAGE_SIZE            0x1000
#define PAGING_PAGE_BOUNDARY        0x1000
#define PAGING_PAGE_ALIGNED         __attribute__((aligned(PAGING_PAGE_SIZE)))

typedef struct {
    bool        nx      : 01;
    uint16_t    rsv0    : 11;
    uint64_t    address : 40;
    uint16_t    flags   : 12;
} __attribute__((packed)) paging_desc_t;

typedef struct {
    uint16_t    pml4;
    uint16_t    pml3;
    uint16_t    pml2;
    uint16_t    pml1;
} paging_indexer_t;

static
__attribute__((always_inline)) 
inline
void paging_desc_set_address(paging_desc_t* descriptor, void* address) {
    descriptor->address = (uint64_t)address >> 12;
}

static
__attribute__((always_inline)) 
inline
void* paging_desc_get_address(paging_desc_t* descriptor) {
    return (void*)(descriptor->address << 12);
}

void*   paging_map_page(void* virt, void* phys, uint16_t flags);
void*   paging_remap_page(void* old_virt, void* new_virt);
void*   paging_walk_page(void* virt);
void    paging_edit_page(void* virt, uint16_t flags);