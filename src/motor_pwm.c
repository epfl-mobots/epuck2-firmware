#include <ch.h>
#include <hal.h>
#include <parameter/parameter.h>
#include "main.h"
#include "motor_pwm.h"

#define PWM_CLK_FREQ 42000000
#define PWM_FREQUENCY 44100
#define PWM_CYCLE (PWM_CLK_FREQ / PWM_FREQUENCY)

#define PWM_LIMIT 0.99f
#define ADC_MEASUREMENT_POS 0.01

#define MOT0_PHASE_A 2
#define MOT0_PHASE_B 3
#define MOT1_PHASE_A 0
#define MOT1_PHASE_B 1
#define PWM4_ADC_TRIGGER 3

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
        {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL},
    },
    .cr2 = 0,
    .bdtr = 0,

    /* Expose a DMA trigger, which is used by the motor current sense. */
    .dier = TIM_DIER_CC4DE,
};

static parameter_namespace_t encoders_ns;
static struct {
    parameter_namespace_t ns;
    parameter_t is_inverted;
} left_params, right_params;


void motor_pwm_start(void)
{
    parameter_namespace_declare(&encoders_ns, &parameter_root, "motors");
    parameter_namespace_declare(&left_params.ns, &encoders_ns, "left");
    parameter_namespace_declare(&right_params.ns, &encoders_ns, "right");

    parameter_boolean_declare_with_default(&left_params.is_inverted,
                                           &left_params.ns,
                                           "is_inverted",
                                           false);
    parameter_boolean_declare_with_default(&right_params.is_inverted,
                                           &right_params.ns,
                                           "is_inverted",
                                           false);

    pwmStart(&PWMD3, &pwmcfg1);
    pwmStart(&PWMD4, &pwmcfg2);

    /* This channel is used to trigger the motor current sense on the "enabled"
     * part of the waveform. */
    pwmEnableChannel(&PWMD4, PWM4_ADC_TRIGGER, (pwmcnt_t)PWM_CYCLE * ADC_MEASUREMENT_POS);
}

void motor_right_pwm_set(float duty_cycle)
{
    /* Clamp the value between -PWM_LIMIT and PWM_LIMIT. */
    if (duty_cycle > PWM_LIMIT) {
        duty_cycle = PWM_LIMIT;
    } else if (duty_cycle < -PWM_LIMIT) {
        duty_cycle = -PWM_LIMIT;
    }

    if (parameter_boolean_get(&right_params.is_inverted)) {
        duty_cycle = -duty_cycle;
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

    if (parameter_boolean_get(&left_params.is_inverted)) {
        duty_cycle = -duty_cycle;
    }

    if (duty_cycle <= 0) {
        pwmEnableChannel(&PWMD3, MOT0_PHASE_A, (pwmcnt_t)0);
        pwmEnableChannel(&PWMD3, MOT0_PHASE_B, (pwmcnt_t)(PWM_CYCLE * -duty_cycle));
    } else {
        pwmEnableChannel(&PWMD3, MOT0_PHASE_A, (pwmcnt_t)(PWM_CYCLE * duty_cycle));
        pwmEnableChannel(&PWMD3, MOT0_PHASE_B, (pwmcnt_t)0);
    }
}
