#ifndef PID_CASCADE_H
#define PID_CASCADE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "pid/pid.h"

typedef struct cascade_controller {
    pid_ctrl_t current_pid;
    pid_ctrl_t velocity_pid;
    pid_ctrl_t position_pid;

    float position_error;
    float position_output;
    float velocity_error;
    float velocity_output;
    float current_error;
    float current_output;

    bool pos_ctrl_en;
    bool vel_ctrl_en;

    float* output;
} cascade_controller;

float cascade_step(cascade_controller *ctrl);
void cascade_init(cascade_controller *ctrl);
void cascade_mode_pos_ctrl(cascade_controller *ctrl);
void cascade_mode_vel_ctrl(cascade_controller *ctrl);
void cascade_mode_cur_ctrl(cascade_controller *ctrl);

#ifdef __cplusplus
}
#endif

#endif /* PID_CASCADE_H */
