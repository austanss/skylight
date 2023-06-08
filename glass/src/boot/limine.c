#include <string.h>
#include "protocol.h"
#include "dev/uart/serial.h"
#include "dev/acpi/tables/tables.h"
#include "mm/paging/paging.h"
#include "mm/pmm/pmm.h"

#include <stdint.h>

/* Misc */

#ifdef LIMINE_NO_POINTERS
#  define LIMINE_PTR(TYPE) uint64_t
#else
#  define LIMINE_PTR(TYPE) TYPE
#endif

#ifdef __GNUC__
#  define LIMINE_DEPRECATED __attribute__((__deprecated__))
#  define LIMINE_DEPRECATED_IGNORE_START \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#  define LIMINE_DEPRECATED_IGNORE_END \
    _Pragma("GCC diagnostic pop")
#else
#  define LIMINE_DEPRECATED
#  define LIMINE_DEPRECATED_IGNORE_START
#  define LIMINE_DEPRECATED_IGNORE_END
#endif

#define LIMINE_COMMON_MAGIC 0xc7b1dd30df4c8b88, 0x0a82e883a194f07b

struct limine_uuid {
    uint32_t a;
    uint16_t b;
    uint16_t c;
    uint8_t d[8];
};

#define LIMINE_MEDIA_TYPE_GENERIC 0
#define LIMINE_MEDIA_TYPE_OPTICAL 1
#define LIMINE_MEDIA_TYPE_TFTP 2

struct limine_file {
    uint64_t revision;
    LIMINE_PTR(void *) address;
    uint64_t size;
    LIMINE_PTR(char *) path;
    LIMINE_PTR(char *) cmdline;
    uint32_t media_type;
    uint32_t unused;
    uint32_t tftp_ip;
    uint32_t tftp_port;
    uint32_t partition_index;
    uint32_t mbr_disk_id;
    struct limine_uuid gpt_disk_uuid;
    struct limine_uuid gpt_part_uuid;
    struct limine_uuid part_uuid;
};

/* Boot info */

#define LIMINE_BOOTLOADER_INFO_REQUEST { LIMINE_COMMON_MAGIC, 0xf55038d8e2a1202f, 0x279426fcf5f59740 }

struct limine_bootloader_info_response {
    uint64_t revision;
    LIMINE_PTR(char *) name;
    LIMINE_PTR(char *) version;
};

struct limine_bootloader_info_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_bootloader_info_response *) response;
};

/* Stack size */

#define LIMINE_STACK_SIZE_REQUEST { LIMINE_COMMON_MAGIC, 0x224ef0460a8e8926, 0xe1cb0fc25f46ea3d }

struct limine_stack_size_response {
    uint64_t revision;
};

struct limine_stack_size_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_stack_size_response *) response;
    uint64_t stack_size;
};

/* HHDM */

#define LIMINE_HHDM_REQUEST { LIMINE_COMMON_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b }

struct limine_hhdm_response {
    uint64_t revision;
    uint64_t offset;
};

struct limine_hhdm_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_hhdm_response *) response;
};

/* Framebuffer */

#define LIMINE_FRAMEBUFFER_REQUEST { LIMINE_COMMON_MAGIC, 0x9d5827dcd881dd75, 0xa3148604f6fab11b }

#define LIMINE_FRAMEBUFFER_RGB 1

struct limine_video_mode {
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};

struct limine_framebuffer {
    LIMINE_PTR(void *) address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused[7];
    uint64_t edid_size;
    LIMINE_PTR(void *) edid;
    /* Response revision 1 */
    uint64_t mode_count;
    LIMINE_PTR(struct limine_video_mode **) modes;
};

struct limine_framebuffer_response {
    uint64_t revision;
    uint64_t framebuffer_count;
    LIMINE_PTR(struct limine_framebuffer **) framebuffers;
};

struct limine_framebuffer_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_framebuffer_response *) response;
};

/* Terminal */

#define LIMINE_TERMINAL_REQUEST { LIMINE_COMMON_MAGIC, 0xc8ac59310c2b0844, 0xa68d0c7265d38878 }

#define LIMINE_TERMINAL_CB_DEC 10
#define LIMINE_TERMINAL_CB_BELL 20
#define LIMINE_TERMINAL_CB_PRIVATE_ID 30
#define LIMINE_TERMINAL_CB_STATUS_REPORT 40
#define LIMINE_TERMINAL_CB_POS_REPORT 50
#define LIMINE_TERMINAL_CB_KBD_LEDS 60
#define LIMINE_TERMINAL_CB_MODE 70
#define LIMINE_TERMINAL_CB_LINUX 80

