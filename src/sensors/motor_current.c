#include <ch.h>
#include <hal.h>
#include "main.h"
#include "motor_current.h"

#define MOTOR_NB_CHANNELS 2
#define DMA_BUFFER_SIZE (128)
static int32_t motor_value[MOTOR_NB_CHANNELS];

/* Semaphore used to signal the reading thread when a measurement is ready. */
static BSEMAPHORE_DECL(measurement_ready_sem, true);

/* Motor current topic. */
static messagebus_topic_t motor_current_topic;
static MUTEX_DECL(motor_current_topic_lock);
static CONDVAR_DECL(motor_current_topic_condvar);
static motor_current_msg_t motor_current_value;

static void adc_motor_cb(ADCDriver *adcp, adcsample_t *adc_motor_samples, size_t n)
{
    (void)adcp;
    uint32_t accumulator[2] = {0};

    size_t i;

    /* Computes the average over the whole period. */
    for (i = 0; i < n; i += MOTOR_NB_CHANNELS) {
        accumulator[0] += adc_motor_samples[i];
        accumulator[1] += adc_motor_samples[i + 1];
    }
    accumulator[0] /= (n / 2);
    accumulator[1] /= (n / 2);

    chSysLockFromISR();
    motor_value[0] = accumulator[0];
    motor_value[1] = accumulator[1];
    chSysUnlockFromISR();

    chBSemSignalI(&measurement_ready_sem);
}

static void motor_current_topic_create(const char *name)
{
    messagebus_topic_init(&motor_current_topic,
                          &motor_current_topic_lock,
                          &motor_current_topic_condvar,
                          &motor_current_value,
                          sizeof(motor_current_value));
    messagebus_advertise_topic(&bus, &motor_current_topic, name);
}

static void motor_current_start_adc(void)
{
    static adcsample_t adc_motor_samples[MOTOR_NB_CHANNELS * DMA_BUFFER_SIZE];
    static const ADCConversionGroup adcgrpcfg1 = {
        .circular = TRUE,
        .num_channels = MOTOR_NB_CHANNELS,
        .end_cb = adc_motor_cb,
        .error_cb = NULL,
        .cr1 = 0,
        .cr2 = ADC_CR2_SWSTART,
        .smpr1 = 0,
        .smpr2 = ADC_SMPR2_SMP_AN0(ADC_SAMPLE_112) |
                 ADC_SMPR2_SMP_AN2(ADC_SAMPLE_112),
        .sqr1 = ADC_SQR1_NUM_CH(MOTOR_NB_CHANNELS),
        .sqr2 = 0,
        .sqr3 = ADC_SQR3_SQ1_N(0) | ADC_SQR3_SQ2_N(2)
    };

    adcStart(&ADCD1, NULL);
    adcStartConversion(&ADCD1, &adcgrpcfg1, adc_motor_samples, DMA_BUFFER_SIZE);
}

static THD_FUNCTION(adc_motor_current, arg)
{
    (void)arg;
    chRegSetThreadName(__FUNCTION__);

    /* First create the topic on which the motor currents will be published. */
    motor_current_topic_create("/motors/current");

    /* Then start the measurement itself. */
    motor_current_start_adc();


    while (1) {
        motor_current_msg_t msg;

        /* Wait for a new measurement to be available. */
        chBSemWait(&measurement_ready_sem);

        /* Convert the value to amperes. */
        chSysLock();
        msg.left = motor_value[0];
        msg.right = motor_value[1];
        chSysUnlock();

        // TODO: Convert it to amperes

        /* Publish them. */
        messagebus_topic_publish(&motor_current_topic, &msg, sizeof(msg));
    }
}

void motor_current_start(void)
{
    static THD_WORKING_AREA(adc_motor_current_wa, 128);
    chThdCreateStatic(adc_motor_current_wa,
                      sizeof(adc_motor_current_wa),
                      HIGHPRIO,
                      adc_motor_current,
                      NULL);
}