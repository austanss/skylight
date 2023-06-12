#include "isr.h"
#include "proc/task/task.h"
#include "symbols.h"
#include "dev/uart/uartsh.h"
#include "misc/conv.h"
#include "boot/protocol.h"

void __print_interrupt_stacktrace(isr_xframe_t* ctx) {
    char itoa_buffer[67];
    serial_print_quiet("Attempted stacktrace: interrupt during \n");
    uint64_t rbp = ctx->base_frame.rbp;
    void* rip = (void *)ctx->base_frame.rip;
    // Assumes unrelocated
    while (true) {
        symbol_t* symbol = NULL;
        // Symbols are sorted by address lo->hi
        for (int i = 0; __symbol_tab[i].addr != (void *)0xFFFFFFFFFFFFFFFF; i++) {
            if ((uint64_t)rip >= (uint64_t)__symbol_tab[i].addr && (uint64_t)rip < (uint64_t)__symbol_tab[i+1].addr) {
                symbol = (symbol_t *)&__symbol_tab[i];
                serial_print_quiet("\t");
                serial_print_quiet(utoa((uint64_t)rip, itoa_buffer, 16));
                serial_print_quiet(" <");
                serial_print_quiet(symbol->name);
                serial_print_quiet(">\n");
                break;
            }
        }
        if (symbol == NULL) {
            serial_print_quiet("\t");
            serial_print_quiet(utoa((uint64_t)rip, itoa_buffer, 16));
            serial_print_quiet(" <unknown>\n");
        }
        if ((uint64_t)rip != ctx->base_frame.rip)
            rbp = *(uint64_t*)rbp;
        if (rbp == 0) return;
        rip = (void *)(*(uint64_t*)(rbp + 8));
    }
}

void isr_exception_handler(isr_xframe_t* frame);
void isr_exception_handler(isr_xframe_t* frame) {
    serial_set_input_masked(true);
    serial_print_error("FATAL CPU Xception! System halt!\n");
    __print_interrupt_stacktrace(frame);
    while (true) {
        __asm__ volatile ("cli; hlt");
    }
}
