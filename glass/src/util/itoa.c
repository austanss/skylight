#include <stdint.h>
#include <stdlib.h>

char* itoa(int value, unsigned int base) {
    static char buffer[64] = {};
    static const char convert[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    
    int i = 60;

    if (value < 0) {
        buffer[i] = '-';
        i--;
    }

    for(; value && i; i++, value /= base) {
        buffer[i] = convert[value % base];
    }

    return &buffer[++i];
}