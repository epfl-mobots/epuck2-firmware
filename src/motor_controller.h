#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "parameter/parameter.h"
#include "pid/pid.h"

typedef struct {
    float left;
    float right;
} motor_voltage_msg_t;

/** A group of parameters related to a given PID controller. */
struct pid_param_s {
    parameter_namespace_t ns;
    parameter_t kp;
    parameter_t ki;
    parameter_t kd;
    parameter_t i_limit;
};

enum motor_controller_mode {
    MOTOR_CONTROLLER_CURRENT=0,
    MOTOR_CONTROLLER_VELOCITY,
    MOTOR_CONTROLLER_POSITION,
};

/** Cascade PID controller. */
typedef struct {
    /** Namespace grouping all parameters related to the controller. */
    parameter_namespace_t param_ns_control;

    /** Subnamespace for the safety limits parameters. */
    struct {
        parameter_namespace_t ns;
        parameter_t velocity;
        parameter_t current;
        parameter_t acceleration;
    } limits;

    /** The mode in which the controller operatres. */
    enum motor_controller_mode mode;

    struct {
        /** Parameters for this controller. */
        struct pid_param_s params;

        /** PID filter instance. */
        pid_ctrl_t pid;

        /** Setpoint in rad, rad/s or rad/s^2 */
        float target_setpoint;

        /** Interpolated setpoint, respecting limits. */
        float setpoint;

        /** Error of the controller. */
        float error;

        /** Callback used to get the input of the filter. */
        float (*get)(void *);
        void *get_arg;

        /** Control frequency divider. */
        int divider;

        /** Simple counter used for the divider calculation. */
        int divider_counter;
    } position, velocity, current,theta,thetad,voltage;
} motor_controller_t;

/** Inits a motor controller in a given parameter namespace. */
void motor_controller_init(motor_controller_t *controller, parameter_namespace_t *parent);

/** Sets the control mode for the given controller.
 *
 * @note This function switches mode in a safe manner (i.e. should not cause
 * any unexpected motion).
 */
void motor_controller_set_mode(motor_controller_t *controller,
                               enum motor_controller_mode mode);

/** Runs all the needed filter and returns the motor voltage to apply. */
float motor_controller_process(motor_controller_t *controller);

/** Sets the frequency of all the control loops. */
void motor_controller_set_frequency(motor_controller_t *controller, float frequency);

float segway_voltage_setpoint(motor_controller_t *controller,void *arg);

/** Sets the frequency prescalers for velocity and position control loops.
 *
 * This function sets the divider used for the velocity and position control
 * loops. For example a velocity divider of 5 with a frequency of 100 Hz means
 * the current control will process at 100 Hz but the velocity loop will run at
 * 20 Hz.
 *
 * @note Must be called before motor_controller_set_frequency.
 */
void motor_controller_set_prescaler(motor_controller_t *controller, int velocity_divider, int position_divider);

#ifdef __cplusplus
}
#endif

#endif
