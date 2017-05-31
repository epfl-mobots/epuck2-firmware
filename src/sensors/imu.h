#ifndef IMU_H
#define IMU_H
#ifdef __cplusplus
extern "C" {
#endif

/** Message containing one measurement from the IMU. */
typedef struct {
    float acceleration[3];
    float roll_rate[3];
    float theta;
} imu_msg_t;

/** Starts the Inertial Motion Unit (IMU) publisher. */
void imu_start(void);

#ifdef __cplusplus
}
#endif
#endif /* IMU_H */
