#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "dev/uart/uartsh.h"
#include "dev/io.h"
#include "dev/apic/ioapic.h"

static uint8_t available_com_ports[8] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

static void serial_write(const char* s);

static bool _input_masked;
static uint8_t input_com_gsi;
static uint8_t input_idt_vector;
static uint16_t input_port;
static uint16_t output_port;

static uint16_t __com_ports[9] = {
    0,
    SERIAL_COM1_PORT,
    SERIAL_COM2_PORT,
    SERIAL_COM3_PORT,
    SERIAL_COM4_PORT,
    SERIAL_COM5_PORT,
    SERIAL_COM6_PORT,
    SERIAL_COM7_PORT,
    SERIAL_COM8_PORT
};

static bool serial_check_port(uint16_t port) {
    if (port == 0 || port > 8) return false;
    outb(__com_ports[port] + COM_REGISTER_SCRATCH, 0xAF);   // probe by writing to and reading back scratchr register
    uint8_t test = inb(__com_ports[port] + 0x07);
    return (test == 0xAF);
}

static void serial_configure_port(uint16_t port) {
    if (!serial_check_port(port)) return;

}

static void serial_select_input_port(uint16_t port) {
    if (port < 1 || port > 2) return;
    input_port = port;
    input_com_gsi = apic_io_get_gsi((port == 1) ? SERIAL_COM1_IRQ : SERIAL_COM2_IRQ);
}

static void serial_select_output_port(uint16_t port) {
    if (port < 1 || port > 8) return;
    output_port = port;
}

void serial_set_input_masked(bool masked) {
    _input_masked = masked;
    if (input_com_gsi != 0) {
        if (_input_masked)
            apic_io_mask_irq(input_com_gsi);
        else
            apic_io_unmask_irq(input_com_gsi);
    }
}

void serial_console_enable() {
    _input_masked = true;
    input_idt_vector = 0;
    input_com_gsi = 0;
    input_port = 0;
    output_port = 0;
    for (uint64_t i = 1; i < 9; i++) {
        if (serial_check_port(i)) {
            available_com_ports[i] = true;
            serial_configure_port(i);
        }
    }
    for (uint64_t i = 1; i <= 2; i++) {
        if (available_com_ports[i]) {
            serial_select_input_port(i);
            break;
        }
    }
    for (uint64_t i = 1; i <= 8; i++) {
        if (available_com_ports[i]) {
            serial_select_output_port(i);
            break;
        }
    }
    serial_set_input_masked(false);
    serial_write("[debug] serial console enabled!\n");
}

static void serial_write(const char* s) {
    if (output_port == 0) return;
    char* __s = (char*)s;
    while (*__s != '\0') {
        outb(__com_ports[output_port] + COM_REGISTER_DATA, *__s);
        __s++;
    }
}

void serial_print_error(const char* error) {
    serial_write("[ERROR] ");
    serial_write(error);
}
