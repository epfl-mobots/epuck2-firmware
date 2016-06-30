#ifndef IMU_H
#define IMU_H

#include <ch.h>
#include <hal.h>

#ifdef __cplusplus
extern "C" {
#endif

void imu_start(void);
void imu_init(void);

extern event_source_t imu_events;

#define IMU_EVENT_READING        (1 << 0)


#ifdef __cplusplus
}
#endif



#endif /* IMU_H */
