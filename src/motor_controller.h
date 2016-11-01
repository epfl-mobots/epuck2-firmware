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
    parameter_t param_vel_limit;
    parameter_t param_torque_limit;
    parameter_t param_acc_limit;

    enum {
        MOTOR_CONTROLLER_CURRENT=0,
        MOTOR_CONTROLLER_VELOCITY,
    } mode;

    struct pid_param_s params_pos_pid, params_vel_pid, params_cur_pid;
    pid_ctrl_t cur_pid, vel_pid, pos_pid;

    float cur_setpoint, vel_setpoint;


    struct {
        struct {
            float (*fn)(void *);
            void *arg;
        } get_current, get_velocity;
    } callbacks;
} motor_controller_t;

void motor_controller_init(motor_controller_t *controller, parameter_namespace_t *parent);

float motor_controller_process(motor_controller_t *controller);

#ifdef __cplusplus
}
#endif

#endif
