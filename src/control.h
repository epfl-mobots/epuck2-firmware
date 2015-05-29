#ifndef CONTROL_H
#define CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "pid/pid.h"

void control_start(void);
void control_test(void);



#ifdef __cplusplus
}
#endif

#endif /* CONTROL_H */
