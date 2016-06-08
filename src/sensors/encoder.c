#include <ch.h>
#include <hal.h>

#define MAX_16BIT      ((1 << 16) - 1)
#define MAX_16BIT_DIV2 32767

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
    rccEnableTIM1(FALSE);                           // enable timer 1 clock
    rccResetTIM1();
    setup_timer(STM32_TIM1);

    rccEnableTIM2(FALSE);                           // enable timer 2 clock
    rccResetTIM2();
    setup_timer(STM32_TIM2);
}
