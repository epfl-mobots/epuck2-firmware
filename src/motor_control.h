#ifndef CONTROLLER_H
#define CPNTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "pid/pid.h"

void motor_control_init(void);

#ifdef __cplusplus
}
#endif

#endif /* PID_CASCADE_H */