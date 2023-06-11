#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "dev/uart/uartsh.h"
#include "dev/io.h"
#include "cpu/interrupts/idt.h"
#include "dev/apic/ioapic.h"
#include "dev/apic/lapic.h"
#include "mm/pmm/pmm.h"

#define SHELL_PROMPT "glass #> "
#define PROMPT_LENGTH 9

static void uart_shell_write(const char* s);

#define UNSUPPORTED_LF 0x0A
#define SUPPORTED_CR 0x0D

static bool prompt_placed;

static uint16_t output_port = 0;
extern uint16_t __com_port_addrs[9];

static uint8_t* command_buffer;
static uint64_t command_buffer_index;

static void __uart_internal_write(const char* s, uint16_t port) {
    if (port == 0) return;
    char* __s = (char*)s;
    while (*__s != '\0') {
        outb(__com_port_addrs[port] + COM_REGISTER_DATA, *__s);
        __s++;
    }
}

// Called before writing to serial output
void uart_shell_cancel_prompt() {
    if (!prompt_placed) return;
    char s[PROMPT_LENGTH + 1];
    memset(s, '\b', PROMPT_LENGTH);
    s[PROMPT_LENGTH] = '\0';
    __uart_internal_write(s, output_port);
    prompt_placed = false;
}
// Called after writing to serial output
void uart_shell_replace_prompt() {
    if (prompt_placed) uart_shell_cancel_prompt();
    __uart_internal_write(SHELL_PROMPT, output_port);
    prompt_placed = true;
}

static void uart_shell_write(const char* s) {
    uart_shell_cancel_prompt();
    if (output_port == 0) return;
    __uart_internal_write(s, output_port);
    uart_shell_replace_prompt();
}

static void __shell_command_help();
static void __shell_command_ping();
static void __shell_command_dump_idt();
static void __shell_command_dump_gdt();
static void __shell_command_dump_tss();
static void __shell_command_dump_proc();
static void __shell_command_mem_map();
static void __shell_command_uart_info();
static void __shell_command_pci_info();

#define NUMBER_OF_COMMANDS 9

void(*__shell_command_handlers[NUMBER_OF_COMMANDS])() = {
    __shell_command_help,
    __shell_command_ping,
    __shell_command_dump_idt,
    __shell_command_dump_gdt,
    __shell_command_dump_tss,
    __shell_command_dump_proc,
    __shell_command_mem_map,
    __shell_command_uart_info,
    __shell_command_pci_info
};

const char* __uart_shell_commands[NUMBER_OF_COMMANDS] = {
    "help",
    "ping",
    "idt",
    "gdt",
    "tss",
    "lsp",
    "mem",
    "uart"
};

static void uart_shell_command_execute(uint16_t command_no) {
    if (command_no >= NUMBER_OF_COMMANDS) return;
    __shell_command_handlers[command_no]();
}

static void uart_shell_clear_buffer() {
    memset((void *)command_buffer, 0, PAGING_PAGE_SIZE);
    command_buffer_index = 0;
}

static void uart_shell_command_finalize() {
    command_buffer[command_buffer_index] = '\0';
    char* command = (char*)command_buffer;
    for (uint16_t i = 0; i < NUMBER_OF_COMMANDS; i++) {
        if (strcmp(command, __uart_shell_commands[i]) == 0) {
            uart_shell_command_execute(i);
            prompt_placed = false;
            uart_shell_replace_prompt();
            uart_shell_clear_buffer();
            return;
        }
    }
    uart_shell_write("Unknown command...\n");
    prompt_placed = false;
    uart_shell_replace_prompt();
    uart_shell_clear_buffer();
}

static void uart_command_buffer_append(uint8_t c) {
    command_buffer[command_buffer_index++] = c;
}

static bool previous_was_cr = false;
void __uart_shell_handle_character(uint8_t c) {
    if (c == UNSUPPORTED_LF || c == SUPPORTED_CR) {
        __uart_internal_write("\r\n", output_port);
        c = SUPPORTED_CR;
    }
    if (c == SUPPORTED_CR && !previous_was_cr) {
        uart_shell_command_finalize();
        previous_was_cr = true;
        return;
    }
    previous_was_cr = false;
    if ((c < 0x20 || c > 0x7E) && c != SUPPORTED_CR) return;
    uint8_t cs[2] = {c == SUPPORTED_CR ? UNSUPPORTED_LF : c, '\0'};
    __uart_internal_write((const char *)&cs[0], output_port);
    uart_command_buffer_append(c);
}

void uart_shell_start(uint16_t out_port) {
    output_port = out_port;
    previous_was_cr = false;
    command_buffer = (uint8_t*)pmm_alloc_page();
    command_buffer_index = 0;
    memset(command_buffer, 0, PAGING_PAGE_SIZE);
    prompt_placed = false;
    uart_shell_replace_prompt();
}

void serial_print_quiet(const char* s) {
    __uart_internal_write(s, output_port);    
}

static void __shell_command_help() {
    __uart_internal_write("You are using the \"glass\" kernel debug shell through the serial port. Available commands:\n\n", output_port);
    __uart_internal_write("help: print this message\n", output_port);
    __uart_internal_write("ping: print \"pong\"\n", output_port);
    __uart_internal_write("idt: print dump of idt structure\n", output_port);
    __uart_internal_write("gdt: print dump of gdt structure\n", output_port);
    __uart_internal_write("tss: print dump of tss structure\n", output_port);
    __uart_internal_write("lsp: print dump of current processes\n", output_port);
    __uart_internal_write("mem: print info on memory state\n", output_port);
    __uart_internal_write("uart: print info on uart (serial) state\n", output_port);
    __uart_internal_write("pci: print list of pci functions\n", output_port);
}

static void __shell_command_ping() {
    __uart_internal_write("pong\n", output_port);
}

extern void __idt_dump();
static void __shell_command_dump_idt() {
    __idt_dump();
}

extern void __gdt_dump();
static void __shell_command_dump_gdt() {
    __gdt_dump();
}

extern void __tss_dump();
static void __shell_command_dump_tss() {
    __tss_dump();
}

extern void __proc_dump();
static void __shell_command_dump_proc() {
    __proc_dump();
}

extern void __pmm_dump();
static void __shell_command_mem_map() {
    __pmm_dump();
}

extern void __uart_dump();
static void __shell_command_uart_info() {
    __uart_dump();
}

extern void __pci_dump();
static void __shell_command_pci_info() {
    __pci_dump();
}
