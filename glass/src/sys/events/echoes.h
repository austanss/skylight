#pragma once
#include <stdint.h>

#define SCHEDULER_TICK_EVENT_ID 0x0001

typedef struct {
    uint64_t    id;
    uint64_t    data_length;
} echoes_packet_t;

uint64_t    echoes_establish_broadcaster(uint64_t id);
void        echoes_dissolve_broadcaster(uint64_t id);
void        echoes_broadcast_event(echoes_packet_t* packet);
uint64_t    echoes_subscribe_event(uint64_t id, void (*handler)(echoes_packet_t *));
void        echoes_unsubscribe_event(uint64_t hook);
