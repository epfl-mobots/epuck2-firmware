#include "motor_pid.h"
#include "motor_pwm.h"
#include "pid/pid.h"
#include "sensors/battery_level.h"
#include "msgbus/messagebus.h"
#include "main.h"
#include "parameter/parameter.h"
#include <math.h>

void pid_param_update(struct pid_param_s *p, pid_ctrl_t *ctrl)
{
    if (parameter_changed(&p->kp) ||
        parameter_changed(&p->ki) ||
        parameter_changed(&p->kd)) {
        pid_set_gains(ctrl, parameter_scalar_get(&p->kp),
                            parameter_scalar_get(&p->ki),
                            parameter_scalar_get(&p->kd));
        pid_reset_integral(ctrl);
    }
    if (parameter_changed(&p->i_limit)) {
        pid_set_integral_limit(ctrl, parameter_scalar_get(&p->i_limit));
    }
}

static void pid_param_declare(struct pid_param_s *p)
{
    parameter_scalar_declare_with_default(&p->kp, &p->ns, "kp", 0);
    parameter_scalar_declare_with_default(&p->ki, &p->ns, "ki", 0);
    parameter_scalar_declare_with_default(&p->kd, &p->ns, "kd", 0);
    parameter_scalar_declare_with_default(&p->i_limit, &p->ns, "i_limit", INFINITY);
}

static void declare_parameters(motor_pid_t *motor_pid, parameter_namespace_t *root)
{
    parameter_namespace_declare(&motor_pid->param_ns_control, root, "control");
    parameter_scalar_declare(&motor_pid->param_vel_limit, &motor_pid->param_ns_control, "velocity_limit");
    parameter_scalar_declare(&motor_pid->param_torque_limit, &motor_pid->param_ns_control, "torque_limit");
    parameter_scalar_declare(&motor_pid->param_acc_limit, &motor_pid->param_ns_control, "acceleration_limit");

    parameter_namespace_declare(&motor_pid->params_pos_pid.ns, &motor_pid->param_ns_control, "position");
    parameter_namespace_declare(&motor_pid->params_vel_pid.ns, &motor_pid->param_ns_control, "velocity");
    parameter_namespace_declare(&motor_pid->params_cur_pid.ns, &motor_pid->param_ns_control, "current");

    pid_param_declare(&motor_pid->params_pos_pid);
    pid_param_declare(&motor_pid->params_vel_pid);
    pid_param_declare(&motor_pid->params_cur_pid);
}

void motor_pid_init(motor_pid_t *motor_pid, parameter_namespace_t *parent)
{
    declare_parameters(motor_pid, parent);
}
