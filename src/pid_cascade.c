#include "pid_cascade.h"
#include "math.h"
#include "pid/pid.h"
#include "ch.h"
#include "hal.h"

float cascade_step(cascade_controller *ctrl)
{
    // position control
    if (ctrl->pos_ctrl_en) {
        ctrl->position_output = pid_process(&(ctrl->position_pid), ctrl->position_error);
    } else {
        pid_reset_integral(&ctrl->position_pid);
        ctrl->position_output = 0;
    }

    // velocity control
    ctrl->velocity_error += ctrl->position_output;
    if (ctrl->vel_ctrl_en) {
        ctrl->velocity_output = pid_process(&(ctrl->velocity_pid), ctrl->velocity_error);
    } else {
        pid_reset_integral(&ctrl->velocity_pid);
        ctrl->velocity_output = 0;
    }

    //torque control
    ctrl->current_error += ctrl->velocity_output;
    ctrl->current_output = pid_process(&(ctrl->current_pid), ctrl->current_error);

    return ctrl->current_output;
}

void cascade_init(cascade_controller *ctrl)
{
    ctrl->pos_ctrl_en = true;
    ctrl->vel_ctrl_en = true;

    pid_init(&(ctrl->position_pid));
    pid_init(&(ctrl->velocity_pid));
    pid_init(&(ctrl->current_pid));

    pid_set_integral_limit(&(ctrl->position_pid), 0);
    pid_set_integral_limit(&(ctrl->velocity_pid), 0);
    pid_set_integral_limit(&(ctrl->current_pid), 0);

    pid_set_gains(&(ctrl->position_pid), 0., 0., 0.);
    pid_set_gains(&(ctrl->velocity_pid), 0., 0., 0);
    pid_set_gains(&(ctrl->current_pid), 0., 0., 0.);

}

void cascade_mode_pos_ctrl(cascade_controller *ctrl)
{
    ctrl->pos_ctrl_en = true;
    ctrl->vel_ctrl_en = true;
}

void cascade_mode_vel_ctrl(cascade_controller *ctrl)
{
    ctrl->pos_ctrl_en = false;
    ctrl->vel_ctrl_en = true;
}

void cascade_mode_cur_ctrl(cascade_controller *ctrl)
{
    ctrl->pos_ctrl_en = false;
    ctrl->vel_ctrl_en = false;
}
