#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <ch.h>

#ifdef __cplusplus
extern "C" {
#endif

void communication_start(BaseSequentialStream *out);

#ifdef __cplusplus
}
#endif

#endif /* COMMUNICATION_H */