#include "pid_cascade.h"
#include "math.h"
#include "pid/pid.h"
#include "motor_pwm.h"
#include "ch.h"
#include "hal.h"

float cascade_step(cascade_controller *ctrl)
{
    // position control
    ctrl->position_output = pid_process(&(ctrl->position_pid), ctrl->position_error);

    // velocity control
    ctrl->velocity_error += ctrl->position_output;
    ctrl->velocity_output = pid_process(&(ctrl->velocity_pid), ctrl->velocity_error);

    //torque control
    ctrl->current_error += ctrl->velocity_output;
    ctrl->current_output = pid_process(&(ctrl->current_pid), ctrl->current_error);

    return ctrl->current_output;
}

void cascade_init(cascade_controller *ctrl)
{
    /*Left motor controller init*/

    pid_init(&(ctrl->position_pid));
    pid_init(&(ctrl->velocity_pid));
    pid_init(&(ctrl->current_pid));

    pid_set_integral_limit(&(ctrl->position_pid), 0);
    pid_set_integral_limit(&(ctrl->velocity_pid), 0);
    pid_set_integral_limit(&(ctrl->current_pid), 0);

    pid_set_gains(&(ctrl->position_pid), 0., 0., 0.);
    pid_set_gains(&(ctrl->velocity_pid), 0., 0., 0);
    pid_set_gains(&(ctrl->current_pid), 0., 0., 0.);

    //pid_set_gains(&(ctrl_left->position_pid), 0, 0, 0);
    //pid_set_gains(&(ctrl_left->velocity_pid), 1, 0.00005, 0);
    //pid_set_gains(&(ctrl_left->current_pid), 0.0001, 0.00005, 0);

    //pid_set_gains(&(ctrl_right->position_pid), 0, 0, 0);
    //pid_set_gains(&(ctrl_right->velocity_pid), 1, 0.00005, 0);
    //pid_set_gains(&(ctrl_right->current_pid), 0.0001, 0.00005, 0);
}