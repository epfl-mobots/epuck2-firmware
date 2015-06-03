#ifndef SKEL_H
#define SKEL_H

#ifdef __cplusplus
extern "C" {
#endif

// ChibiOS includes
#include <hal.h>

// Aseba include
#include "vm/natives.h"
#include "vm/vm.h"
#include "common/types.h"

#include "skel_user.h"

extern struct _vmVariables vmVariables;
extern unsigned int events_flags;
extern AsebaVMState vmState;

extern char aseba_byte_code_container[VM_BYTECODE_SIZE];

/*
 * In your code, put "SET_EVENT(EVENT_NUMBER)" when you want to trigger an
 * event. This macro is interrupt-safe, you can call it anywhere you want.
 */
#define SET_EVENT(event) (events_flags |= (1 << event))
#define CLEAR_EVENT(event) (events_flags &= ~(1 << event))
#define IS_EVENT(event) (events_flags & (1 << event))

/*
 * Call this to init aseba. Beware, this will:
 * 1. Clear vmVariables.
 */
void aseba_vm_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SKEL_H */
