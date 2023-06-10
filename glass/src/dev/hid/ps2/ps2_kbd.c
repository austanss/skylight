#include <stdlib.h>
#include <string.h>
#include "dev/hid/kbd.h"
#include "dev/hid/ps2/ps2_kbd.h"
#include "dev/io.h"
#include "cpu/interrupts/idt.h"
#include "dev/apic/ioapic.h"
#include "dev/apic/lapic.h"

#define SCANCODE_SET_1 0x01 // old and compatible scan code set
#define SCANCODE_SET_2 0x02 // default and universal scan code set
#define SCANCODE_SET_3 0x03 // advanced scan code set
#define SCANCODES_SUPPORTED 0x02 // selected scancode set 2

#define PS2_NO_OPERAND 0xff

static uint8_t ps2_kbd_vector   = 0x00;
static uint8_t ps2_kbd_gsi      = 0x00;
static uint8_t ps2_kbd_scan_set = 0x00;

static bool _capslock;
static bool _numlock;
static bool _scrolllock;
static bool _ctrldown;
static bool _altdown;
static bool _fndown;
static bool _shiftdown;

static bool _ismasked;

static bool ps2_test_responsivity() {
    uint8_t status = 0;
    outb(PS2_KBD_DATA_PORT, 0xEE);
    status = inb(PS2_KBD_DATA_PORT);
    return status == 0xEE;
}

static void ps2_buffer_wait() {
    uint8_t status = inb(PS2_KBD_STATUS_REG);
    while (status & 0x02) {
        status = inb(PS2_KBD_STATUS_REG);
    }
}

static bool ps2_kbd_send_command(uint8_t command, uint8_t operand) {
    if (!ps2_test_responsivity()) return false;
    uint8_t status = inb(PS2_KBD_DATA_PORT);
    if (status != PS2_KBD_STATUS_ERROR) {
        outb(PS2_KBD_DATA_PORT, command);
        ps2_buffer_wait();
        if (operand != PS2_NO_OPERAND)
            outb(PS2_KBD_DATA_PORT, operand);
        
    }
    uint64_t timeout = 0;
    while (status != PS2_KBD_STATUS_ACK) {
        if (status == PS2_KBD_STATUS_RESEND && timeout < 1) {
            outb(PS2_KBD_COMMAND_PORT, command);
            timeout++;
        }
        if (status == PS2_KBD_STATUS_ERROR) {
            return false;
        }
        status = inb(PS2_KBD_DATA_PORT);
    }
    return true;
}

// Only works with scan code set two
static bool ps2_kbd_needs_next_byte(uint8_t* scancode, size_t bytes) {
    if (bytes == 1) {
        if (scancode[0] == 0xE0) return true; // many different keys
        if (scancode[0] == 0xF0) return true; // many different keys
        if (scancode[0] == 0xE1) return true; // pause
    }
    if (bytes == 2) {
        if (scancode[1] == 0xF0) return true; // many different keys
        if (scancode[0] == 0xE0 && scancode[1] == 0x12) return true; // print screen
        if (scancode[0] == 0xE1 && scancode[1] == 0x14) return true; // pause
    }
    if (bytes == 3) {
        if (scancode[2] == 0x77) return true; // pause
        if (scancode[2] == 0x7C) return true; // print screen
        if (scancode[2] == 0xE0) return true; // print screen
    }
    if (bytes == 4) {
        if (scancode[3] == 0xE0) return true; // print screen
        if (scancode[3] == 0xE1) return true; // pause
    }
    if (bytes == 5) {
        if (scancode[4] == 0xF0) return true; // pause and print screen
    }
    if (bytes == 6) {
        if (scancode[5] == 0x14) return true; // pause
    }
    if (bytes == 7) {
        if (scancode[6] == 0xF0) return true; // pause
    }
    return false;
}

static bool ps2_kbd_validate_state() {
    return (ps2_kbd_scan_set == SCANCODES_SUPPORTED) && !_ismasked;
}

// Returns number of bytes in scan code
size_t ps2_kbd_get_scancode(uint8_t* scancode_out) {
    if (!ps2_kbd_validate_state()) return 0;
    uint64_t bytes;
    for (bytes = 0; bytes < 8; bytes++) {
        scancode_out[bytes] = inb(PS2_KBD_DATA_PORT);
        if (!ps2_kbd_needs_next_byte(scancode_out, (bytes + 1))) return ++bytes;
    }
    return 0;
}

