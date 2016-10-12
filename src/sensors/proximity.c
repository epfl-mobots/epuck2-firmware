#include "proximity.h"

#include "ch.h"
#include "hal.h"
#include <stdio.h>
#include <string.h>

#include "main.h"

#define PWM_CLK_FREQ 42000000
#define PWM_FREQUENCY 1000
#define PWM_CYCLE (PWM_CLK_FREQ / PWM_FREQUENCY)
/* Max duty cycle for the IR emitter is IFmax/IFpeak = 0.071, 2x safety margin. */
#define TCRT1000_DC 0.03
#define ON_MEASUREMENT_POS 0.02
#define OFF_MEASUREMENT_POS 0.5
#define NUM_IR_SENSORS 13

#define PROXIMITY_ADC_SAMPLE_TIME ADC_SAMPLE_112
#define DMA_BUFFER_SIZE 16

#define ADC2_NB_CHANNELS 1

#define EXTSEL_TIM8_CH1 0x0d

static unsigned int adc3_values[PROXIMITY_NB_CHANNELS] = {0};
static unsigned int adc2_values[ADC2_NB_CHANNELS] = {0};
static BSEMAPHORE_DECL(adc3_ready, true);
static BSEMAPHORE_DECL(adc2_ready, true);

static adcsample_t adc3_proximity_samples[PROXIMITY_NB_CHANNELS * DMA_BUFFER_SIZE];
static adcsample_t adc2_proximity_samples[1 * DMA_BUFFER_SIZE];

static void adc_cb(ADCDriver *adcp, adcsample_t *samples, size_t n)
{
    (void) adcp;
    (void) samples;
    (void) n;

    unsigned int *values = NULL;
    binary_semaphore_t *sem = NULL;

    /* First select the buffer and semaphore to store processing results in. */
    if (adcp == &ADCD3) {
        values = adc3_values;
        sem = &adc3_ready;
    } else if (adcp == &ADCD2) {
        values = adc2_values;
        sem = &adc2_ready;
    } else {
        chSysHalt("ADC callback: Unknown ADC");
    }

    /* Reset all samples to zero. */
    memset(values, 0, adcp->grpp->num_channels * sizeof(unsigned int));

    /* Compute the average over the different measurements. */
    for (size_t j = 0; j < n; j++) {
        for (size_t i = 0; i < adcp->grpp->num_channels; i++) {
            values[i] += samples[adcp->grpp->num_channels * j + i];
        }
    }

    for (size_t i = 0; i < adcp->grpp->num_channels; i++) {
        values[i] /= n;
    }

    /* Stops the automatic conversions and signal the proximity thread that the
     * ADC measurements are done. */
    chSysLockFromISR();
    adcStopConversionI(adcp);
    chBSemSignalI(sem);
    chSysUnlockFromISR();
}

static const ADCConversionGroup adcgrpcfg2 = {
    .circular = true,
    .num_channels = ADC2_NB_CHANNELS,
    .end_cb = adc_cb,
    .error_cb = NULL,

    /* Discontinuous mode with 1 conversion per trigger. */
    .cr1 = ADC_CR1_DISCEN,

    /* External trigger on timer 8 CC1. */
    .cr2 = ADC_CR2_EXTEN_1 | ADC_CR2_EXTSEL_SRC(EXTSEL_TIM8_CH1),

    /* Sampling duration, all set to PROXIMITY_ADC_SAMPLE_TIME. */
    .smpr1 = 0,
    .smpr2 = ADC_SMPR2_SMP_AN0(PROXIMITY_ADC_SAMPLE_TIME),

    /* On ADC2 we only have one proximity sensor. */
    .sqr1 = ADC_SQR1_NUM_CH(ADC2_NB_CHANNELS),
    .sqr2 = 0,
    .sqr3 = ADC_SQR3_SQ1_N(14), // IR_AN12
};


