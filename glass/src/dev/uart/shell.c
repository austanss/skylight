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

void(*__shell_command_handlers[3])() = {
    __shell_command_help,
    __shell_command_ping,
    __shell_command_dump_idt
};

static void uart_shell_command_execute(uint16_t command_no) {
    __shell_command_handlers[command_no]();
}

#define NUMBER_OF_COMMANDS 3
const char* __uart_shell_commands[NUMBER_OF_COMMANDS] = {
    "help",
    "ping",
    "idt"
};

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

void __uart_shell_handle_character(uint8_t c) {
    if (c == UNSUPPORTED_LF)
        c = SUPPORTED_CR;
    if ((c < 0x20 || c > 0x7E) && c != SUPPORTED_CR) return;
    uint8_t cs[2] = {c == SUPPORTED_CR ? UNSUPPORTED_LF : c, '\0'};
    __uart_internal_write((const char *)&cs[0], output_port);
    if (c == SUPPORTED_CR)
        uart_shell_command_finalize();
    else
        uart_command_buffer_append(c);
}

void uart_shell_start(uint16_t out_port) {
    output_port = out_port;
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
    __uart_internal_write("help: receive this message\n", output_port);
    __uart_internal_write("ping: receive \"pong\"\n", output_port);
    __uart_internal_write("idt: receive idt dump\n", output_port);
}

static void __shell_command_ping() {
    __uart_internal_write("pong\n", output_port);
}

extern void __idt_dump();
static void __shell_command_dump_idt() {
    __idt_dump();
}