#define LIMINE_TERMINAL_CTX_SIZE ((uint64_t)(-1))
#define LIMINE_TERMINAL_CTX_SAVE ((uint64_t)(-2))
#define LIMINE_TERMINAL_CTX_RESTORE ((uint64_t)(-3))
#define LIMINE_TERMINAL_FULL_REFRESH ((uint64_t)(-4))

/* Response revision 1 */
#define LIMINE_TERMINAL_OOB_OUTPUT_GET ((uint64_t)(-10))
#define LIMINE_TERMINAL_OOB_OUTPUT_SET ((uint64_t)(-11))

#define LIMINE_TERMINAL_OOB_OUTPUT_OCRNL (1 << 0)
#define LIMINE_TERMINAL_OOB_OUTPUT_OFDEL (1 << 1)
#define LIMINE_TERMINAL_OOB_OUTPUT_OFILL (1 << 2)
#define LIMINE_TERMINAL_OOB_OUTPUT_OLCUC (1 << 3)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONLCR (1 << 4)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONLRET (1 << 5)
#define LIMINE_TERMINAL_OOB_OUTPUT_ONOCR (1 << 6)
#define LIMINE_TERMINAL_OOB_OUTPUT_OPOST (1 << 7)

LIMINE_DEPRECATED_IGNORE_START

struct LIMINE_DEPRECATED limine_terminal;

typedef void (*limine_terminal_write)(struct limine_terminal *, const char *, uint64_t);
typedef void (*limine_terminal_callback)(struct limine_terminal *, uint64_t, uint64_t, uint64_t, uint64_t);

struct LIMINE_DEPRECATED limine_terminal {
    uint64_t columns;
    uint64_t rows;
    LIMINE_PTR(struct limine_framebuffer *) framebuffer;
};

struct LIMINE_DEPRECATED limine_terminal_response {
    uint64_t revision;
    uint64_t terminal_count;
    LIMINE_PTR(struct limine_terminal **) terminals;
    LIMINE_PTR(limine_terminal_write) write;
};

struct LIMINE_DEPRECATED limine_terminal_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_terminal_response *) response;
    LIMINE_PTR(limine_terminal_callback) callback;
};

LIMINE_DEPRECATED_IGNORE_END

/* 5-level paging */

#define LIMINE_5_LEVEL_PAGING_REQUEST { LIMINE_COMMON_MAGIC, 0x94469551da9b3192, 0xebe5e86db7382888 }

struct limine_5_level_paging_response {
    uint64_t revision;
};

struct limine_5_level_paging_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_5_level_paging_response *) response;
};

/* SMP */

#define LIMINE_SMP_REQUEST { LIMINE_COMMON_MAGIC, 0x95a67b819a1b857e, 0xa0b61b723b6a73e0 }

struct limine_smp_info;

typedef void (*limine_goto_address)(struct limine_smp_info *);

#if defined (__x86_64__) || defined (__i386__)

#define LIMINE_SMP_X2APIC (1 << 0)

struct limine_smp_info {
    uint32_t processor_id;
    uint32_t lapic_id;
    uint64_t reserved;
    LIMINE_PTR(limine_goto_address) goto_address;
    uint64_t extra_argument;
};

struct limine_smp_response {
    uint64_t revision;
    uint32_t flags;
    uint32_t bsp_lapic_id;
    uint64_t cpu_count;
    LIMINE_PTR(struct limine_smp_info **) cpus;
};

#elif defined (__aarch64__)

struct limine_smp_info {
    uint32_t processor_id;
    uint32_t gic_iface_no;
    uint64_t mpidr;
    uint64_t reserved;
    LIMINE_PTR(limine_goto_address) goto_address;
    uint64_t extra_argument;
};

struct limine_smp_response {
    uint64_t revision;
    uint32_t flags;
    uint64_t bsp_mpidr;
    uint64_t cpu_count;
    LIMINE_PTR(struct limine_smp_info **) cpus;
};

#else
#error Unknown architecture
#endif

struct limine_smp_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_smp_response *) response;
    uint64_t flags;
};

/* Memory map */

#define LIMINE_MEMMAP_REQUEST { LIMINE_COMMON_MAGIC, 0x67cf3d9d378a806f, 0xe304acdfc50c3c62 }

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES     6
#define LIMINE_MEMMAP_FRAMEBUFFER            7

struct limine_memmap_entry {
    uint64_t base;
    uint64_t length;
    uint64_t type;
};

struct limine_memmap_response {
    uint64_t revision;
    uint64_t entry_count;
    LIMINE_PTR(struct limine_memmap_entry **) entries;
};

