#include "ch.h"
#include "hal.h"

#define PWM_CLK_FREQ 42000000
#define PWM_CYCLE 2000


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
    /*PWM pin conf*/
    palSetPadMode(GPIOB, GPIOB_PWM0_PHASE_A, PAL_MODE_ALTERNATE(2));
    palSetPadMode(GPIOB, GPIOB_PWM0_PHASE_B, PAL_MODE_ALTERNATE(2));
    palSetPadMode(GPIOD, GPIOD_PWM1_PHASE_A, PAL_MODE_ALTERNATE(2));
    palSetPadMode(GPIOD, GPIOD_PWM1_PHASE_B, PAL_MODE_ALTERNATE(2));

    pwmStart(&PWMD3, &pwmcfg1);
    pwmStart(&PWMD4, &pwmcfg2);
}

void motor_pwm_stop(void)
{
    pwmStop(&PWMD3);
    pwmStop(&PWMD4);
}

void motor_pwm_set(int pwm_select, float pwm_command)
{
    if(pwm_command >= 0) {
        if (pwm_command > 1) {
            pwm_command = 1;
        }
        if(!pwm_select) {
            pwmEnableChannel(&PWMD3, 3, (pwmcnt_t) (PWM_CYCLE * pwm_command));
            pwmEnableChannel(&PWMD3, 4, (pwmcnt_t) 0);
        }
        else {
            pwmEnableChannel(&PWMD4, 1, (pwmcnt_t) (PWM_CYCLE * pwm_command));
            pwmEnableChannel(&PWMD3, 2, (pwmcnt_t) 0);
        }
    }
    else {
        if(pwm_command < -1) {
            pwm_command = -1;
        }
        if(!pwm_select) {
            pwmEnableChannel(&PWMD3, 4, (pwmcnt_t) (PWM_CYCLE * pwm_command));
            pwmEnableChannel(&PWMD3, 3, (pwmcnt_t) 0);
        }
        else {
            pwmEnableChannel(&PWMD4, 2, (pwmcnt_t) (PWM_CYCLE * pwm_command));
            pwmEnableChannel(&PWMD4, 1, (pwmcnt_t) 0);
        }
    }
}