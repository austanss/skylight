#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "lib/glass.h"
#include "tty/tty.h"

int main(void) {
    tty_start();
    tty_write("\n[frame] reached process entry.\n");
    tty_write("[frame] acquiring framebuffer lock... done.\n");
    tty_write("skylight v0.3: \"sunrise\" (untracked build)\n\n");
    tty_write("< keyboard disabled (driver unavailable) > ");
    while (true);
    tty_kill();
    return -1;
}
