#include "isr.h"
#include "dev/uart/serial.h"

void isr_exception_handler(isr_xframe_t* frame);
void isr_exception_handler(isr_xframe_t* frame) {
    serial_terminal()->puts("\n\nerror: cpu exception ")->putul(frame->base_frame.vector)->puts(" @ ")->putul(frame->base_frame.rip);
    serial_terminal()->puts("\nerror code: ")->putul((uint64_t)frame->base_frame.error_code);
    __asm__ volatile ("cli; hlt");
}
