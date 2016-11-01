#include "motor_pid.h"
#include "motor_pwm.h"
#include "pid/pid.h"
#include "sensors/battery_level.h"
#include "msgbus/messagebus.h"
#include "main.h"
#include "parameter/parameter.h"
#include <math.h>

// control loop parameters
static parameter_namespace_t param_ns_control;
static parameter_t param_vel_limit;
static parameter_t param_torque_limit;
static parameter_t param_acc_limit;
static parameter_namespace_t param_ns_pos_ctrl;
static parameter_namespace_t param_ns_vel_ctrl;
static parameter_namespace_t param_ns_cur_ctrl;
static struct pid_param_s pos_pid_params;
static struct pid_param_s vel_pid_params;
static struct pid_param_s cur_pid_params;

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

static void pid_param_declare(struct pid_param_s *p, parameter_namespace_t *ns)
{
    parameter_scalar_declare_with_default(&p->kp, ns, "kp", 0);
    parameter_scalar_declare_with_default(&p->ki, ns, "ki", 0);
    parameter_scalar_declare_with_default(&p->kd, ns, "kd", 0);
    parameter_scalar_declare_with_default(&p->i_limit, ns, "i_limit", INFINITY);
}

void declare_parameters(parameter_namespace_t *root)
{
    parameter_namespace_declare(&param_ns_control, root, "control");
    parameter_scalar_declare(&param_vel_limit, &param_ns_control, "velocity_limit");
    parameter_scalar_declare(&param_torque_limit, &param_ns_control, "torque_limit");
    parameter_scalar_declare(&param_acc_limit, &param_ns_control, "acceleration_limit");

    parameter_namespace_declare(&param_ns_pos_ctrl, &param_ns_control, "position");
    pid_param_declare(&pos_pid_params, &param_ns_pos_ctrl);

    parameter_namespace_declare(&param_ns_vel_ctrl, &param_ns_control, "velocity");
    pid_param_declare(&vel_pid_params, &param_ns_vel_ctrl);

    parameter_namespace_declare(&param_ns_cur_ctrl, &param_ns_control, "current");
    pid_param_declare(&cur_pid_params, &param_ns_cur_ctrl);
}
