#include "ch.h"
#include "hal.h"
#include "sensors/mpu60X0.h"
#include "exti.h"
#include "imu.h"

#define IMU_INTERRUPT_EVENT		1


typedef struct {
    float rate[3];
} gyrometer_sample_t;

typedef struct {
    float acceleration[3];
} accelerometer_sample_t;


static mpu60X0_t 		mpu6050;
gyrometer_sample_t 		imu_gyro_sample;
accelerometer_sample_t 	imu_acc_sample;

event_source_t imu_events;

void imu_get_gyro(float *gyro)
{
    chSysLock();
    gyro[0] = imu_gyro_sample.rate[0];
    gyro[1] = imu_gyro_sample.rate[1];
    gyro[2] = imu_gyro_sample.rate[2];
    chSysUnlock();
}

void imu_get_acc(float *acc)
{
    chSysLock();
    acc[0] = imu_acc_sample.acceleration[0];
    acc[1] = imu_acc_sample.acceleration[1];
    acc[2] = imu_acc_sample.acceleration[2];
    chSysUnlock();
}

static THD_WORKING_AREA(imu_reader_thd_wa, 512);
static THD_FUNCTION(imu_reader_thd, arg) {

    (void)arg;
    static event_listener_t imu_int;
    float temp;
    chEvtRegisterMaskWithFlags(&exti_events, &imu_int,
                           (eventmask_t)IMU_INTERRUPT_EVENT,
                           (eventflags_t)EXTI_EVENT_MPU6000_INT);

    chRegSetThreadName("IMU_reader");


    i2cAcquireBus(dev);
    while(!mpu60X0_ping(&mpu6050));

    mpu60X0_setup(&mpu6050, MPU60X0_ACC_FULL_RANGE_2G
                          | MPU60X0_GYRO_FULL_RANGE_250DPS
                          | MPU60X0_SAMPLE_RATE_DIV(100)
                          | MPU60X0_LOW_PASS_FILTER_6);
    i2cReleaseBus(dev);


    static float gyro[3];
    static float acc[3];

    while (TRUE) {
        //MPU reading
        // chEvtWaitAny(IMU_INTERRUPT_EVENT);
        // chEvtGetAndClearFlags(&imu_int);
        chThdSleepMilliseconds(1);

        i2cAcquireBus(mpu6050.i2c);
        mpu60X0_read(&mpu6050, gyro, acc, &temp);
        i2cReleaseBus(mpu6050.i2c);

        chSysLock();
        imu_gyro_sample.rate[0] = gyro[0];
        imu_gyro_sample.rate[1] = gyro[1];
        imu_gyro_sample.rate[2] = gyro[2];
        imu_acc_sample.acceleration[0] = acc[0];
        imu_acc_sample.acceleration[1] = acc[1];
        imu_acc_sample.acceleration[2] = acc[2];
        chSysUnlock();

        chEvtBroadcastFlags(&imu_events, IMU_EVENT_READING);
    }
    return 0;
}

void imu_start(void)
{
    exti_setup();
    chEvtObjectInit(&imu_events);

    chThdCreateStatic(imu_reader_thd_wa, sizeof(imu_reader_thd_wa), NORMALPRIO, imu_reader_thd, NULL);
}

void imu_init(I2CDriver *dev)
{
    mpu60X0_init_using_i2c(&mpu6050, dev, 0);
}
