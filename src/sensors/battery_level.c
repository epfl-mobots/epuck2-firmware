#include <ch.h>
#include <hal.h>
#include <stdio.h>
#include "battery_level.h"

#define DMA_BUFFER_SIZE (16)
#define BATTERY_ADC_GAIN ((51. + 33.) / 33.)
#define ADC_GAIN (3.3 / 4096)

static unsigned int battery_value;
static adcsample_t adc_samples[DMA_BUFFER_SIZE];
static BSEMAPHORE_DECL(measurement_ready, true);

static void adc_cb(ADCDriver *adcp, adcsample_t *samples, size_t n)
{
    /* Calculate the average over all samples. */
    battery_value = 0;
    for (size_t i = 0; i < n; i++) {
        battery_value += samples[i];
    }
    battery_value /= n;

    chSysLockFromISR();
    adcStopConversionI(adcp);
    chBSemSignalI(&measurement_ready);
    chSysUnlockFromISR();
}

static const ADCConversionGroup group = {
    .circular = true,
    .num_channels = 1,
    .end_cb = adc_cb,
    .error_cb = NULL,
    .cr1 = 0,
    .cr2 = ADC_CR2_SWSTART, /* manual start */
    .smpr1 = 0,
    .smpr2 = ADC_SMPR2_SMP_AN0(ADC_SAMPLE_480),

    .sqr1 = ADC_SQR1_NUM_CH(1),
    .sqr2 = 0,
    .sqr3 = ADC_SQR3_SQ1_N(3), /* vsys_bat is on input 3. */
};

static THD_FUNCTION(battery_thd, arg)
{
    (void) arg;

    while (true) {
        adcAcquireBus(&ADCD2);
        adcStartConversion(&ADCD2, &group, adc_samples, DMA_BUFFER_SIZE);
        chBSemWait(&measurement_ready);
        adcReleaseBus(&ADCD2);

        chThdSleepSeconds(2);

        float val = battery_value * ADC_GAIN * BATTERY_ADC_GAIN;

        printf("battery: %.2f [V]\r\n", val);
    }
}

void battery_level_start(void)
{
    static THD_WORKING_AREA(battery_thd_wa, 2048);
    chThdCreateStatic(battery_thd_wa, sizeof(battery_thd_wa), NORMALPRIO, battery_thd, NULL);
}
