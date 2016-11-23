#include <ch.h>
#include <hal.h>
#include <math.h>
#include "msgbus/messagebus.h"
#include "main.h"
#include "encoder.h"

#define MAX_16BIT      ((1 << 16) - 1)
#define MAX_16BIT_DIV2 32767

#define TICKS_PER_RADIAN (12 * 100 / (2 * M_PI))  // 12 ticks times 100 reduction

#define LOOP_FREQUENCY 100.0  // Hz
#define VELOCITY_SMOOTHING_FACTOR 0.5

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

static float iir_filter(float input, float previous_output, float smoothing_factor)
{
    return smoothing_factor * input + (1 - smoothing_factor) * previous_output;
}


static THD_FUNCTION(encoders_thd, arg)
{
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    TOPIC_DECL(encoders_topic, encoders_msg_t);
    messagebus_advertise_topic(&bus, &encoders_topic.topic, "/encoders");
    TOPIC_DECL(wheel_pos_topic, wheel_pos_msg_t);
    messagebus_advertise_topic(&bus, &wheel_pos_topic.topic, "/wheel_pos");
    TOPIC_DECL(wheel_velocities_topic, wheel_velocities_msg_t);
    messagebus_advertise_topic(&bus, &wheel_velocities_topic.topic, "/wheel_velocities");

    rccEnableTIM1(FALSE);                           // enable timer 1 clock
    rccResetTIM1();
    setup_timer(STM32_TIM1);

    rccEnableTIM2(FALSE);                           // enable timer 2 clock
    rccResetTIM2();
    setup_timer(STM32_TIM2);

    uint32_t left_encoder_old, right_encoder_old;
    encoders_msg_t encoders = {0, 0};
    wheel_pos_msg_t wheel_positions = {0.f, 0.f};
    wheel_velocities_msg_t wheel_velocities =  {0.f, 0.f};

    left_encoder_old = encoder_get_left();
    right_encoder_old = encoder_get_right();

    while (1) {
        uint32_t left_encoder, right_encoder;
        float left_velocity, right_velocity;

        left_encoder = encoder_get_left();
        right_encoder = encoder_get_right();

        /* We use -= because encoders are reversed. */
        encoders.left  -= encoder_tick_diff(left_encoder_old, left_encoder);
        encoders.right -= encoder_tick_diff(right_encoder_old, right_encoder);

        wheel_positions.left = (float)encoders.left / TICKS_PER_RADIAN;
        wheel_positions.right = (float)encoders.right / TICKS_PER_RADIAN;

        /* Encoders are still reversed. */
        left_velocity = -(float)encoder_tick_diff(left_encoder_old, left_encoder)
                        / TICKS_PER_RADIAN * LOOP_FREQUENCY;
        right_velocity = -(float)encoder_tick_diff(right_encoder_old, right_encoder)
                         / TICKS_PER_RADIAN * LOOP_FREQUENCY;

        wheel_velocities.left = iir_filter(left_velocity,
                                           wheel_velocities.left,
                                           VELOCITY_SMOOTHING_FACTOR);
        wheel_velocities.right = iir_filter(right_velocity,
                                            wheel_velocities.right,
                                            VELOCITY_SMOOTHING_FACTOR);

        messagebus_topic_publish(&encoders_topic.topic, &encoders, sizeof(encoders));
        messagebus_topic_publish(&wheel_pos_topic.topic, &wheel_positions,
                                 sizeof(wheel_positions));
        messagebus_topic_publish(&wheel_velocities_topic.topic, &wheel_velocities,
                                 sizeof(wheel_velocities));

        left_encoder_old = left_encoder;
        right_encoder_old = right_encoder;
        chThdSleepMilliseconds(1000 / LOOP_FREQUENCY);
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
    static THD_WORKING_AREA(encoders_thd_wa, 768);
    chThdCreateStatic(encoders_thd_wa, sizeof(encoders_thd_wa), NORMALPRIO, encoders_thd, NULL);
}
