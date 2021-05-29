#include "echoes.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct _echoes_subscriber_node echoes_subscriber_node_t;
struct _echoes_subscriber_node {
    uint64_t                    hook;
    void                        (*handler)(echoes_packet_t *);
    echoes_subscriber_node_t*   prev;
    echoes_subscriber_node_t*   next;
};

typedef struct _echoes_broadcaster_node echoes_broadcaster_node_t;
struct _echoes_broadcaster_node {
    uint64_t                    id;
    echoes_subscriber_node_t*   subscribers;
    echoes_broadcaster_node_t*  next;
};

static echoes_broadcaster_node_t null_broadcaster_node = {
    0,
    NULL,
    NULL
};

uint64_t echoes_establish_broadcaster(uint64_t id) {
    echoes_broadcaster_node_t* new;

    for (new = &null_broadcaster_node; !!new->next; new = new->next)
        if (new->id == id)
            return 0;

    new->next = (echoes_broadcaster_node_t *)malloc(sizeof(echoes_broadcaster_node_t));
    new = new->next;
    new->id = id;
    new->next = NULL;
    new->subscribers = (echoes_subscriber_node_t *)malloc(sizeof(echoes_subscriber_node_t));
    new->subscribers->handler = NULL;
    new->subscribers->hook = 0;
    new->subscribers->next = NULL;
    new->subscribers->prev = NULL;

    return new->id;
}

void echoes_dissolve_broadcaster(uint64_t id) {
    echoes_broadcaster_node_t* tbd;

    for (tbd = &null_broadcaster_node; !!tbd->next->next; tbd = tbd->next)
        if (tbd->next->id == id)
            break;

    if (!tbd->next->next && tbd->next->id != id)
        return;

    echoes_broadcaster_node_t* ahead = tbd->next->next;

    echoes_subscriber_node_t* tbf;

    for (tbf = tbd->next->subscribers; !!tbf->next; tbf = tbf->next);

    for (tbf = tbf->prev; !!tbf->prev; tbf = tbf->prev)
        free(tbf->next);

    free(tbd->next);

    tbd->next = ahead;
}

uint64_t echoes_subscribe_event(uint64_t id, void (*handler)(echoes_packet_t *)) {
    echoes_broadcaster_node_t* broadcaster;

    for (broadcaster = &null_broadcaster_node; !!broadcaster->next; broadcaster = broadcaster->next)
        if (broadcaster->id == id)
            break;

    if (!broadcaster->next && broadcaster->id != id)
        return 0;
    
    echoes_subscriber_node_t* new;

    for (new = broadcaster->subscribers; !!new->next; new = new->next);

    if (!new)
        return 0;

    new->next = (echoes_subscriber_node_t *)malloc(sizeof(echoes_subscriber_node_t));
    new->next->prev = new;

    new = new->next;

    new->next = NULL;
    new->hook = (id ^ (uint64_t)handler ^ (uint64_t)new->prev);
    new->handler = handler;
    
    return new->hook;
}

void echoes_unsubscribe_event(uint64_t hook) {
    echoes_subscriber_node_t* tbd = NULL;

    bool found = false;

    for (echoes_broadcaster_node_t* walker = &null_broadcaster_node; !!walker->next; walker = walker->next) {
        if (!walker->subscribers->handler && !walker->subscribers->hook)
            continue;

        for (tbd = walker->subscribers; !!tbd->next; tbd = tbd->next) {
            if (tbd->hook == hook) {
                found = true;
                break;
            }
        }

        if (found)
            break;
    }

    if (!found)
        return;

    echoes_subscriber_node_t* prev = tbd->prev;
    echoes_subscriber_node_t* next = tbd->next;
    
    tbd->next->prev = prev;
    tbd->prev->next = next;

    free(tbd);
}

void echoes_broadcast_event(echoes_packet_t* packet) {
    echoes_broadcaster_node_t* broadcaster = NULL;

    for (broadcaster = &null_broadcaster_node; !!broadcaster; broadcaster = broadcaster->next) {
        if (broadcaster->id == packet->id)
            break;
    }

    if (!broadcaster)
        return;

    uint64_t packet_size = sizeof(echoes_packet_t) + packet->data_length;

    for (echoes_subscriber_node_t* subscriber = broadcaster->subscribers; !!subscriber; subscriber = subscriber->next) {
        echoes_packet_t* local_packet = (echoes_packet_t *)malloc(packet_size);
        memcpy(&local_packet, &packet, packet_size);
        subscriber->handler(local_packet);
        free(local_packet);
    }
}
