#include <ch.h>
#include <hal.h>

#define DMA_BUFFER_SIZE (128)
/***** MOTOR THREAD *****/
#define MOTOR_NB_CHANNELS 2
#define MOTOR_ADC_BASE 	GPIOC
#define PROXIMITY_NB_CHANNELS 7

static int32_t motor_value[2];
static int32_t proximity_value[2];


void analog_get_motor(int32_t *value)
{
    chSysLock();
    value[0] = proximity_value[0];
    value[0] = proximity_value[0];
    chSysUnlock();
}

void analog_get_proximity(int32_t *value)
{
	chSysLock();
	value[0] = proximity_value[0];
	value[1] = proximity_value[1];
    value[2] = proximity_value[2];
    value[3] = proximity_value[3];
    value[4] = proximity_value[4];
    value[5] = proximity_value[5];
    value[6] = proximity_value[6];
	chSysUnlock();
}

static void adc_motor_cb(ADCDriver *adcp, adcsample_t *adc_motor_samples, size_t n)
{
    (void)adcp;
    uint32_t accumulator1 = 0;
    uint32_t accumulator2 = 0;

    int i;
    for (i = 0; i < (int)(n); i += MOTOR_NB_CHANNELS) {
    	accumulator1 += adc_motor_samples[i];
        accumulator2 += adc_motor_samples[i+1];
    }
    chSysLockFromISR();
    motor_value[0] = (accumulator1 / DMA_BUFFER_SIZE);
    motor_value[1] = (accumulator2 / DMA_BUFFER_SIZE);
    chSysUnlockFromISR();
}

static void adc_proximity_cb(ADCDriver *adcp, adcsample_t *adc_proximity_samples, size_t n)
{
    (void)adcp;
    uint32_t accumulator1 = 0;
    uint32_t accumulator2 = 0;

    int i;
    for (i = 0; i < (int)(n); i += PROXIMITY_NB_CHANNELS) {
        accumulator1 += adc_proximity_samples[i];
        accumulator2 += adc_proximity_samples[i+1];
    }
    chSysLockFromISR();
    proximity_value[0] = (accumulator1 / DMA_BUFFER_SIZE);
    proximity_value[1] = (accumulator2 / DMA_BUFFER_SIZE);
    chSysUnlockFromISR();
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
        adc_proximity_cb,	           						// callback fn
        NULL,                   							// error callback fn
        0,                      							// CR1
        ADC_CR2_SWSTART,			            							// CR2
        ADC_SMPR1_SMP_AN11(ADC_SAMPLE_112) | 
        ADC_SMPR1_SMP_AN12(ADC_SAMPLE_112),					//SMPR1
        0,   												// SMPR2
        ADC_SQR1_NUM_CH(2),     							// SQR1
        0,                      							// SQR2
        ADC_SQR3_SQ1_N(11) | ADC_SQR3_SQ2_N(12)       		// SQR3 
        //PC1 (IN11) and PC2(IN12)
    };



    //palSetGroupMode(MOTOR_ADC_BASE, 1 | 2,
	//              0, PAL_MODE_INPUT_ANALOG);

    //adcStart(&ADCD1, NULL);

    //adcStartConversion(&ADCD1, &adcgrpcfg1, adc_motor_samples, DMA_BUFFER_SIZE); // should never return

    return 0;
}

static THD_WORKING_AREA(adc_proximity_current_wa, 128);
static THD_FUNCTION(adc_proximity_current, arg)
{
    (void)arg;
    chRegSetThreadName("adc proyimity sensors");

    static adcsample_t adc_motor_samples[PROXIMITY_NB_CHANNELS * DMA_BUFFER_SIZE];
    static const ADCConversionGroup adcgrpcfg2 = {
        TRUE,                                               // circular
        PROXIMITY_NB_CHANNELS,                                  // nb channels
        adc_motor_cb,                                       // callback fn
        NULL,                                               // error callback fn
        0,                                                  // CR1
        ADC_CR2_SWSTART,                                                    // CR2
        ADC_SMPR1_SMP_AN11(ADC_SAMPLE_112) | 
        ADC_SMPR1_SMP_AN12(ADC_SAMPLE_112),                 //SMPR1
        0,                                                  // SMPR2
        ADC_SQR1_NUM_CH(2),                                 // SQR1
        0,                                                  // SQR2
        ADC_SQR3_SQ1_N(11) | ADC_SQR3_SQ2_N(12)             // SQR3 
        //PC1 (IN11) and PC2(IN12)
    };



    palSetGroupMode(MOTOR_ADC_BASE, 1 | 2,
                  0, PAL_MODE_INPUT_ANALOG);

    adcStart(&ADCD1, NULL);

    adcStartConversion(&ADCD1, &adcgrpcfg1, adc_motor_samples, DMA_BUFFER_SIZE); // should never return

    return 0;
}


void analogic_start(void)
{
    chThdCreateStatic(adc_motor_current_wa, sizeof(adc_motor_current_wa), HIGHPRIO, adc_motor_current, NULL);
}
