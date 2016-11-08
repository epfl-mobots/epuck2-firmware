#ifndef MOTOR_PID_THREAD_H
#define MOTOR_PID_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "motor_controller.h"

typedef struct {
    enum motor_controller_mode mode;
    float left;
    float right;
} wheels_setpoint_t;

/** Start wheel motor control. */
void motor_pid_start(void);

#ifdef __cplusplus
}
#endif

#endif
