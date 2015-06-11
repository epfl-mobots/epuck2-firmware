#ifndef IMU_H
#define IMU_H

#ifdef __cplusplus
extern "C" {
#endif

void imu_start(void);
void imu_init(I2CDriver *dev);

void imu_get_gyro(float *gyro);
void imu_get_acc(float *acc);

extern event_source_t imu_events;

#define IMU_EVENT_READING        (1<<0)


#ifdef __cplusplus
}
#endif



#endif /* IMU_H */
