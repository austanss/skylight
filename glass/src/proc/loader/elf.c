#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "proc/loader/elf.h"
#include "mm/pmm/pmm.h"
#include "mm/paging/paging.h"
#include "dev/uart/serial.h"

bool elf_load_segment(void* file, elf_program_header_t* header, elf_load_info_t* out_info) {
    size_t pages = header->p_memsz / PAGING_PAGE_SIZE;

    if (header->p_memsz % PAGING_PAGE_SIZE > 0)
        pages++;

    void* segment = (void *)header->p_vaddr;

    void* segment_virt = pmm_alloc_pool(pages);

    for (size_t page = 0; page < pages; page++)
        paging_map_page(segment + (page * PAGING_PAGE_SIZE), (segment_virt + (page * PAGING_PAGE_SIZE)), PAGING_FLAGS_USER_PAGE);

    memset(segment, 0x00, header->p_memsz);

    memcpy(segment, file + header->p_offset, header->p_filesz);

    out_info->segments[out_info->segment_count].loaded_at = header->p_vaddr;
    out_info->segments[out_info->segment_count].located_at = ((uint64_t)segment_virt);
    out_info->segments[out_info->segment_count].length = pages * PAGING_PAGE_SIZE;

    out_info->segment_count++;

    return true;
}

elf_load_info_t* elf_load_program(void* file) {
    elf_header_t* header = (elf_header_t *)file;

    serial_terminal()->puts("\nLoading program from file @ ")->putul((uint64_t)file)->puts("...\n\n");

    serial_terminal()->puts("Signature: ")->putc(header->magic[0])->putc(header->magic[1])->putc(header->magic[2])->putc(header->magic[3])->putc('\n');
    serial_terminal()->puts("Bitness: ")->puts(header->bitness == ELF_BITNESS_64 ? "64-bit" : "Incompatible")->putc('\n');
    serial_terminal()->puts("Endianness: ")->puts(header->endianness == ELF_ENDIANNESS_LITTLE ? "Little-endian" : "Incompatible")->putc('\n');
    serial_terminal()->puts("ABI: ")->puts(header->abi == ELF_ABI_SYSTEMV ? "System-V" : "Incompatible")->putc('\n');
    serial_terminal()->puts("R-E-S-C: ")->putul(header->resc)->putc('\n');
    serial_terminal()->puts("Architecture: ")->puts(header->architecture == ELF_ARCH_x86_64 ? "x86-64" : "Incompatible")->putc('\n');
    serial_terminal()->puts("Entrypoint: ")->putul(header->program_entry)->putc('\n');

    if (strncmp((char *)&header->magic[0], ELF_HEADER_MAGIC, 4) > 0)
        return NULL;

    if (header->architecture != ELF_ARCH_x86_64)
        return NULL;

    if (header->endianness != ELF_ENDIANNESS_LITTLE || header->bitness != ELF_BITNESS_64)
        return NULL;

    if (header->abi != ELF_ABI_SYSTEMV)
        return NULL;

    size_t header_table_size = header->program_table_entry_size * header->program_table_entries;

    elf_load_info_t* load_info = (elf_load_info_t *)malloc(sizeof(elf_load_info_t));
    load_info->entry = header->program_entry;
    load_info->segment_count = 0;
    load_info->segments = (elf_load_segment_t *)pmm_alloc_page();

    for (elf_program_header_t* pheader = (elf_program_header_t *)(file + header->header_table);;) {
        if (((uint64_t)pheader - ((uint64_t)file + header->header_table)) >= header_table_size)
            break;

        if (pheader->type == ELF_SEGMENT_LOAD)
            if (!elf_load_segment(file, pheader, load_info))
                return NULL;

        pheader = (elf_program_header_t *)((void *)pheader + header->program_table_entry_size);
    }

    serial_terminal()->puts("\nLoaded segments:\n");
    for (uint64_t i = 0; i < load_info->segment_count; i++)
        serial_terminal()->puts("\tSegment ")->putul(i)->puts(" @ ")->putul(load_info->segments[i].loaded_at)->puts(" -> ")->putul(load_info->segments[i].located_at)->puts(" (")->putul(load_info->segments[i].length)->puts(" bytes)\n");

    return load_info;
}
