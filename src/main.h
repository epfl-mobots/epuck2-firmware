#ifndef MAIN_H
#define MAIN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "msgbus/messagebus.h"

/** Robot wide IPC bus. */
extern messagebus_t bus;

#ifdef __cplusplus
}
#endif
#endif
