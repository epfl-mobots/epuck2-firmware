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

extern gyrometer_sample_t       imu_gyro_sample;
extern accelerometer_sample_t   imu_acc_sample;

#ifdef __cplusplus
}
#endif



#endif /* IMU_H */
