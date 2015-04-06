#ifndef IMU_H
#define IMU_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

void imu_start(void);
void imu_setup(void);

typedef struct {
	float rate[3];
} gyrometer_sample_t;

typedef struct {
	float rate[3];
} accelerometer_sample_t;

extern gyrometer_sample_timu_   gyro_sample;
extern accelerometer_sample_t   imu_acc_sample;

#endif /* IMU_H */
