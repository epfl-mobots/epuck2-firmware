#ifndef MOTOR_PID_H
#define MOTOR_PID_H

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
        MOTOR_PID_CURRENT_CONTROL=0,
    } mode;

    struct pid_param_s params_pos_pid, params_vel_pid, params_cur_pid;
    pid_ctrl_t cur_pid, vel_pid, pos_pid;

    float cur_setpoint;


    struct {
        struct {
            float (*fn)(void *);
            void *arg;
        } get_current;
    } callbacks;
} motor_pid_t;

void motor_pid_init(motor_pid_t *motor_pid, parameter_namespace_t *parent);

float motor_pid_process(motor_pid_t *motor_pid);

#ifdef __cplusplus
}
#endif

#endif
