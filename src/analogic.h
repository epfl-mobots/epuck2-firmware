#ifndef ANALOGIC_H
#define ANALOGIC_H

#ifdef __cplusplus
extern "C" {
#endif

void analogic_start(bool adc1, bool adc2, bool adc3);
void analog_get_motor(int32_t *value);
void analog_get_proximity(int32_t *value_low, int32_t *value_high);


#ifdef __cplusplus
}
#endif

#endif /* ANALOGIC_H */
