#include <ch.h>
#include <hal.h>
#include "sensors/proximity.h"

#define DMA_BUFFER_SIZE (128)

#define MOTOR_NB_CHANNELS 2

#define PROXIMITY_NB_CHANNELS_ADC3 12
#define PROXIMITY_NB_CHANNELS_ADC2 1

#define PROXIMITY_CB 1

static int32_t motor_value[MOTOR_NB_CHANNELS];
static int32_t proximity_value3[PROXIMITY_NB_CHANNELS_ADC3];
static int32_t proximity_value2;

event_source_t analogic_events;


void analog_get_motor(int32_t *value)
{
    chSysLock();
    value[0] = motor_value[0];
    value[1] = motor_value[1];
    chSysUnlock();
}

void analog_get_proximity(int32_t *value)
{
    int i;
	chSysLock();
    for (i = 0; i < PROXIMITY_NB_CHANNELS_ADC3; i++) {
        value[i] = proximity_value3[i];
    }
    value[PROXIMITY_NB_CHANNELS_ADC3] = proximity_value2;
	chSysUnlock();
}

static void adc_motor_cb(ADCDriver *adcp, adcsample_t *adc_motor_samples, size_t n)
{
    (void)adcp;
    uint32_t accumulator[2] = {0};

    int i;
    for (i = 0; i < (int)(n); i += MOTOR_NB_CHANNELS) {
    	accumulator[0] += adc_motor_samples[i];
        accumulator[1] += adc_motor_samples[i+1];
    }
    chSysLockFromISR();
    motor_value[0] = (accumulator[0] / DMA_BUFFER_SIZE);
    motor_value[1] = (accumulator[1] / DMA_BUFFER_SIZE);
    chSysUnlockFromISR();
}

static void adc2_proximity_cb(ADCDriver *adcp, adcsample_t *adc2_proximity_samples, size_t n)
{
    (void)adcp;
    uint32_t accumulator = 0;
    int i;
    if(proximity_change_adc_trigger()) {

        for (i = 0; i < (int)(n); i += PROXIMITY_NB_CHANNELS_ADC2) {
            accumulator += adc2_proximity_samples[i];
        }

        chSysLockFromISR();
        for (i = 0; i < PROXIMITY_NB_CHANNELS_ADC2; i++) {
            proximity_value2 = (accumulator / DMA_BUFFER_SIZE);
        }
        chSysUnlockFromISR();
    }
}

static void adc3_proximity_cb(ADCDriver *adcp, adcsample_t *adc3_proximity_samples, size_t n)
{
    (void)adcp;
    uint32_t accumulator[PROXIMITY_NB_CHANNELS_ADC3] = {0};

    int i, j;
    for (i = 0; i < (int)(n); i += PROXIMITY_NB_CHANNELS_ADC3) {
        for (j = 0; j < PROXIMITY_NB_CHANNELS_ADC3; i++) {
            accumulator[j] += adc3_proximity_samples[i+j];
        }
    }

    chSysLockFromISR();
    for (i = 0; i < PROXIMITY_NB_CHANNELS_ADC3; i++) {
        proximity_value3[i] = (accumulator[i] / DMA_BUFFER_SIZE);
    }
    chSysUnlockFromISR();
    chEvtBroadcastFlags(&analogic_events, PROXIMITY_CB);
}

static THD_WORKING_AREA(adc_motor_current_wa, 128);
static THD_FUNCTION(adc_motor_current, arg)
{
    (void)arg;
    chRegSetThreadName("adc motor current");

    static adcsample_t adc_motor_samples[MOTOR_NB_CHANNELS * DMA_BUFFER_SIZE];
    static const ADCConversionGroup adcgrpcfg1 = {
        TRUE,                   							// circular
        MOTOR_NB_CHANNELS,      							// nb channels
        adc_motor_cb,	           						     // callback fn
        NULL,                   							// error callback fn
        0,                      							// CR1
        ADC_CR2_SWSTART,			            			// CR2
        0,		                                			//SMPR1
        ADC_SMPR2_SMP_AN0(ADC_SAMPLE_112) |
        ADC_SMPR2_SMP_AN2(ADC_SAMPLE_112),   				// SMPR2
        ADC_SQR1_NUM_CH(MOTOR_NB_CHANNELS),     		    // SQR1
        0,                      							// SQR2
        ADC_SQR3_SQ1_N(0) | ADC_SQR3_SQ2_N(2)       		// SQR3 
        //PC1 (IN11) and PC2(IN12)
    };


    palSetGroupMode(GPIOA, 0 | 2, 0, PAL_MODE_INPUT_ANALOG);

    adcStart(&ADCD1, NULL);

    adcStartConversion(&ADCD1, &adcgrpcfg1, adc_motor_samples, DMA_BUFFER_SIZE); // should never return

    return 0;
}


