#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "lib/glass.h"
#include "tty/tty.h"

int main(void) {
    void* page = pmap(PMAP_VIRT_DEFAULT);
    tty_start();
    while (true);
    tty_kill();
    return -1;
}
