#ifndef ANALOGIC_H
#define ANALOGIC_H

#ifdef __cplusplus
extern "C" {
#endif

void analogic_start(void);
void analog_get_motor(int32_t *value);
void analog_get_proximity(int32_t *value);


#ifdef __cplusplus
}
#endif

#endif /* ANALOGIC_H */