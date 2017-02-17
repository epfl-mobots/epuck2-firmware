#include <ch.h>
#include <hal.h>
#include "main.h"
#include "motor_current.h"

#define ADC_MAX 4096

#if defined(MOT_CURRENT_USE_AD628)
#define ADC_GAIN  (3.3 / (4096 * 0.33 * 5.15));
#elif defined(MOT_CURRENT_USE_INA282)
#define ADC_GAIN  (3.3 / (4096 * 0.033 * 50));
#else
#error "Please select your motor current consumption type in board.h"
#endif

#define MOTOR_NB_CHANNELS 2
#define DMA_BUFFER_SIZE (128)

/* Trigger ADC on Timer 4, Channel 4. */
#define EXTSEL_TIM4_CC4 0x9

static int32_t motor_value[MOTOR_NB_CHANNELS];

/* Semaphore used to signal the reading thread when a measurement is ready. */
static BSEMAPHORE_DECL(measurement_ready_sem, true);

/* Motor current topic. */
static TOPIC_DECL(motor_current_topic, motor_current_msg_t);

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
    chBSemSignalI(&measurement_ready_sem);
    chSysUnlockFromISR();
}

static void motor_current_topic_create(const char *name)
{
    messagebus_topic_init(&motor_current_topic.topic,
                          &motor_current_topic.lock,
                          &motor_current_topic.condvar,
                          &motor_current_topic.value,
                          sizeof(motor_current_topic.value));
    messagebus_advertise_topic(&bus, &motor_current_topic.topic, name);
}

static void motor_current_start_adc(void)
{
    static adcsample_t adc_motor_samples[MOTOR_NB_CHANNELS * DMA_BUFFER_SIZE];

    static const ADCConversionGroup adcgrpcfg1 = {
        .circular = TRUE,
        .num_channels = MOTOR_NB_CHANNELS,
        .end_cb = adc_motor_cb,
        .error_cb = NULL,

        /* Discontinuous mode, 2 conversions per trigger. */
        .cr1 = ADC_CR1_DISCEN | ADC_CR1_DISCNUM_0,

        /* External trigger on timer 4 CC4 event. */
        .cr2 = ADC_CR2_EXTEN_1 | ADC_CR2_EXTSEL_SRC(EXTSEL_TIM4_CC4),

        /* Sample time registers
         *
         * The ADC clock is 10.5 Mhz (APB2 @ 84 Mhz / 8).
         * Since PWM frequency is 44.1 Khz, and we want to sample in about 5%
         * of the PWM cycle (i.e. 1.13 us).
         * 15 cycles + 15 (for internal ADC use) cycles -> 0.35 per sample,
         * meaning two samples fit.
         */
        .smpr2 = ADC_SMPR2_SMP_AN0(ADC_SAMPLE_15) |
                 ADC_SMPR2_SMP_AN2(ADC_SAMPLE_15),
        .smpr1 = 0,

        /* Sequence registers */
        .sqr3 = ADC_SQR3_SQ1_N(0) | ADC_SQR3_SQ2_N(2),
        .sqr2 = 0,
        .sqr1 = ADC_SQR1_NUM_CH(MOTOR_NB_CHANNELS),
    };

    adcStart(&ADCD1, NULL);
    adcStartConversion(&ADCD1, &adcgrpcfg1, adc_motor_samples, DMA_BUFFER_SIZE);
}

static THD_FUNCTION(adc_motor_current, arg)
{
    (void)arg;
    chRegSetThreadName(__FUNCTION__);

    static parameter_namespace_t current_ns;

    static struct {
        parameter_namespace_t ns;
        parameter_t is_inverted;
    } left_params, right_params;

    parameter_namespace_declare(&current_ns, &parameter_root, "current_sense");
    parameter_namespace_declare(&left_params.ns, &current_ns, "left");
    parameter_namespace_declare(&right_params.ns, &current_ns, "right");

    parameter_boolean_declare_with_default(&left_params.is_inverted,
                                           &left_params.ns,
                                           "is_inverted",
                                           false);
    parameter_boolean_declare_with_default(&right_params.is_inverted,
                                           &right_params.ns,
                                           "is_inverted",
                                           false);

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

        /* Convert the current to amperes. */
        msg.left = (msg.left - (ADC_MAX / 2)) * ADC_GAIN;
        msg.right = (msg.right - (ADC_MAX / 2)) * ADC_GAIN;

        /* Invert current measurements if required. */
        if (parameter_boolean_get(&left_params.is_inverted)) {
            msg.left = -msg.left;
        }
        if (parameter_boolean_get(&right_params.is_inverted)) {
            msg.right = -msg.right;
        }

        /* Publish them. */
        messagebus_topic_publish(&motor_current_topic.topic, &msg, sizeof(msg));
    }
}

void motor_current_start(void)
{
    static THD_WORKING_AREA(adc_motor_current_wa, 256);
    chThdCreateStatic(adc_motor_current_wa,
                      sizeof(adc_motor_current_wa),
                      HIGHPRIO,
                      adc_motor_current,
                      NULL);
}
