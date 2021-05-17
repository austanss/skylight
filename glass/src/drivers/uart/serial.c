#include "serial.h"
#include "../io.h"

serial_terminal_t this;

void serial_message(const char* message) {
    while (*message) {
        outb(0x3F8, *message);
        message++;
    }
}

serial_terminal_t* serial_terminal() {
    return &this;
}

serial_terminal_t* puts(const char* s) {
    serial_message(s);
    return &this;
}

serial_terminal_t* putc(const char c) {
    outb(0x3F8, c);
    return &this;
}

serial_terminal_t this = {
    puts,
    putc
};