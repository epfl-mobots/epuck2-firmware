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

struct pid_param_s {
    parameter_namespace_t ns;
    parameter_t kp;
    parameter_t ki;
    parameter_t kd;
    parameter_t i_limit;
};

typedef struct {
    parameter_namespace_t param_ns_control;

    struct {
        parameter_namespace_t ns;
        parameter_t velocity;
        parameter_t current;
        parameter_t acceleration;
    } limits;

    enum {
        MOTOR_CONTROLLER_CURRENT=0,
        MOTOR_CONTROLLER_VELOCITY,
        MOTOR_CONTROLLER_POSITION,
    } mode;

    struct {
        struct pid_param_s params;
        pid_ctrl_t pid;
        float setpoint;
        float error;
        float (*get)(void *);
        void *get_arg;
    } position, velocity, current;
} motor_controller_t;

void motor_controller_init(motor_controller_t *controller, parameter_namespace_t *parent);

float motor_controller_process(motor_controller_t *controller);

#ifdef __cplusplus
}
#endif

#endif