static const ADCConversionGroup adcgrpcfg3 = {
    .circular = true,
    .num_channels = PROXIMITY_NB_CHANNELS,
    .end_cb = adc_cb,
    .error_cb = NULL,

    /* Discontinuous mode with 1 conversion per trigger. */
    .cr1 = ADC_CR1_DISCEN,

    /* External trigger on timer 8 CC1. */
    .cr2 = ADC_CR2_EXTEN_1 | ADC_CR2_EXTSEL_SRC(EXTSEL_TIM8_CH1),

    /* Sampling duration, all set to PROXIMITY_ADC_SAMPLE_TIME. */
    .smpr2 = ADC_SMPR2_SMP_AN0(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN1(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN2(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN3(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN4(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN5(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN6(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN7(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN8(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR2_SMP_AN9(PROXIMITY_ADC_SAMPLE_TIME),
    .smpr1 = ADC_SMPR1_SMP_AN10(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR1_SMP_AN11(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR1_SMP_AN12(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR1_SMP_AN13(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR1_SMP_AN14(PROXIMITY_ADC_SAMPLE_TIME) |
             ADC_SMPR1_SMP_AN15(PROXIMITY_ADC_SAMPLE_TIME),

    /* Channels are defined starting from front (range sensor) and turning
     * counter clock wise. */
    /* Proximity sensors channels. */
    .sqr3 = ADC_SQR3_SQ1_N(12) |
            ADC_SQR3_SQ2_N(13) |
            ADC_SQR3_SQ3_N(11) |
            ADC_SQR3_SQ4_N(14) |
            ADC_SQR3_SQ5_N(15) |
            ADC_SQR3_SQ6_N(6),
    .sqr2 = ADC_SQR2_SQ7_N(5) |
    /* Ground sensors channels. */
            ADC_SQR2_SQ8_N(8) |
            ADC_SQR2_SQ9_N(2) | /* IR_IN14 is on ADC2, this is only a dummy. */
            ADC_SQR2_SQ10_N(10) |
            ADC_SQR2_SQ11_N(4) |
            ADC_SQR2_SQ12_N(7),
    .sqr1 = ADC_SQR1_SQ13_N(9) |
            ADC_SQR1_NUM_CH(PROXIMITY_NB_CHANNELS)
};

/** Takes a single measurement on all channels and stores the result in the provided array. */
static void take_measurement(unsigned int result[PROXIMITY_NB_CHANNELS])
{
    int i;

    /* Gets exclusive access to the ADCs. */
    adcAcquireBus(&ADCD3);
    adcAcquireBus(&ADCD2);

    /* Starts a new conversion. */
    adcStartConversion(&ADCD3, &adcgrpcfg3, adc3_proximity_samples, DMA_BUFFER_SIZE);
    adcStartConversion(&ADCD2, &adcgrpcfg2, adc2_proximity_samples, DMA_BUFFER_SIZE);

    /* Wait for the conversions to be done. */
    chBSemWait(&adc3_ready);
    chBSemWait(&adc2_ready);

    /* Copies the results in the provided arrays. */
    for (i = 0; i < PROXIMITY_NB_CHANNELS; i++) {
        /* The 8th sensor is the only one on ADC2. */
        if (i == 8) {
            result[i] = adc2_values[0];
        } else {
            result[i] = adc3_values[i];
        }
    }

    /* Gets exclusive access to the ADCs. */
    adcReleaseBus(&ADCD2);
    adcReleaseBus(&ADCD3);
}

/** Sets the position in the LED waveform (0..1) at which the measurement will be taken. */
static void set_measurement_position(float pos)
{
    pwmEnableChannel(&PWMD8, 0, (pwmcnt_t) (PWM_CYCLE * pos));
}

static THD_FUNCTION(proximity_thd, arg)
{
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    /* Declares the topic on the bus. */
    TOPIC_DECL(proximity_topic, proximity_msg_t);
    messagebus_advertise_topic(&bus, &proximity_topic.topic, "/proximity");

    while (true) {
        proximity_msg_t msg;

        set_measurement_position(OFF_MEASUREMENT_POS);
        take_measurement(msg.ambient);

        set_measurement_position(ON_MEASUREMENT_POS);
        take_measurement(msg.reflected);

        for (int i = 0; i < PROXIMITY_NB_CHANNELS; i++) {
            msg.delta[i] = msg.reflected[i] - msg.ambient[i];
        }

        palTogglePad(GPIOE, GPIOE_LED_STATUS);

        messagebus_topic_publish(&proximity_topic.topic, &msg, sizeof(msg));
    }
}

void proximity_start(void)
{
    static const PWMConfig pwmcfg_proximity = {
        /* timer clock frequency */
        .frequency = PWM_CLK_FREQ,
        /* timer period */
        .period = PWM_CYCLE,
        .cr2 = 0,

        /* Enable DMA event generation on channel 1. */
        .dier = TIM_DIER_CC1DE,
        .callback = NULL,
        .channels = {
                     /* Channel 1 is used to generate ADC triggers. It must be in output
                      * mode, although it is not routed to any pin. */
                     {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL},

                     /* Channel 2N is used to generate TCRT1000 drive signals. */
                     {.mode = PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH, .callback = NULL},
                     {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
                     {.mode = PWM_OUTPUT_DISABLED, .callback = NULL},
        },
    };

    /* Init PWM */
    pwmStart(&PWMD8, &pwmcfg_proximity);

    /* Set duty cycle for TCRT1000 drivers. */
    pwmEnableChannel(&PWMD8, 1, (pwmcnt_t) (PWM_CYCLE * TCRT1000_DC));

    static THD_WORKING_AREA(proximity_thd_wa, 2048);
    chThdCreateStatic(proximity_thd_wa, sizeof(proximity_thd_wa), NORMALPRIO, proximity_thd, NULL);
}
