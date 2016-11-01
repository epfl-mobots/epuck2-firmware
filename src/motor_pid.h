#ifndef MOTOR_PID_H
#define MOTOR_PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "parameter/parameter.h"
#include "pid/pid.h"

typedef struct {
    float left;
    float right;
} motor_voltage_msg_t;

struct pid_param_s {
    parameter_t kp;
    parameter_t ki;
    parameter_t kd;
    parameter_t i_limit;
};

/** Starts the pid control loop. */
void motor_pid_start(void);

void declare_parameters(parameter_namespace_t *root);
void pid_param_update(struct pid_param_s *p, pid_ctrl_t *ctrl);

#ifdef __cplusplus
}
#endif

#endif
