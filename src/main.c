#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "shell.h"
#include "usbcfg.h"
#include "cmd.h"
#include "mpu60X0.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)


// blink orange led
static THD_WORKING_AREA(led_blinker_wa, 128);
static THD_FUNCTION(led_blinker, arg) {

    (void)arg;
    chRegSetThreadName("blinker");
    while (TRUE) {
        palSetPad(GPIOD, GPIOD_LED3);
        chThdSleepMilliseconds(500);
        palClearPad(GPIOD, GPIOD_LED3);
        chThdSleepMilliseconds(500);
    }
    return 0;
}



static mpu60X0_t mpu6050;

// PB7: I2C1_SDA (AF4)
// PB8: I2C1_SCL (AF4)
static void mpu6050_setup(void)
{
    static const I2CConfig i2c_cfg = {
        .op_mode = OPMODE_I2C,
        .clock_speed = 400000,
        .duty_cycle = FAST_DUTY_CYCLE_2
    };

    chSysLock();
    palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_OPENDRAIN);
    palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_OPENDRAIN);
    chSysUnlock();

    i2cStart(&I2CD1, &i2c_cfg);

    mpu60X0_init_using_i2c(&mpu6050, &I2CD1, 0);
    mpu60X0_setup(&mpu6050, MPU60X0_ACC_FULL_RANGE_2G
                          | MPU60X0_GYRO_FULL_RANGE_250DPS
                          | MPU60X0_SAMPLE_RATE_DIV(100)
                          | MPU60X0_LOW_PASS_FILTER_6);
}

void mpu6050_read(float *gyro, float *acc)
{
    float temp;
    mpu60X0_read(&mpu6050, gyro, acc, &temp);
}


int main(void) {
    thread_t *shelltp = NULL;

    halInit();
    chSysInit();

    // UART2 on PA2(TX) and PA3(RX)
    sdStart(&SD2, NULL);
    palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

    // serial-over-USB CDC driver.
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1000);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    mpu6050_setup();

    chThdCreateStatic(led_blinker_wa, sizeof(led_blinker_wa), NORMALPRIO, led_blinker, NULL);

    shellInit();

    static const ShellConfig shell_cfg1 = {
        (BaseSequentialStream *)&SDU1,
        shell_commands
    };

    while (TRUE) {
        if (!shelltp) {
            if (SDU1.config->usbp->state == USB_ACTIVE) {
                shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
            }
        } else {
            if (chThdTerminatedX(shelltp)) {
                chThdRelease(shelltp);
                shelltp = NULL;
            }
        }
        chThdSleepMilliseconds(500);
    }
}
