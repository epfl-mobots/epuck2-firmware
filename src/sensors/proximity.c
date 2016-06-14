#include "proximity.h"

#include "ch.h"
#include "hal.h"
#include "analogic.h"

#define PWM_CLK_FREQ 42000000
#define PWM_FREQUENCY 1000
#define PWM_CYCLE (PWM_CLK_FREQ / PWM_FREQUENCY)
#define TCRT1000_DC 0.071               // Duty cycle for IR sensors -> 71 microseconds high time
#define COUNTER_HIGH_STATE 0x05DC       // 1500 CLK cycle -> ~17microseconds
#define COUNTER_LOW_STATE  0x9C40       // 40000 CLK cycle
#define WATCHDOG_RELOAD_VALUE 0x0028    // 40 in hexadecimal
#define NUM_IR_SENSORS 13


void proximity_start(void)
{
    static const PWMConfig pwmcfg_proximity = {
        /* timer clock frequency */
        .frequency = PWM_CLK_FREQ,
        /* timer period */
        .period = PWM_CYCLE,
        .cr2 = 0,
        .dier = 0,
        .callback = NULL,
        .channels = {
                     {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
                     {.mode = PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH, .callback = NULL},
                     {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
                     {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
},
    };

    /*Init PWM*/
    pwmStart(&PWMD8, &pwmcfg_proximity);

    /*Start PWM*/
    pwmEnableChannel(&PWMD8, 1, (pwmcnt_t) (PWM_CYCLE * TCRT1000_DC));

    /*Start ADC for proximity sensors*/
    // analogic_start(0, 0, 1);
}
