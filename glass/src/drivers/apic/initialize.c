#include "../acpi/tables/madt.h"
#include "../8259/pic.h"
#include "../uart/serial.h"
#include "cpu/interrupts/idt.h"
#include "cpu/tss/tss.h"
#include "debug.h"
#include "lapic.h"
#include <cpuid.h>
#include <stdbool.h>

static uint8_t default_apic_ist = 0;

void apic_initialize(void);
void apic_initialize() {
    acpi_madt_header_t* madt = ACPI_MADT_GET();

    pic_disable();

    uint32_t erax, erbx, ercx, erdx;
    __cpuid(1, erax, erbx, ercx, erdx);

    bool apic = (erdx >> 9) & 1;

    if (!apic) {
        serial_terminal()->puts("ERROR: Hardware unsupported (cpuid.01h.edx.9): APIC missing; get a new computer");
        __asm__ volatile ("cli; hlt");
    }

    apic_local_set_base((void *)(uintptr_t)madt->lapic_address);

    default_apic_ist = tss_add_stack(0);

    apic_local_write(APIC_LOCAL_REGISTER_SPURIOUS_INT_VECTOR, 0x100 | apic_local_read(APIC_LOCAL_REGISTER_SPURIOUS_INT_VECTOR));

    uint32_t lapic_id = apic_local_read(APIC_LOCAL_REGISTER_ID);

    const uint8_t nmi = 0x04 & 0x07;

    acpi_madt_record_t* madt_record = (acpi_madt_record_t *)((uintptr_t)madt + sizeof(acpi_madt_header_t));

    for (;;) {
        if (((uintptr_t)madt_record - (uintptr_t)madt) >= madt->common.length)
            break;

        madt_record = (acpi_madt_record_t *)((uintptr_t)madt_record + madt_record->length);

        if (madt_record->type == ACPI_MADT_RECORD_TYPE_NMI) {
            acpi_madt_record_nmi_t* lint01_record = (acpi_madt_record_nmi_t *)madt_record;
            if (lint01_record->processor_id != lapic_id && lint01_record->processor_id != 0xff)
                continue;
            
            apic_local_register_t lvt = lint01_record->lint_no == 0 ? APIC_LOCAL_REGISTER_LVT_LINT0 : APIC_LOCAL_REGISTER_LVT_LINT1;

            uint32_t entry = 0x00000000;
            entry |= (nmi & 0x07) << 8;

            bool level_triggered = !!(lint01_record->flags & ACPI_MADT_RECORD_ISO_NMI_FLAG_LEVEL_TRIGGERED);
            bool active_low = !!(lint01_record->flags & ACPI_MADT_RECORD_ISO_NMI_FLAG_ACTIVE_LOW);

            entry |= (uint32_t)level_triggered << 13;
            entry |= (uint32_t)active_low << 15;

            apic_local_write(lvt, entry);
        }
    }

    apic_local_send_ipi(0, 0x20);
}
