#include "proximity.h"

#include "ch.h"
#include "hal.h"
#include "tcrt1000.h"


#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      1


static adcsample_t samples1[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];



/*
 * ADC conversion group.
 * Mode:        Continuous buffer, 1 channel, SW triggered, ADC-Prescaler 480
 * Channels:    IN2.
 */

static const ADCConversionGroup adcgrpcfg1 = {
  TRUE,
  ADC_GRP1_NUM_CHANNELS,
  NULL,
  NULL,
  0,                        /* CR1 */
  ADC_CR2_SWSTART,          /* CR2 */
  ADC_SMPR2_SMP_AN2(ADC_SAMPLE_480),
  0,                        /* SMPR2 */
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,		                 /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN2)
};


int proximity_main(void) {

	//Sets two pins used for testing tcrt1000
	palSetGroupMode(TCRT1000_GPIO_BASE, TCRT1000_GPIO_LED | TCRT1000_GPIO_VOLTAGE,
	              0, PAL_MODE_INPUT_ANALOG);

  adcStart(&ADCD1, NULL);

  adcConvert(&ADCD1, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
  chThdSleepMilliseconds(1000);



	return 0;
}