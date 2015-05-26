#ifndef CONTROLLER_H
#define CPNTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "pid/pid.h"

struct cascade_controller{
    pid_filter_t current_pid;
    pid_filter_t velocity_pid;
    pid_filter_t position_pid;

    float position_error;
    float position_output;
    float velocity_error;
    float velocity_output;
    float current_error;
    float current_output;

    float* output;
};

float cascade_step(struct cascade_controller ctrl);
void cascade_init(struct cascade_controller ctrl_left, struct cascade_controller ctrl_right);


#ifdef __cplusplus
}
#endif

#endif /* PID_CASCADE_H */