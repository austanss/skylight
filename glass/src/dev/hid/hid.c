#include "dev/hid/kbd.h"
#include "dev/hid/ps2/ps2_kbd.h"

void hid_enable_keyboard_interrupts() {
    ps2_kbd_init();
}
