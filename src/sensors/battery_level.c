#include <ch.h>
#include <hal.h>
#include <stdio.h>
#include "battery_level.h"
#include "main.h"

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

    TOPIC_DECL(battery_topic, battery_msg_t);
    messagebus_advertise_topic(&bus, &battery_topic.topic, "/battery_level");

    while (true) {
        adcAcquireBus(&ADCD2);

        /* Starts a measurement and waits for it to complete. */
        adcStartConversion(&ADCD2, &group, adc_samples, DMA_BUFFER_SIZE);
        chBSemWait(&measurement_ready);

        adcReleaseBus(&ADCD2);

        /* Converts the measurement to volts and publish the measurement on the
         * bus. */
        battery_msg_t msg;
        msg.voltage = battery_value * ADC_GAIN * BATTERY_ADC_GAIN;

        messagebus_topic_publish(&battery_topic.topic, &msg, sizeof(msg));

        /* Sleep for some time. */
        chThdSleepSeconds(2);
    }
}

void battery_level_start(void)
{
    static THD_WORKING_AREA(battery_thd_wa, 2048);

    /* Since this is used for safety we run it at highest priority to make sure
     * it never starves. */
    chThdCreateStatic(battery_thd_wa, sizeof(battery_thd_wa), HIGHPRIO, battery_thd, NULL);
}