static THD_WORKING_AREA(adc2_proximity_current_wa, 128);
static THD_FUNCTION(adc2_proximity_current, arg)
{
    (void)arg;
    chRegSetThreadName("adc2 proyimity sensors");

    static adcsample_t adc2_proximity_samples[PROXIMITY_NB_CHANNELS_ADC2 * DMA_BUFFER_SIZE];
    static const ADCConversionGroup adcgrpcfg2 = {
        TRUE,                                               // circular
        PROXIMITY_NB_CHANNELS_ADC2,                         // nb channels
        adc2_proximity_cb,                                  // callback fn
        NULL,                                               // error callback fn
        ADC_CR1_DISCEN | ADC_CR1_DISCNUM_0,                 // CR1 -> disontinuous mode with one conversion per trigger
        0,                                                  // CR2 -> doesn't start and doesn't activate continuous
        /*SMPR1*/
        ADC_SMPR1_SMP_AN14(ADC_SAMPLE_112),                 //PC4 - IR_AN12
        /*SMPR2*/         
        0,            
        /*SQR1*/
        ADC_SQR1_NUM_CH(PROXIMITY_NB_CHANNELS_ADC2),
        /*SQR2*/
        0,        
        /*SQR3*/
        ADC_SQR3_SQ1_N(14),              
    };


    palSetGroupMode(GPIOC, 4,
                  0, PAL_MODE_INPUT_ANALOG);

    adcStart(&ADCD2, NULL);

    adcStartConversion(&ADCD2, &adcgrpcfg2, adc2_proximity_samples, DMA_BUFFER_SIZE); // should never return

    return 0;
}

static THD_WORKING_AREA(adc3_proximity_current_wa, 128);
static THD_FUNCTION(adc3_proximity_current, arg)
{
    (void)arg;
    chRegSetThreadName("adc3 proyimity sensors");

    static adcsample_t adc3_proximity_samples[PROXIMITY_NB_CHANNELS_ADC3 * DMA_BUFFER_SIZE];
    static const ADCConversionGroup adcgrpcfg3 = {
        TRUE,                                               // circular
        PROXIMITY_NB_CHANNELS_ADC3,                                  // nb channels
        adc3_proximity_cb,                                       // callback fn
        NULL,                                               // error callback fn
        0,                                                  // CR1
        0,                                                  // CR2 -> doesn't start and doesn't activate continuous
        /*SMPR1*/
        ADC_SMPR1_SMP_AN10(ADC_SAMPLE_112) |  //PC0 - IR_AN8
        ADC_SMPR1_SMP_AN11(ADC_SAMPLE_112) |  //PC1 - IR_AN9
        ADC_SMPR1_SMP_AN12(ADC_SAMPLE_112) |  //PC2 - IR_AN10
        ADC_SMPR1_SMP_AN13(ADC_SAMPLE_112) |  //PC3 - IR_AN11
        ADC_SMPR1_SMP_AN14(ADC_SAMPLE_112) |  //PF4 - IR_AN1
        ADC_SMPR1_SMP_AN15(ADC_SAMPLE_112),   //PF5 - IR_AN2
        /*SMPR2*/         
        ADC_SMPR2_SMP_AN4(ADC_SAMPLE_112) |   //PF6 - IR_AN3
        ADC_SMPR2_SMP_AN5(ADC_SAMPLE_112) |   //PF7 - IR_AN4
        ADC_SMPR2_SMP_AN6(ADC_SAMPLE_112) |   //PF8 - IR_AN5
        ADC_SMPR2_SMP_AN7(ADC_SAMPLE_112) |   //PF9 - IR_AN6
        ADC_SMPR2_SMP_AN8(ADC_SAMPLE_112) |   //PF10 - IR_AN7
        ADC_SMPR2_SMP_AN9(ADC_SAMPLE_112),    //PF3 - IR_AN0              
        /*SQR1*/
        ADC_SQR1_NUM_CH(PROXIMITY_NB_CHANNELS_ADC3),
        /*SQR2*/
        ADC_SQR2_SQ7_N(7) | ADC_SQR2_SQ8_N(8) |
        ADC_SQR2_SQ9_N(10) | ADC_SQR2_SQ10_N(11) |
        ADC_SQR2_SQ11_N(12) | ADC_SQR2_SQ12_N(13),        
        /*SQR3*/
        ADC_SQR3_SQ1_N(9) | ADC_SQR3_SQ2_N(14) |
        ADC_SQR3_SQ3_N(15) | ADC_SQR3_SQ4_N(4) |
        ADC_SQR3_SQ5_N(5) | ADC_SQR3_SQ6_N(6),              
    };


    palSetGroupMode(GPIOF, 3 | 4 | 5 | 6 | 7 | 8 | 9| 10,
                  0, PAL_MODE_INPUT_ANALOG);
    palSetGroupMode(GPIOC, 0 | 1 | 2 | 3,
                  0, PAL_MODE_INPUT_ANALOG);

    adcStart(&ADCD3, NULL);

    adcStartConversion(&ADCD3, &adcgrpcfg3, adc3_proximity_samples, DMA_BUFFER_SIZE); // should never return

    return 0;
}

void analogic_start(bool adc1, bool adc2, bool adc3)
{
    chEvtObjectInit(&analogic_events);
    if (adc1)
        chThdCreateStatic(adc_motor_current_wa, sizeof(adc_motor_current_wa), HIGHPRIO, adc_motor_current, NULL);
    if (adc2)
    chThdCreateStatic(adc2_proximity_current_wa, sizeof(adc2_proximity_current_wa), HIGHPRIO, adc2_proximity_current, NULL);
    if (adc3)
    chThdCreateStatic(adc3_proximity_current_wa, sizeof(adc3_proximity_current_wa), HIGHPRIO, adc3_proximity_current, NULL);
}
