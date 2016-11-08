#include <ch.h>
#include <hal.h>
#include "motor_pwm.h"

#define PWM_CLK_FREQ 42000000
#define PWM_FREQUENCY 21000
#define PWM_CYCLE (PWM_CLK_FREQ / PWM_FREQUENCY)

#define PWM_LIMIT 0.99f

#define MOT0_PHASE_A 2
#define MOT0_PHASE_B 3
#define MOT1_PHASE_A 0
#define MOT1_PHASE_B 1

static const PWMConfig pwmcfg1 = {
    .frequency = PWM_CLK_FREQ,
    .period = PWM_CYCLE,
    .callback = NULL,
    .channels = {
                 {PWM_OUTPUT_DISABLED, NULL},
                 {PWM_OUTPUT_DISABLED, NULL},
                 {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                 {PWM_OUTPUT_ACTIVE_HIGH, NULL}
},
    .cr2 = 0,
    .bdtr = 0,
    .dier = 0
};

static const PWMConfig pwmcfg2 = {
    .frequency = PWM_CLK_FREQ,
    .period = PWM_CYCLE,
    .callback = NULL,
    .channels = {
                 {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL},
                 {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL},
                 {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
                 {.mode = PWM_OUTPUT_DISABLED, .callback = NULL}
},
    .cr2 = 0,
    .bdtr = 0,
    .dier = 0
};


void motor_pwm_start(void)
{
    pwmStart(&PWMD3, &pwmcfg1);
    pwmStart(&PWMD4, &pwmcfg2);
}

void motor_right_pwm_set(float duty_cycle)
{
    /* Clamp the value between -PWM_LIMIT and PWM_LIMIT. */
    if (duty_cycle > PWM_LIMIT) {
        duty_cycle = PWM_LIMIT;
    } else if (duty_cycle < -PWM_LIMIT) {
        duty_cycle = -PWM_LIMIT;
    }

    if (duty_cycle <= 0) {
        pwmEnableChannel(&PWMD4, MOT1_PHASE_A, (pwmcnt_t)0);
        pwmEnableChannel(&PWMD4, MOT1_PHASE_B, (pwmcnt_t)(PWM_CYCLE * -duty_cycle));
    } else {
        pwmEnableChannel(&PWMD4, MOT1_PHASE_A, (pwmcnt_t)(PWM_CYCLE * duty_cycle));
        pwmEnableChannel(&PWMD4, MOT1_PHASE_B, (pwmcnt_t)0);
    }
}

void motor_left_pwm_set(float duty_cycle)
{
    /* Clamp the value between -PWM_LIMIT and PWM_LIMIT. */
    if (duty_cycle > PWM_LIMIT) {
        duty_cycle = PWM_LIMIT;
    } else if (duty_cycle < -PWM_LIMIT) {
        duty_cycle = -PWM_LIMIT;
    }

    if (duty_cycle <= 0) {
        pwmEnableChannel(&PWMD3, MOT0_PHASE_A, (pwmcnt_t)(PWM_CYCLE * -duty_cycle));
        pwmEnableChannel(&PWMD3, MOT0_PHASE_B, (pwmcnt_t)0);
    } else {
        pwmEnableChannel(&PWMD3, MOT0_PHASE_A, (pwmcnt_t)0);
        pwmEnableChannel(&PWMD3, MOT0_PHASE_B, (pwmcnt_t)(PWM_CYCLE * duty_cycle));
    }
}
