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

void encoder_init(void)
{
    /* Left Encoder Init*/
    palSetPadMode(GPIOE, GPIOE_MOT0_QEA, PAL_MODE_ALTERNATE(1) | PAL_MODE_INPUT);  // Tim1_Ch1 -> PE9
    palSetPadMode(GPIOE, GPIOE_MOT0_QEB, PAL_MODE_ALTERNATE(1) | PAL_MODE_INPUT); // Tim1_Ch2 -> PE11

    rccEnableTIM1(FALSE);                           // enable timer 1 clock
    rccResetTIM1();
    STM32_TIM1->CR2    = 0;
    STM32_TIM1->PSC    = 0;                         // Prescaler value.
    STM32_TIM1->SR     = 0;                         // Clear pending IRQs.
    STM32_TIM1->DIER   = 0;                         // DMA-related DIER bits.
    STM32_TIM1->SMCR   = STM32_TIM_SMCR_SMS(3);     // count on both edges
    STM32_TIM1->CCMR1  = STM32_TIM_CCMR1_CC1S(1);   // CC1 channel is input, IC1 is mapped on TI1
    STM32_TIM1->CCMR1 |= STM32_TIM_CCMR1_CC2S(1);   // CC2 channel is input, IC2 is mapped on TI2
    STM32_TIM1->CCER   = 0;
    STM32_TIM1->ARR    = 0xFFFF;
    STM32_TIM1->CR1    = 1;                         // start

    /* Right Encoder Init*/
    palSetPadMode(GPIOA, GPIOA_MOT1_QEB, PAL_MODE_ALTERNATE(1) | PAL_MODE_INPUT);  // Tim2_Ch1 -> PA15
    palSetPadMode(GPIOA, GPIOA_MOT1_QEA, PAL_MODE_ALTERNATE(1) | PAL_MODE_INPUT);   // Tim2_Ch2 -> PA1

    rccEnableTIM2(FALSE);                           // enable timer 2 clock
    rccResetTIM2();
    STM32_TIM2->CR2    = 0;
    STM32_TIM2->PSC    = 0;                         // Prescaler value.
    STM32_TIM2->SR     = 0;                         // Clear pending IRQs.
    STM32_TIM2->DIER   = 0;                         // DMA-related DIER bits.
    STM32_TIM2->SMCR   = STM32_TIM_SMCR_SMS(3);     // count on both edges
    STM32_TIM2->CCMR1  = STM32_TIM_CCMR1_CC1S(1);   // CC1 channel is input, IC1 is mapped on TI1
    STM32_TIM2->CCMR1 |= STM32_TIM_CCMR1_CC2S(1);   // CC2 channel is input, IC2 is mapped on TI2
    STM32_TIM2->CCER   = 0;
    STM32_TIM2->ARR    = 0xFFFF;
    STM32_TIM2->CR1    = 1;                         // start
}
