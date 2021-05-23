#include <string.h>

int strcmp(const char* __s1, const char* __s2) {
	const signed char* __ss1 = (const signed char *)__s1;
	const signed char* __ss2 = (const signed char *)__s2;
    for (; *__ss1; __ss1++) {
		if (*__ss1 != *__ss2)
			return (signed int)(*__s1 - *__s2);
		__ss2++;
	}
	return 0;
}