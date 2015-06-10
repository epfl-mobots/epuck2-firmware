#ifndef CONTROLLER_H
#define CPNTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "pid/pid.h"

typedef struct cascade_controller{
    pid_ctrl_t current_pid;
    pid_ctrl_t velocity_pid;
    pid_ctrl_t position_pid;

    float position_error;
    float position_output;
    float velocity_error;
    float velocity_output;
    float current_error;
    float current_output;

    float* output;
}cascade_controller;

float cascade_step(cascade_controller *ctrl);
void cascade_init(cascade_controller *ctrl);


#ifdef __cplusplus
}
#endif

#endif /* PID_CASCADE_H */