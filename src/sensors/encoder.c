#include <ch.h>
#include <hal.h>
#include <math.h>
#include "msgbus/messagebus.h"
#include "main.h"
#include "encoder.h"

#define MAX_16BIT      ((1 << 16) - 1)
#define MAX_16BIT_DIV2 32767

#define TICKS_PER_RADIAN (12*100/(2*M_PI))

static void setup_timer(stm32_tim_t *tmr);

uint32_t encoder_get_left(void)
{
    return STM32_TIM1->CNT;
}

uint32_t encoder_get_right(void)
{
    return STM32_TIM2->CNT;
}

int encoder_tick_diff(uint32_t enc_old, uint32_t enc_new)
{
    if (enc_old < enc_new) {
        if (enc_new - enc_old <= MAX_16BIT_DIV2) {
            return enc_new - enc_old;
        } else {
            return -(MAX_16BIT + 1 - (enc_new - enc_old));
        }
    } else if (enc_old > enc_new) {
        if (enc_old - enc_new <= MAX_16BIT_DIV2) {
            return -(enc_old - enc_new);
        } else {
            return MAX_16BIT + 1 - (enc_old - enc_new);
        }
    } else {
        return 0;
    }
}


static THD_FUNCTION(encoders_thd, arg)
{
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    TOPIC_DECL(encoders_topic, encoders_msg_t);
    messagebus_advertise_topic(&bus, &encoders_topic.topic, "/encoders");

    TOPIC_DECL(wheel_pos_topic, wheel_pos_msg_t);
    messagebus_advertise_topic(&bus, &wheel_pos_topic.topic, "/wheel_pos");

    rccEnableTIM1(FALSE);                           // enable timer 1 clock
    rccResetTIM1();
    setup_timer(STM32_TIM1);

    rccEnableTIM2(FALSE);                           // enable timer 2 clock
    rccResetTIM2();
    setup_timer(STM32_TIM2);

    uint32_t left_old, right_old;
    encoders_msg_t values = {0, 0};
    wheel_pos_msg_t wheel_positions = {0.f, 0.f};

    left_old = encoder_get_left();
    right_old = encoder_get_right();

    while (1) {
        uint32_t left, right;

        left = encoder_get_left();
        right = encoder_get_right();

        values.left += encoder_tick_diff(left_old, left);
        values.right += encoder_tick_diff(right_old, right);

        wheel_positions.left = (float)values.left / TICKS_PER_RADIAN;
        wheel_positions.right = (float)values.right / TICKS_PER_RADIAN;

        messagebus_topic_publish(&encoders_topic.topic, &values, sizeof(values));
        messagebus_topic_publish(&wheel_pos_topic.topic, &wheel_positions,
                sizeof(wheel_positions));

        left_old = left;
        right_old = right;
        chThdSleepMilliseconds(10);
    }
}

static void setup_timer(stm32_tim_t *tmr)
{
    tmr->CR2    = 0;
    tmr->PSC    = 0;                         // Prescaler value.
    tmr->SR     = 0;                         // Clear pending IRQs.
    tmr->DIER   = 0;                         // DMA-related DIER bits.
    tmr->SMCR   = STM32_TIM_SMCR_SMS(3);     // count on both edges
    tmr->CCMR1  = STM32_TIM_CCMR1_CC1S(1);   // CC1 channel is input, IC1 is mapped on TI1
    tmr->CCMR1 |= STM32_TIM_CCMR1_CC2S(1);   // CC2 channel is input, IC2 is mapped on TI2
    tmr->CCER   = 0;
    tmr->ARR    = 0xFFFF;
    tmr->CR1    = 1;                         // start
}

void encoder_start(void)
{
    static THD_WORKING_AREA(encoders_thd_wa, 512);
    chThdCreateStatic(encoders_thd_wa, sizeof(encoders_thd_wa), NORMALPRIO, encoders_thd, NULL);
}
