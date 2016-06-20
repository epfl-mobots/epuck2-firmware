#ifndef RANGE_SENSOR_H
#define RANGE_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    uint8_t raw_mm;
    float raw;
} range_t;

void range_get_range(float *range);
void range_start(void);
void range_init(I2CDriver *dev);

#ifdef __cplusplus
}
#endif

#endif /* RANGE_SENSOR_H */
