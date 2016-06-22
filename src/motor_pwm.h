#ifndef MOTOR_PWM_H
#define MOTOR_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

/** Sets the duty cycle for left motor PWM.
 *
 * @param [in] duty_cycle The PWM duty cycle, between -1 and + 1. */
void motor_left_pwm_set(float duty_cycle);

/** Sets the duty cycle for left motor PWM.
 *
 * @param [in] duty_cycle The PWM duty cycle, between -1 and + 1. */
void motor_right_pwm_set(float duty_cycle);

/** Configures the motor PWM driver. */
void motor_pwm_start(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_PWM_H */
