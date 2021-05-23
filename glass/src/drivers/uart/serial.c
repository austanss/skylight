#include <stdbool.h>
#include "serial.h"
#include "../io.h"

serial_terminal_t singleton_terminal_instance;

void serial_message(const char* message) {
    while (!!*message) {
        outb(0x3F8, *message);
        message++;
    }
}

serial_terminal_t* serial_terminal() {
    return &singleton_terminal_instance;
}

serial_terminal_t* puts(const char* s) {
    serial_message(s);
    return &singleton_terminal_instance;
}

serial_terminal_t* putc(const char c) {
    outb(0x3F8, c);
    return &singleton_terminal_instance;
}

serial_terminal_t* putul(uint64_t ul) {

    if (!ul) {
        serial_message("0x0");
        return &singleton_terminal_instance;
    }

    char buffer[19];
    buffer[18] = '\0';

    int index = 17;

    for (; index > 1; index--) {
        if (!ul)
            break;

        unsigned int remainder = ul % 16;
        ul /= 16;
        char character = "0123456789abcdef"[remainder];
        buffer[index] = character;
    }

    buffer[index] = 'x';
    index--;
    buffer[index] = '0';

    serial_message(&buffer[index]);
    
    return &singleton_terminal_instance;
}

serial_terminal_t* putd(int64_t d) {

    if (!d) {
        serial_message("0");
        return &singleton_terminal_instance;
    }

    char buffer[22];
    buffer[21] = '\0';

    bool negative = d < 0;

    int index = 20;

    for (; index > 0; index--) {
        if (!d)
            break;

        unsigned int remainder = d % 10;
        d /= 10;
        char character = "0123456789"[remainder];
        buffer[index] = character;
    }

    index++;

    if (negative) {
        index--;
        buffer[index] = '-';
    }

    serial_message(&buffer[index]);

    return &singleton_terminal_instance;
}

serial_terminal_t singleton_terminal_instance = {
    puts,
    putc,
    putul,
    putd
};