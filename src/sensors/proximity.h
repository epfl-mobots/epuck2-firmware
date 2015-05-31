#ifndef PROXIMITY_H
#define PROXIMITY_H

#ifdef __cplusplus
extern "C" {
#endif

#define TIM8_CCMR1  0x18

void proximity_start(void);
int proximity_change_adc_trigger(void);
void proximity_get(float* proximity);


#ifdef __cplusplus
}
#endif

#endif /* PROXIMITY_H */