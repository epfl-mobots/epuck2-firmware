#include "ch.h"
#include "hal.h"
#include "sensors/mpu60X0.h"
#include "exti.h"
#include "imu.h"

#define IMU_INTERRUPT_EVENT		1


static mpu60X0_t 		mpu6050;
gyrometer_sample_t 		imu_gyro_sample;
accelerometer_sample_t 	imu_acc_sample;

event_source_t imu_events;


void mpu6050_read(float *gyro, float *acc)
{
    float temp;
    mpu60X0_read(&mpu6050, gyro, acc, &temp);
}

void imu_get_gyro(float *gyro)
{
    gyro[0] = imu_gyro_sample.rate[0];
    gyro[1] = imu_gyro_sample.rate[1];
    gyro[2] = imu_gyro_sample.rate[2];
}

void imu_get_acc(float *acc)
{
    acc[0] = imu_acc_sample.acceleration[0];
    acc[1] = imu_acc_sample.acceleration[1];
    acc[2] = imu_acc_sample.acceleration[2];
}

static THD_WORKING_AREA(imu_reader_thd_wa, 128);
static THD_FUNCTION(imu_reader_thd, arg) {

    (void)arg;
    static event_listener_t imu_int;
    chEvtRegisterMaskWithFlags(&exti_events, &imu_int,
                           (eventmask_t)IMU_INTERRUPT_EVENT,
                           (eventflags_t)EXTI_EVENT_MPU6000_INT);

    chRegSetThreadName("IMU_reader");

    while (TRUE) {
        //MPU reading
        chEvtWaitAny(IMU_INTERRUPT_EVENT);
        chEvtGetAndClearFlags(&imu_int);
        mpu6050_read(imu_gyro_sample.rate, imu_acc_sample.acceleration);
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


// PB9: I2C1_SDA (AF4)
// PB6: I2C1_SCL (AF4)
void imu_init(void)
{
    static const I2CConfig i2c_cfg = {
        .op_mode = OPMODE_I2C,
        .clock_speed = 400000,
        .duty_cycle = FAST_DUTY_CYCLE_2
    };

    chSysLock();
    palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_OPENDRAIN);
    palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_OPENDRAIN);
    chSysUnlock();

    i2cStart(&I2CD1, &i2c_cfg);

    mpu60X0_init_using_i2c(&mpu6050, &I2CD1, 0);
    mpu60X0_setup(&mpu6050, MPU60X0_ACC_FULL_RANGE_2G
                          | MPU60X0_GYRO_FULL_RANGE_250DPS
                          | MPU60X0_SAMPLE_RATE_DIV(100)
                          | MPU60X0_LOW_PASS_FILTER_6);
}
