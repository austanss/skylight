#include <stdint.h>
#include <stddef.h>
#include <string.h>

size_t strlen(const char * __s) {
    const char * a = __s;
    for (; *__s; __s++);
    return __s-a;
}