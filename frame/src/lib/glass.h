#pragma once
#include <stdint.h>

#define FIELD_TOTAL_RAM         000
#define FIELD_FREE_RAM          001
#define FIELD_DISPLAY_WIDTH     002
#define FIELD_DISPLAY_HEIGHT    003
#define FIELD_DISPLAY_BPP       004

uint64_t rdinfo(uint64_t field);

#define PMAP_VIRT_DEFAULT       000

void* pmap(void* virt);
void punmap(void* virt);
