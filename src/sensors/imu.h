#ifndef IMU_H
#define IMU_H

#ifdef __cplusplus
extern "C" {
#endif

void imu_start(void);
void imu_init(void);

typedef struct {
    float rate[3];
} gyrometer_sample_t;

typedef struct {
    float acceleration[3];
} accelerometer_sample_t;

void imu_get_gyro(float *gyro);
void imu_get_acc(float *acc);

extern event_source_t imu_events;

#define IMU_EVENT_READING        (1<<0)


#ifdef __cplusplus
}
#endif



#endif /* IMU_H */
