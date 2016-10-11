#include <ch.h>
#include <hal.h>
#include <lwip/opt.h>
#include <lwip/sys.h>
#include <lwip/tcpip.h>
#include <netif/slipif.h>

#include "lwipthread.h"

static sys_sem_t lwip_init_done;

void ipinit_done_cb(void *a)
{
    (void) a;
    sys_sem_signal(&lwip_init_done);
}


void ip_start(void)
{
    ip_addr_t ip, gw, netmask;

    static struct netif slipif;

    sys_sem_new(&lwip_init_done, 0);

    /* Initializes the thing.*/
    tcpip_init(ipinit_done_cb, NULL);

    /* Wait for init to be complete. */
    sys_sem_wait(&lwip_init_done);
    sys_sem_free(&lwip_init_done);

    /* TODO: Get the ip config from the config tree. */
    IP4_ADDR(&ip, 192, 168, 3, 2);
    IP4_ADDR(&gw, 192, 168, 3, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 255);

    /* Init serial interface. */
    netif_add(&slipif, &ip, &netmask, &gw, NULL, slipif_init, tcpip_input);
    netif_set_default(&slipif);
    netif_set_up(&slipif);
}
