#include "isr.h"

void isr_exception_handler(isr_xframe_t* frame) {
    asm volatile ("cli; hlt");
}