#include <stdint.h>
#include <stdlib.h>

char* itoa(long int value, unsigned int base) {
	static char buf[64] = {0};

	int i = 60;

	for(; value && i ; --i, value /= base)

		buf[i] = "0123456789abcdef"[value % base];

	return &buf[i+1];
}