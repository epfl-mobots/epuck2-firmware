#ifndef DATAGRAM_DISPATCHER_H
#define DATAGRAM_DISPATCHER_H

#include <string.h>
#include "cmp/cmp.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dispatcher_entry_s {
    const char *id;
    int (*cb)(cmp_ctx_t *cmp, void *arg);
    void *arg;
};

void datagram_dispatcher_cb(const void *dtgrm, size_t len, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* DATAGRAM_DISPATCHER_H */
