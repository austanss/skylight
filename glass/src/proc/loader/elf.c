#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "proc/loader/elf.h"
#include "mm/pmm/pmm.h"
#include "mm/paging/paging.h"
#include "dev/uart/serial.h"

bool elf_load_segment(void* file, elf_program_header_t* header) {
    size_t pages = header->p_memsz / 0x1000;

    if (pages % 0x1000 > 0)
        pages++;

    void* segment = (void *)header->p_vaddr;

    for (size_t page = 0; page < pages; page++)
        paging_map_page(segment + (page * 0x1000), pmm_alloc_page() - PAGING_VIRTUAL_OFFSET, PAGING_FLAGS_USER_PAGE);

    memset(segment, 0x00, header->p_memsz);

    memcpy(segment, file + header->p_offset, header->p_filesz);

    return true;
}

void* elf_load_program(void* file) {
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

    for (elf_program_header_t* pheader = (elf_program_header_t *)(file + header->header_table);;) {
        if (((uint64_t)pheader - ((uint64_t)file + header->header_table)) >= header_table_size)
            break;

        if (pheader->type == ELF_SEGMENT_LOAD)
            if (!elf_load_segment(file, pheader))
                return NULL;

        pheader = (elf_program_header_t *)((void *)pheader + header->program_table_entry_size);
    }

    return (void *)header->program_entry;
}
