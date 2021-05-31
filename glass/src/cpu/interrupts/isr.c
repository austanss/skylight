#include "isr.h"
#include "dev/uart/serial.h"

void isr_exception_handler(isr_xframe_t* frame);
void isr_exception_handler(isr_xframe_t* frame) {
    serial_terminal()->puts("error: cpu exception ")->putul(frame->base_frame.vector);
    __asm__ volatile ("cli; hlt");
}
