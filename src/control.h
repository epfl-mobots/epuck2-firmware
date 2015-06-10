#ifndef CONTROL_H
#define CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "pid/pid.h"
#include "pid_cascade.h"
#include "feedback.h"
#include "setpoints.h"
#include "parameter/parameter.h"



struct pid_parameter_s {
    parameter_namespace_t root;
    parameter_t kp;
    parameter_t ki;
    parameter_t kd;
    parameter_t ilimit;
};


struct motor_s {
    parameter_namespace_t root;
    const char *id;

    struct cascade_controller cascade;
    struct feedback feedback;
    struct setpoints setpoints;
    float pwm_input;

    struct pid_parameter_s position_pid;
    struct pid_parameter_s velocity_pid;
    struct pid_parameter_s current_pid;
};

extern struct motor_s left;
extern struct motor_s right;

void control_start(void);


#ifdef __cplusplus
}
#endif

#endif /* CONTROL_H */
