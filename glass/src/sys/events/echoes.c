#include "echoes.h"
#include <stddef.h>

typedef struct _echoes_subscriber_node echoes_subscriber_node_t;
struct _echoes_subscriber_node {
    uint64_t                    hook;
    void                        (*handler)(echoes_packet_t *);
    echoes_subscriber_node_t*   next;
};

typedef struct _echoes_broadcaster_node echoes_broadcaster_node_t;
struct _echoes_broadcaster_node {
    uint64_t                    id;
    echoes_subscriber_node_t*   subscribers;
    echoes_broadcaster_node_t*  next;
};
