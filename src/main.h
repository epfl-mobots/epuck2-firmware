#ifndef MAIN_H
#define MAIN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "msgbus/messagebus.h"
#include "parameter/parameter.h"

/** Macro to declare a topic and associated locking constructs. */
#define TOPIC_DECL(name, type) struct { \
    messagebus_topic_t topic; \
    mutex_t lock; \
    condition_variable_t condvar; \
    type value; \
} name = { \
    .lock = _MUTEX_DATA(name.lock), \
    .condvar = _CONDVAR_DATA(name.condvar), \
    .topic = _MESSAGEBUS_TOPIC_DATA(name.topic, name.lock, name.condvar, &name.value, sizeof(type)), \
};

/** Robot wide IPC bus. */
extern messagebus_t bus;

/** Robot wide parameter tree */
extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif
#endif
