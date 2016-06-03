#ifndef MOTOR_PWM_H
#define MOTOR_PWM_H

#ifdef __cplusplus
extern "C" {
#endif



/*
 * pwm_select : 0 -> motor on PB0 and PB1 // 1 -> motor on PD12 and PD13
 * pwm_command : duty cycle between -1 and +1, negative for reverse direction
 */
void motor_pwm_set(int pwm_select, float pwm_command);

/*
 * drive the motor voltage
 */
void motor_pwm_start(void);

/*
 * don't drive the motor (floating)
 */
void motor_pwm_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_PWM_H */