struct limine_memmap_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_memmap_response *) response;
};

/* Entry point */

#define LIMINE_ENTRY_POINT_REQUEST { LIMINE_COMMON_MAGIC, 0x13d86c035a1cd3e1, 0x2b0caa89d8f3026a }

typedef void (*limine_entry_point)(void);

struct limine_entry_point_response {
    uint64_t revision;
};

struct limine_entry_point_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_entry_point_response *) response;
    LIMINE_PTR(limine_entry_point) entry;
};

/* Kernel File */

#define LIMINE_KERNEL_FILE_REQUEST { LIMINE_COMMON_MAGIC, 0xad97e90e83f1ed67, 0x31eb5d1c5ff23b69 }

struct limine_kernel_file_response {
    uint64_t revision;
    LIMINE_PTR(struct limine_file *) kernel_file;
};

struct limine_kernel_file_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_kernel_file_response *) response;
};

/* Module */

#define LIMINE_MODULE_REQUEST { LIMINE_COMMON_MAGIC, 0x3e7e279702be32af, 0xca1c4f3bd1280cee }

#define LIMINE_INTERNAL_MODULE_REQUIRED (1 << 0)

struct limine_internal_module {
    LIMINE_PTR(const char *) path;
    LIMINE_PTR(const char *) cmdline;
    uint64_t flags;
};

struct limine_module_response {
    uint64_t revision;
    uint64_t module_count;
    LIMINE_PTR(struct limine_file **) modules;
};

struct limine_module_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_module_response *) response;

    /* Request revision 1 */
    uint64_t internal_module_count;
    LIMINE_PTR(struct limine_internal_module **) internal_modules;
};

/* RSDP */

#define LIMINE_RSDP_REQUEST { LIMINE_COMMON_MAGIC, 0xc5e77b6b397e7b43, 0x27637845accdcf3c }

struct limine_rsdp_response {
    uint64_t revision;
    LIMINE_PTR(void *) address;
};

struct limine_rsdp_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_rsdp_response *) response;
};

/* SMBIOS */

#define LIMINE_SMBIOS_REQUEST { LIMINE_COMMON_MAGIC, 0x9e9046f11e095391, 0xaa4a520fefbde5ee }

struct limine_smbios_response {
    uint64_t revision;
    LIMINE_PTR(void *) entry_32;
    LIMINE_PTR(void *) entry_64;
};

struct limine_smbios_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_smbios_response *) response;
};

/* EFI system table */

#define LIMINE_EFI_SYSTEM_TABLE_REQUEST { LIMINE_COMMON_MAGIC, 0x5ceba5163eaaf6d6, 0x0a6981610cf65fcc }

struct limine_efi_system_table_response {
    uint64_t revision;
    LIMINE_PTR(void *) address;
};

struct limine_efi_system_table_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_efi_system_table_response *) response;
};

/* Boot time */

#define LIMINE_BOOT_TIME_REQUEST { LIMINE_COMMON_MAGIC, 0x502746e184c088aa, 0xfbc5ec83e6327893 }

struct limine_boot_time_response {
    uint64_t revision;
    int64_t boot_time;
};

struct limine_boot_time_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_boot_time_response *) response;
};

/* Kernel address */

#define LIMINE_KERNEL_ADDRESS_REQUEST { LIMINE_COMMON_MAGIC, 0x71ba76863cc55f63, 0xb2644a48c516a487 }

struct limine_kernel_address_response {
    uint64_t revision;
    uint64_t physical_base;
    uint64_t virtual_base;
};

struct limine_kernel_address_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_kernel_address_response *) response;
};

/* Device Tree Blob */

#define LIMINE_DTB_REQUEST { LIMINE_COMMON_MAGIC, 0xb40ddb48fb54bac7, 0x545081493f81ffb7 }

struct limine_dtb_response {
    uint64_t revision;
    LIMINE_PTR(void *) dtb_ptr;
};

struct limine_dtb_request {
    uint64_t id[4];
    uint64_t revision;
    LIMINE_PTR(struct limine_dtb_response *) response;
};

memory_map_t* memory_map;
boot_module_t* boot_modules;

extern void _start_limine64();

// Limine requests

struct limine_framebuffer_request l_framebuffer_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = NULL
};
__attribute__((section(".limine_reqs")))
void* __framebuffer_req = (void *)&l_framebuffer_req;

struct limine_memmap_request l_memory_map_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = NULL
};
__attribute__((section(".limine_reqs")))
void* __memory_map_req = (void *)&l_memory_map_req;

