#include "isr.h"
#include "dev/uart/serial.h"
#include "proc/task/task.h"

void isr_exception_handler(isr_xframe_t* frame);
void isr_exception_handler(isr_xframe_t* frame) {
    serial_terminal()->puts("\n\nerror: cpu exception ")->putul(frame->base_frame.vector)->puts(" @ ")->putul(frame->base_frame.rip);
    serial_terminal()->puts("\nerror code: ")->putul((uint64_t)frame->base_frame.error_code);

    serial_terminal()->puts("\nrax: ")->putul(frame->general_registers.rax)->puts(", rbx: ")->putul(frame->general_registers.rbx)->puts(", rcx: ")->putul(frame->general_registers.rcx)->putc('\n');
    serial_terminal()->puts("rdx: ")->putul(frame->general_registers.rdx)->puts(", rdi: ")->putul(frame->general_registers.rdi)->puts(", rsi: ")->putul(frame->general_registers.rsi)->putc('\n');
    serial_terminal()->puts("cr0: ")->putul(frame->control_registers.cr0)->puts(", cr2: ")->putul(frame->control_registers.cr2)->putc('\n');
    serial_terminal()->puts("cr3: ")->putul(frame->control_registers.cr3)->puts(", cr4: ")->putul(frame->control_registers.cr4)->putc('\n');

    serial_terminal()->puts("rsp: ")->putul(frame->base_frame.rsp)->puts(", rbp: ")->putul(frame->base_frame.rbp)->putc('\n');

    __asm__ volatile ("cli; hlt");
}