void ps2_kbd_sync_leds() {
    uint8_t leds = 0x00;
    if (_scrolllock) leds |= (1 << 0);
    if (_numlock) leds |= (1 << 1);
    if (_capslock) leds |= (1 << 2);
    ps2_kbd_send_command(PS2_FUNC_SET_LEDS, leds);
}

void ps2_kbd_handle_scancode(uint8_t* scancode, size_t bytes) {
    uint64_t scancode_rendered = 0x00;
    if (bytes == 0) return;
    for (uint64_t i = 0; i < bytes; i++) {
        scancode_rendered <<= 8;
        scancode_rendered |= scancode[i];
    }
}

void __kbd_ps2_irq_handler() {
    if (!ps2_kbd_validate_state()) {
        if (_ismasked) {
            ps2_kbd_set_mask(PS2_KBD_MASKED);
        }
        else
            return;
    }
    uint8_t* scancode = (uint8_t *)malloc(8);
    memset(&scancode[0], 0x00, 8);
    uint64_t scancode_size = ps2_kbd_get_scancode(&scancode[0]);
    if (scancode_size == 0) return;
    ps2_kbd_handle_scancode(&scancode[0], scancode_size);
    ps2_kbd_sync_leds();
    free(scancode);

    apic_local_send_eoi();
}

uint8_t _scancode_set_id(uint8_t scancode_set) {
    switch (scancode_set) {
        case 1: return 0x43;
        case 2: return 0x41;
        case 3: return 0x3f;
        default: return 0x00;
    }
}

void ps2_kbd_disable_set_translation() {
    uint8_t config_byte = 0;
    ps2_buffer_wait();
    outb(PS2_KBD_COMMAND_PORT, 0x20); // read config byte
    ps2_buffer_wait();
    config_byte = inb(PS2_KBD_DATA_PORT);
    config_byte &= ~(1 << 6);
    outb(PS2_KBD_COMMAND_PORT, 0x60); // write config byte
    outb(PS2_KBD_DATA_PORT, config_byte);
    ps2_buffer_wait();
}

void ps2_kbd_set_scancodes(uint8_t scancode_set) {
    ps2_kbd_disable_set_translation();
    ps2_kbd_scan_set = scancode_set;
    if (scancode_set == 0) return;
    ps2_kbd_send_command(PS2_FUNC_SCANSET, scancode_set);
    ps2_buffer_wait();
    outb(PS2_KBD_DATA_PORT, PS2_FUNC_SCANSET);
    ps2_buffer_wait();
    outb(PS2_KBD_DATA_PORT, 0x00);
    ps2_buffer_wait();
    inb(PS2_KBD_DATA_PORT); // discard ack
    uint8_t set = inb(PS2_KBD_DATA_PORT);
    if (set != scancode_set)
        ps2_kbd_set_scancodes(scancode_set);
}

void ps2_kbd_reset_state() {
    _capslock = false;
    _numlock = false;
    _scrolllock = false;
    _ctrldown = false;
    _altdown = false;
    _fndown = false;
    _shiftdown = false;
    ps2_kbd_sync_leds();
}

void ps2_kbd_init() {
    if (ps2_kbd_vector != 0x00) return;
    
    outb(PS2_KBD_COMMAND_PORT, 0xAE);
    ps2_kbd_set_scancodes(SCANCODE_SET_2);
    ps2_kbd_reset_state();

    ps2_kbd_vector = idt_allocate_vector();
    idt_install_irq_handler(ps2_kbd_vector, &__kbd_ps2_irq_handler);
    ps2_kbd_gsi = apic_io_get_gsi(PS2_KBD_IRQ_LINE);
    apic_io_redirect_irq(ps2_kbd_gsi, ps2_kbd_vector, false, false);
    ps2_kbd_set_mask(PS2_KBD_MASK_OFF);
}

void ps2_kbd_set_mask(bool mask) {
    if (mask)
        apic_io_mask_irq(ps2_kbd_gsi);
    else
        apic_io_unmask_irq(ps2_kbd_gsi);
    _ismasked = mask;
}

void ps2_kbd_kill() {
    if (ps2_kbd_vector == 0x00) return;
    ps2_kbd_set_mask(PS2_KBD_MASKED);
    apic_io_redirect_irq(ps2_kbd_gsi, 0, false, false);
    idt_set_descriptor(ps2_kbd_vector, 0x00, 0x00, 0x00);
    idt_free_vector(ps2_kbd_vector);
    ps2_kbd_vector = 0x00;
    ps2_kbd_gsi = 0x00;
    outb(PS2_KBD_COMMAND_PORT, 0xAD);
}