struct limine_module_request l_module_req = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0,
    .response = NULL
};
__attribute__((section(".limine_reqs")))
void* __module_req = (void *)&l_module_req;

struct limine_rsdp_request l_rsdp_req = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = NULL
};
__attribute__((section(".limine_reqs")))
void* __rsdp_req = (void *)&l_rsdp_req;

struct limine_stack_size_request l_stack_size_req = {
    .id = LIMINE_STACK_SIZE_REQUEST,
    .revision = 0,
    .response = NULL,
    .stack_size = PAGING_PAGE_SIZE * 16
};
__attribute__((section(".limine_reqs")))
void* __stack_size_req = (void *)&l_stack_size_req;

struct limine_hhdm_request l_hhdm_req = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
    .response = NULL
};
__attribute__((section(".limine_reqs")))
void* __hhdm_req = (void *)&l_hhdm_req;

struct limine_entry_point_request l_entry_req = {
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .revision = 0,
    .response = NULL,
    .entry = &_start_limine64
};
__attribute__((section(".limine_reqs")))
void* __entry_req = (void *)&l_entry_req;

struct limine_kernel_address_request l_address_req = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
    .response = NULL
};
__attribute__((section(".limine_reqs")))
void* __address_req = (void *)&l_address_req;

__attribute__((section(".limine_reqs")))
void* __final_req = NULL;

static uint64_t _kernel_physical_load;
static uint64_t _kernel_virtual_load;

void* get_kernel_load_physical() {
    return (void *)_kernel_physical_load;
}
uint64_t get_kernel_virtual_offset() {
    return _kernel_virtual_load - _kernel_physical_load;
}

// End Limine requests

boot_module_t* get_boot_module(char* name) {
    serial_terminal()->puts("Loading module \"")->puts(name)->puts("\"...\n");

    serial_terminal()->puts("Boot modules at ")->putul((uint64_t)boot_modules)->putc('\n');

    if (strlen(name) > 128)
        return NULL;

    for (uint64_t i = 0; i < boot_module_count; i++) {
        if (!strcmp(name, boot_modules[i].name))
            return &boot_modules[i];
        serial_terminal()->puts("Detected boot module: \"")->puts(boot_modules[i].name)->puts("\"\n");
    }

    serial_terminal()->puts("Could not locate requested module.\n");

    return NULL;
}

#define IOMMU_IGNORED

uint64_t limine_mmap_type_convert(uint64_t limine_type) {
    switch (limine_type) {
        case LIMINE_MEMMAP_USABLE:
            return MEMORY_MAP_FREE;
        case LIMINE_MEMMAP_RESERVED:
            return MEMORY_MAP_BUSY;
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            return MEMORY_MAP_BUSY;
        case LIMINE_MEMMAP_FRAMEBUFFER:
            return MEMORY_MAP_MMIO;
        case LIMINE_MEMMAP_ACPI_NVS:
            return MEMORY_MAP_NOUSE;
        case LIMINE_MEMMAP_BAD_MEMORY:
            return MEMORY_MAP_NOUSE;
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return MEMORY_MAP_BUSY;
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            return MEMORY_MAP_BUSY;
        default:
            return MEMORY_MAP_NOUSE;
    }
}

