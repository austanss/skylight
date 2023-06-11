#include <stdlib.h>
#include <string.h>
#include "cpu/interrupts/idt.h"
#include "cpu/gdt/gdt.h"
#include "cpu/tss/tss.h"
#include "mm/paging/paging.h"
#include "misc/conv.h"
#include "dev/uart/uartsh.h"

PAGING_PAGE_ALIGNED
idt_desc_t __idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtr;

static bool vectors[IDT_MAX_DESCRIPTORS];

uint64_t __routine_handlers[IDT_MAX_DESCRIPTORS];

extern uint64_t isr_stub_table[];

void idt_install_irq_handler(uint8_t vector, void* handler) {
    __routine_handlers[vector] = (uint64_t)handler;
    idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXTERNAL, TSS_IST_ROUTINE);
}

void idt_set_descriptor(uint8_t vector, uintptr_t isr, uint8_t flags, uint8_t ist) {
    idt_desc_t* descriptor = &__idt[vector];

    descriptor->base_low       = isr & 0xFFFF;
    descriptor->cs             = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist            = ist;
    descriptor->attributes     = flags;
    descriptor->base_mid       = (isr >> 16) & 0xFFFF;
    descriptor->base_high      = (isr >> 32) & 0xFFFFFFFF;
    descriptor->rsv0           = 0;
}

void idt_assemble() {
    idtr.base = (uintptr_t)&__idt[0];
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < IDT_CPU_EXCEPTION_COUNT; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXCEPTION, TSS_IST_EXCEPTION);
        vectors[vector] = true;
    }

    idt_reload(&idtr);
}

uint8_t idt_allocate_vector() {
    for (unsigned int i = 0; i < IDT_MAX_DESCRIPTORS; i++) {
        if (!vectors[i]) {
            vectors[i] = true;
            return (uint8_t)i;
        }
    }

    return 0;
}

void idt_free_vector(uint8_t vector) {
    idt_set_descriptor(vector, 0, 0, 0);
    __routine_handlers[vector] = 0;
    vectors[vector] = false;
}

void __idt_dump() {
    serial_print_quiet("\nIDT dump:\n");
    for (uint16_t vector = 0; vector < IDT_MAX_DESCRIPTORS; vector++) {
        if (vectors[vector]) {
            idt_desc_t* descriptor = &__idt[vector];
            uint64_t isr = (uint64_t)descriptor->base_low | ((uint64_t)descriptor->base_mid << 16) | ((uint64_t)descriptor->base_high << 32);
            uint8_t flags = descriptor->attributes;
            uint8_t ist = descriptor->ist;
            uint16_t cs = descriptor->cs;
            char itoa_buffer[67];
            memset(itoa_buffer, '\0', 67);
            serial_print_quiet(utoa(vector, itoa_buffer, 10));
            serial_print_quiet(") isr:");
            if (vector < 32)
                serial_print_quiet(utoa(isr, itoa_buffer, 16));
            else if (vector >= 32)
                serial_print_quiet(utoa(__routine_handlers[vector], itoa_buffer, 16));
            serial_print_quiet(", flags: ");
            serial_print_quiet(utoa(flags, itoa_buffer, 16));
            serial_print_quiet(", cs: ");
            serial_print_quiet(utoa(cs, itoa_buffer, 16));
            serial_print_quiet(" ... \n");
        }
    }
}
