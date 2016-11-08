#include "motor_controller.h"
#include "pid/pid.h"
#include "msgbus/messagebus.h"
#include "main.h"
#include "parameter/parameter.h"
#include <math.h>
#include <string.h>

static float safe_get_current(motor_controller_t *controller);
static float safe_get_velocity(motor_controller_t *controller);
static float safe_get_position(motor_controller_t *controller);

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

static void declare_parameters(motor_controller_t *controller, parameter_namespace_t *root)
{
    parameter_namespace_declare(&controller->param_ns_control, root, "control");
    parameter_namespace_declare(&controller->limits.ns, &controller->param_ns_control, "limits");
    parameter_scalar_declare_with_default(&controller->limits.velocity,
                                          &controller->limits.ns,
                                          "velocity",
                                          INFINITY);
    parameter_scalar_declare_with_default(&controller->limits.current,
                                          &controller->limits.ns,
                                          "current",
                                          INFINITY);
    parameter_scalar_declare(&controller->limits.acceleration,
                             &controller->limits.ns,
                             "acceleration");

    parameter_namespace_declare(&controller->position.params.ns,
                                &controller->param_ns_control,
                                "position");
    parameter_namespace_declare(&controller->velocity.params.ns,
                                &controller->param_ns_control,
                                "velocity");
    parameter_namespace_declare(&controller->current.params.ns,
                                &controller->param_ns_control,
                                "current");

    pid_param_declare(&controller->position.params);
    pid_param_declare(&controller->velocity.params);
    pid_param_declare(&controller->current.params);
}

void motor_controller_init(motor_controller_t *controller, parameter_namespace_t *parent)
{
    memset(controller, 0, sizeof(motor_controller_t));
    declare_parameters(controller, parent);
    pid_init(&controller->current.pid);
    pid_init(&controller->velocity.pid);
    pid_init(&controller->position.pid);
}

void motor_controller_set_frequency(motor_controller_t *controller, float frequency)
{
    controller->position.pid.frequency = frequency;
    controller->velocity.pid.frequency = frequency;
    controller->current.pid.frequency = frequency;
}

float motor_controller_process(motor_controller_t *controller)
{
    /* Update controller gains. */
    pid_param_update(&controller->position.params, &controller->position.pid);
    pid_param_update(&controller->velocity.params, &controller->velocity.pid);
    pid_param_update(&controller->current.params, &controller->current.pid);

    /* Position control */
    if (controller->mode >= MOTOR_CONTROLLER_POSITION) {
        float position = safe_get_position(controller);
        controller->position.error = position - controller->position.setpoint;
        controller->velocity.setpoint = pid_process(&controller->position.pid,
                                                    controller->position.error);
    }

    /* Clamp velocity */
    float max_velocity = parameter_scalar_get(&controller->limits.velocity);
    if (controller->velocity.setpoint > max_velocity) {
        controller->velocity.setpoint = max_velocity;
    } else if (controller->velocity.setpoint < -max_velocity) {
        controller->velocity.setpoint = -max_velocity;
    }

    /* Velocity control */
    if (controller->mode >= MOTOR_CONTROLLER_VELOCITY) {
        float velocity = safe_get_velocity(controller);
        controller->velocity.error = velocity - controller->velocity.setpoint;
        controller->current.setpoint = pid_process(&controller->velocity.pid,
                                                   controller->velocity.error);
    }

    /* Current (torque) control. */
    float max_current = parameter_scalar_get(&controller->limits.current);
    if (controller->current.setpoint > max_current) {
        controller->current.setpoint = max_current;
    } else if (controller->current.setpoint < -max_current) {
        controller->current.setpoint = -max_current;
    }
    float current = safe_get_current(controller);
    controller->current.error = current - controller->current.setpoint;

    return pid_process(&controller->current.pid, controller->current.error);
}

static float safe_get_current(motor_controller_t *controller)
{
    float current = 0.;

    if (controller->current.get) {
        current = controller->current.get(controller->current.get_arg);
    }

    return current;
}

static float safe_get_velocity(motor_controller_t *controller)
{
    float velocity = 0.;

    if (controller->velocity.get) {
        velocity = controller->velocity.get(controller->velocity.get_arg);
    }

    return velocity;
}

static float safe_get_position(motor_controller_t *controller)
{
    float position = 0.;

    if (controller->position.get) {
        position = controller->position.get(controller->position.get_arg);
    }

    return position;
}
