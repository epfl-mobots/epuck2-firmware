#ifndef MOTOR_PID_H
#define MOTOR_PID_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float left;
    float right;
} motor_voltage_msg_t;

/** Starts the pid control loop. */
void motor_pid_start(void);

#ifdef __cplusplus
}
#endif

#endif
