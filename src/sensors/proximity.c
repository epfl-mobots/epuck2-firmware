#include "proximity.h"

#include "ch.h"
#include "hal.h"
#include "analogic.h"

#define PWM_CLK_FREQ 42000000
#define PWM_CYCLE 42000
#define TCRT1000_DC 0.071               //Duty cycle for IR sensors -> 71 microseconds high time
#define COUNTER_HIGH_STATE 0x05DC       //1500 CLK cycle -> ~17microseconds
#define COUNTER_LOW_STATE  0x9C40       //40000 CLK cycle
#define WATCHDOG_RELOAD_VALUE 0x0028    //40 in hexadecimal
#define NUM_IR_SENSORS 13

void proximity_pwm_cb(PWMDriver *pwmp)
{
    (void)pwmp;
    IWDG->KR = 0xAAAA;      //Resets watchdog counter
}

static const PWMConfig pwmcfg_proximity = {
    PWM_CLK_FREQ,                                   /* 42MHz PWM clock frequency.  */
    PWM_CYCLE,                                      /* PWM cycle is 1kHz.   */
    proximity_pwm_cb,
    {
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_DISABLED, NULL},
    {PWM_OUTPUT_DISABLED, NULL}
    },
    /* HW dependent part.*/
    0,                  // TIMx_CR2 value
    0                   // TIMx_DIER value
};

int proximity_change_adc_trigger(void) {
    if (STM32_TIM8->CCR[0] == COUNTER_HIGH_STATE) {
        STM32_TIM8->CCR[0] = COUNTER_LOW_STATE;
        return TRUE;
    }
    else {
        STM32_TIM8->CCR[0] = COUNTER_HIGH_STATE;
        return FALSE;
    }
}

void proximity_get(float* proximity)
{
    int32_t value_low[NUM_IR_SENSORS];
    int32_t value_high[NUM_IR_SENSORS];

    int i;
    analog_get_proximity(value_low, value_high);

    for (i = 0; i < NUM_IR_SENSORS; i++){
        proximity[i] = (float) (value_high[i]-value_low[i]) ;
    }
}


void proximity_start(void) {

    /*Init PWM*/    
    pwmStart(&PWMD8, &pwmcfg_proximity);
    pwmEnablePeriodicNotification(&PWMD8);          //allows callback to happen

    /*Watchdog to stop the PWM in case of malfunction*/
    /*Watchdog clock frequency is 32kHz*/
    IWDG->RLR = (IWDG_RLR_RL | WATCHDOG_RELOAD_VALUE);
    IWDG->KR |= 0xCCCC;             //Starts Watchdog countdown

    /*Start PWM*/
    pwmEnableChannel(&PWMD8, 0, (pwmcnt_t) (PWM_CYCLE * TCRT1000_DC));

    /*Capture/compare configuration*/
    STM32_TIM8->CCR[0] = COUNTER_HIGH_STATE;        //Compare value
    STM32_TIM8->CCMR1 |= TIM_CCMR1_OC1M_0;          //Output compare mode, sets output to high on match
    STM32_TIM8->CCMR1 &= ~(TIM_CCMR1_OC1PE);        //Disables preload
    STM32_TIM8->CCER &= ~(TIM_CCER_CC1P);           //active high polarity
    STM32_TIM8->CCER |= TIM_CCER_CC1E;              //enable output

    /*Start ADC for proximity sensors*/
    analogic_start(0,0,1);
}