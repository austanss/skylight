#include "isr.h"

void isr_exception_handler(isr_xframe_t* frame) {
    __asm__ volatile ("cli; hlt");
}
