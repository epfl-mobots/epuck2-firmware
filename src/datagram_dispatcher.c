#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "cmp/cmp.h"
#include "cmp_mem_access/cmp_mem_access.h"
#include "datagram_dispatcher.h"

void datagram_dispatcher_cb(const void *dtgrm, size_t len, void *arg)
{
    struct dispatcher_entry_s *dispatcher_tab = arg;
    cmp_ctx_t cmp;
    cmp_mem_access_t mem;
    cmp_mem_access_ro_init(&cmp, &mem, dtgrm, len);
    uint32_t map_size;
    if (!cmp_read_map(&cmp, &map_size)) {
        return;
    }
    uint32_t i;
    for (i = 0; i < map_size; i++) {
        uint32_t id_size;
        if (!cmp_read_str_size(&cmp, &id_size)) {
            return;
        }
        size_t str_pos = cmp_mem_access_get_pos(&mem);
        cmp_mem_access_set_pos(&mem, str_pos + id_size);
        const char *str = cmp_mem_access_get_ptr_at_pos(&mem, str_pos);
        while (dispatcher_tab->id != NULL) {
            if (strncmp(dispatcher_tab->id, str, id_size) == 0) {
                if (dispatcher_tab->cb(&cmp, dispatcher_tab->arg) != 0) {
                    return; // parsing error, stop parsing this datagram
                }
                break;
            }
            dispatcher_tab++;
        }
    }
}
