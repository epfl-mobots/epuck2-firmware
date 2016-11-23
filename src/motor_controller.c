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
    parameter_scalar_declare_with_default(&controller->limits.acceleration,
                                          &controller->limits.ns,
                                          "acceleration",
                                          INFINITY);

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
    float max_velocity = parameter_scalar_get(&controller->limits.velocity);
    float max_acceleration = parameter_scalar_get(&controller->limits.acceleration);
    float delta_t = 1.0 / controller->position.pid.frequency;

    if (controller->mode >= MOTOR_CONTROLLER_POSITION) {
        float desired_acceleration = motor_controller_vel_ramp(controller->position.setpoint,
                                                               controller->velocity.target_setpoint,
                                                               controller->position.target_setpoint,
                                                               delta_t,
                                                               max_velocity,
                                                               max_acceleration);
        controller->position.setpoint =
            motor_controller_pos_setpt_interpolation(controller->position.setpoint,
                                                     controller->velocity.target_setpoint,
                                                     desired_acceleration,
                                                     delta_t);
        controller->velocity.target_setpoint =
            motor_controller_vel_setpt_interpolation(controller->velocity.target_setpoint,
                                                     desired_acceleration,
                                                     delta_t);
        float position = safe_get_position(controller);
        controller->position.error = position - controller->position.setpoint;
        controller->velocity.setpoint = controller->velocity.target_setpoint +
                                        pid_process(&controller->position.pid,
                                                     controller->position.error);
    }

    if (controller->mode == MOTOR_CONTROLLER_VELOCITY) {
        /* Clamp velocity */
        controller->velocity.target_setpoint =
            motor_controller_limit_symmetric(controller->velocity.target_setpoint,
                                             max_velocity);
        float delta_velocity = controller->velocity.target_setpoint
                               - controller->velocity.setpoint;
        delta_velocity = motor_controller_limit_symmetric(delta_velocity,
                                                          delta_t * max_acceleration);
        controller->velocity.setpoint += delta_velocity;
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
    if (controller->mode == MOTOR_CONTROLLER_CURRENT) {
        controller->current.setpoint = controller->current.target_setpoint;
    }
    controller->current.setpoint =
        motor_controller_limit_symmetric(controller->current.setpoint,
                                         max_current);

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

float motor_controller_limit_symmetric(float value, float limit)
{
    if (value > limit) {
        return limit;
    } else if (value < -limit) {
        return -limit;
    } else {
        return value;
    }
}

float motor_controller_pos_setpt_interpolation(float pos, float vel, float acc,
                                               float delta_t)
{
    return pos + vel * delta_t + acc / 2 * delta_t * delta_t;
}

float motor_controller_vel_setpt_interpolation(float vel, float acc, float delta_t)
{
    return vel + acc * delta_t;
}

float motor_controller_vel_ramp(float pos, float vel, float target_pos,
                                float delta_t, float max_vel, float max_acc)
{
    // returns acceleration to be applied for the next delta_t
    float breaking_dist = vel * vel / 2 / max_acc;  // distance needed to break with max_acc
    float error = pos - target_pos;
    float error_sign = copysignf(1.0, error);

    if (error_sign != copysignf(1.0, vel)) {    // decreasing error with current vel
        if (fabs(error) <= breaking_dist || fabs(error) <= max_acc * delta_t * delta_t / 2) {
            // too close to break (or just close enough)
            return -motor_controller_limit_symmetric(vel / delta_t, max_acc);
        } else if (fabs(vel) >= max_vel) {
            // maximal velocity reached -> just cruise
            return 0;
        } else {
            // we can go faster
            return -error_sign * max_acc;
        }
    } else {
        // driving away from target position -> turn around
        if (fabs(error) <= max_acc * delta_t * delta_t / 2) {
            return -motor_controller_limit_symmetric(vel / delta_t, max_acc);
        } else {
            return -error_sign * max_acc;
        }
    }
}

void motor_controller_set_mode(motor_controller_t *controller,
                               enum motor_controller_mode mode)
{
    switch (mode) {
        case MOTOR_CONTROLLER_POSITION:
            if (controller->mode < MOTOR_CONTROLLER_POSITION) {
                controller->position.setpoint =
                    controller->position.get(controller->position.get_arg);
                controller->velocity.target_setpoint =
                    controller->velocity.get(controller->velocity.get_arg);
            }
            controller->mode = mode;
            break;

        case MOTOR_CONTROLLER_VELOCITY:
            if (controller->mode < MOTOR_CONTROLLER_VELOCITY) {
                controller->velocity.setpoint =
                    controller->velocity.get(controller->velocity.get_arg);
            }
            controller->mode = mode;
            break;

        case MOTOR_CONTROLLER_CURRENT:
            controller->mode = mode;
            break;
    }
}
