#include "isr.h"
#include "proc/task/task.h"
#include "dev/uart/uartsh.h"

void __print_interrupt_stacktrace(isr_xframe_t* ctx) {
    serial_print_quiet("Attempted stacktrace:\n");
}

void isr_exception_handler(isr_xframe_t* frame);
void isr_exception_handler(isr_xframe_t* frame) {
    serial_print_error("FATAL CPU Xception! System halt!\n");
    serial_set_input_masked(true);
    while (true) {
        __asm__ volatile ("cli; hlt");
    }
}