void limine_reinterpret();
void limine_reinterpret() {
    acpi_load_rsdp(l_rsdp_req.response->address);

    // dark blood magic
    // find two consecutive entries USED->FREE
    // use a few pages from the beginning of the free section
    // modify the memory map to protect section from pmm 
    // could break if memory map is too big in a small section
    
    // pointer to array of pointers to entries
    struct limine_memmap_entry** limine_map = l_memory_map_req.response->entries;
    uint64_t limine_map_entries = l_memory_map_req.response->entry_count;

    _kernel_physical_load = l_address_req.response->physical_base;
    _kernel_virtual_load = l_address_req.response->virtual_base;
    serial_terminal()->puts("Kernel physical load: ")->putul(_kernel_physical_load)->puts("\n");

    uint64_t map_pages = (limine_map_entries * sizeof(struct limine_memmap_entry)) / PAGING_PAGE_SIZE;  
    if (((limine_map_entries * sizeof(struct limine_memmap_entry)) % PAGING_PAGE_SIZE) != 0)
        map_pages++;

    map_pages+=2; // modules stuff too

    struct limine_memmap_entry* used_entry = NULL;
    struct limine_memmap_entry* free_entry = NULL;

    // pre-parse, check for the massive iommu entry
    serial_terminal()->puts("limine memory map:\n");
    for (uint64_t i = 0; i < limine_map_entries; i++) {
        serial_terminal()->puts("\tMemory from ")->putul(limine_map[i]->base)->puts(" for ")->putd(limine_map[i]->length / PAGING_PAGE_SIZE)->puts(" pages is ")->putd(limine_map[i]->type)->puts("\n");
    #ifdef IOMMU_IGNORED
        if (limine_map[i]->base >= MEMORY_AMD_IOMMU_BLOCK_START && limine_map[i]->base <= MEMORY_AMD_IOMMU_BLOCK_END) {
            serial_terminal()->puts("\t\tIgnoring IOMMU block...\n");
            limine_map[i]->type = LIMINE_MEMMAP_BAD_MEMORY;
        }
    #endif
    }

    for (uint64_t i = 0; i < limine_map_entries; i++) {
        if (limine_map[i+1]->type == LIMINE_MEMMAP_USABLE 
                && (limine_mmap_type_convert(limine_map[i]->type) == MEMORY_MAP_BUSY)
                && ((limine_map[i+1]->length / PAGING_PAGE_SIZE) > map_pages)) 
        {
            used_entry = (struct limine_memmap_entry *)limine_map[i];
            free_entry = (struct limine_memmap_entry *)limine_map[i+1];
            break;
        }
    }

    if (!used_entry || !free_entry) {
        serial_terminal()->puts("\nThe worst memory map edge case has came true. System failing.\n");
        __asm__ volatile ("cli");
        __asm__ volatile ("hlt");
    }

    void* candidate = (void *)free_entry->base;
    used_entry->length += (map_pages * PAGING_PAGE_SIZE);
    free_entry->base += (map_pages * PAGING_PAGE_SIZE);
    free_entry->length -= (map_pages * PAGING_PAGE_SIZE);
    // I'm imagining a possible scenario where the memory map is protected under an ACPI NVS region...

    // Now we actually have to make the new memory map
    memory_map = (memory_map_t *)candidate;
    memory_map->entries = (memory_map_entry_t *)((uint64_t)candidate + sizeof(memory_map_t));
    memory_map->entry_count = limine_map_entries;

    for (uint64_t i = 0; i < memory_map->entry_count; i++) {
        memory_map->entries[i].base = limine_map[i]->base;
        memory_map->entries[i].length = limine_map[i]->length;
        memory_map->entries[i].signal = limine_mmap_type_convert(limine_map[i]->type);
    }

    // Make first megabyte unusable
    memory_map->entries[0].base = 0x0000000000000000;
    memory_map->entries[0].length = 0x0000000000100000;
    memory_map->entries[0].signal = MEMORY_MAP_NOUSE;
    for (uint64_t i = 1; i < memory_map->entry_count; i++) {
        if (memory_map->entries[i].base < 0x0000000000100000) {
            if (memory_map->entries[i].base + memory_map->entries[i].length < 0x0000000000100000) {
                memory_map->entries[i].length = 0;
                memory_map->entries[i].signal = MEMORY_MAP_NOUSE;
            }
            else {
                memory_map->entries[i].length -= (0x0000000000100000 - memory_map->entries[i].base);
                memory_map->entries[i].base = 0x0000000000100000;
            }
        }
        else break;
    }

    // assumes 32-bit framebuffer...
    framebuffer.frame_addr = (uint64_t)l_framebuffer_req.response->framebuffers[0]->address;
    framebuffer.frame_height = l_framebuffer_req.response->framebuffers[0]->height;
    framebuffer.frame_width = l_framebuffer_req.response->framebuffers[0]->width;
    framebuffer.frame_pitch = l_framebuffer_req.response->framebuffers[0]->pitch;
    framebuffer.frame_bpp = l_framebuffer_req.response->framebuffers[0]->bpp;

    serial_terminal()->puts("\nhhdm offset: ")->putul(l_hhdm_req.response->offset)->puts("\n");

    // Hope modules aren't tooo many (although I ultimately have control over this)
    boot_modules = (boot_module_t *)((uint64_t)candidate + ((map_pages - 2) * PAGING_PAGE_SIZE));
    boot_module_count = l_module_req.response->module_count;

    for (uint64_t i = 0; i < boot_module_count; i++) {     
        boot_modules[i].name = l_module_req.response->modules[i]->cmdline;
        boot_modules[i].virt = (uint64_t)l_module_req.response->modules[i]->address;
        boot_modules[i].phys = (uint64_t)l_module_req.response->modules[i]->address & 0xFFFFFFFF;
        boot_modules[i].size = l_module_req.response->modules[i]->size;
    }
}

framebuffer_info_t framebuffer;
uint64_t boot_module_count;
