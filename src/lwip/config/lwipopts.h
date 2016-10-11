/**
 * @file
 *
 * lwIP Options Configuration
 */

#ifndef __LWIPOPT_H__
#define __LWIPOPT_H__


/* See lwip/src/include/lwip/opt.h for reference. */

#define MEM_ALIGNMENT                   4

#define TCPIP_MBOX_SIZE                 MEMP_NUM_PBUF

#define LWIP_SOCKET 0

#define LWIP_ARP                        0
#define LWIP_HAVE_LOOPIF                1

#define LWIP_HAVE_SLIPIF                1
#define SLIP_STREAM                     ((BaseSequentialStream *)&SDU1)

#define TCPIP_THREAD_PRIO               NORMALPRIO
#define TCPIP_THREAD_STACKSIZE          2048

#define SLIPIF_THREAD_PRIO              (NORMALPRIO - 1)
#define SLIPIF_THREAD_STACKSIZE         1024

#define DEFAULT_THREAD_PRIO             NORMALPRIO
#define DEFAULT_THREAD_STACK_SIZE       4096

#define DEFAULT_RAW_RECVMBOX_SIZE       4
#define DEFAULT_UDP_RECVMBOX_SIZE       4
#define DEFAULT_TCP_RECVMBOX_SIZE       4
#define DEFAULT_ACCEPTMBOX_SIZE         4

/** Use newlib malloc() instead of memory pools. */
#include <stdlib.h>
#define MEM_LIBC_MALLOC 1
#define MEMP_MEM_MALLOC 1

#endif /* __LWIPOPT_H__ */
