#include <stdint.h>
#include <stddef.h>
#include <string.h>

size_t strlen(const char * __s) {
    size_t len;
    for (len = 0; *__s; len++, *__s++);
    return len;
}