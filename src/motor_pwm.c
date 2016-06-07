#include "ch.h"
#include "hal.h"

#define PWM_CLK_FREQ 42000000
#define PWM_CYCLE 2000

#define PWM_LIMIT 0.99f

#define MOT0_PHASE_A 2
#define MOT0_PHASE_B 3
#define MOT1_PHASE_A 0
#define MOT1_PHASE_B 1

static const PWMConfig pwmcfg1 = {
    PWM_CLK_FREQ,                                   /* 42MHz PWM clock frequency.  */
    PWM_CYCLE,                                      /* PWM cycle is 21kHz.   */
    NULL,
    {
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
        {PWM_OUTPUT_ACTIVE_HIGH, NULL}
    },
    /* HW dependent part.*/
    0,                  // TIMx_CR2 value
    0                   // TIMx_DIER value
};

static const PWMConfig pwmcfg2 = {
    PWM_CLK_FREQ,                                   /* 42MHz PWM clock frequency.  */
    PWM_CYCLE,                                      /* PWM cycle is 21kHz.   */
    NULL,
    {
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_DISABLED, NULL}
    },
    /* HW dependent part.*/
    0,                  // TIMx_CR2 value
    0                   // TIMx_DIER value
};


void motor_pwm_start(void)
{
    pwmStart(&PWMD3, &pwmcfg1);
    pwmStart(&PWMD4, &pwmcfg2);
}

void motor_pwm_set(int pwm_select, float pwm_command)
{
    pwm_command = -pwm_command;
    if (pwm_command >= 0) {
        if (pwm_command > PWM_LIMIT) {
            pwm_command = PWM_LIMIT;
        }
        if (!pwm_select) {
            pwmEnableChannel(&PWMD3, MOT0_PHASE_A, (pwmcnt_t) (PWM_CYCLE * pwm_command));
            pwmEnableChannel(&PWMD3, MOT0_PHASE_B, (pwmcnt_t) 0);
        } else {
            pwmEnableChannel(&PWMD4, MOT1_PHASE_B, (pwmcnt_t) (PWM_CYCLE * pwm_command));
            pwmEnableChannel(&PWMD4, MOT1_PHASE_A, (pwmcnt_t) 0);
        }
    } else {
        if (pwm_command < -PWM_LIMIT) {
            pwm_command = -PWM_LIMIT;
        }
        pwm_command = -pwm_command;
        if (!pwm_select) {
            pwmEnableChannel(&PWMD3, MOT0_PHASE_B, (pwmcnt_t) (PWM_CYCLE * pwm_command));
            pwmEnableChannel(&PWMD3, MOT0_PHASE_A, (pwmcnt_t) 0);
        } else {
            pwmEnableChannel(&PWMD4, MOT1_PHASE_A, (pwmcnt_t) (PWM_CYCLE * pwm_command));
            pwmEnableChannel(&PWMD4, MOT1_PHASE_B, (pwmcnt_t) 0);
        }
    }
}
