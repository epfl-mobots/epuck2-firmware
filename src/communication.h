#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <ch.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "parameter/parameter.h"


void communication_start(BaseSequentialStream *out);

extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif /* COMMUNICATION_H */