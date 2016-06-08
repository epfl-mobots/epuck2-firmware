#include <string.h>
#include <stdlib.h>
#include <hal.h>
#include <test.h>
#include <chprintf.h>
#include <shell.h>
#include "usbconf.h"
#include "sensors/imu.h"
#include "sensors/encoder.h"
#include "motor_pwm.h"

#define TEST_WA_SIZE        THD_WORKING_AREA_SIZE(256)
#define SHELL_WA_SIZE       THD_WORKING_AREA_SIZE(2048)

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[])
{
    thread_t *tp;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: test\r\n");
        return;
    }
    tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriorityX(),
                             TestThread, chp);
    if (tp == NULL) {
        chprintf(chp, "out of memory\r\n");
        return;
    }
    chThdWait(tp);
}

static void cmd_mpu6050(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    float acc[3];
    float gyro[3];
    imu_get_gyro(gyro);
    imu_get_acc(acc);
    chprintf(chp,
             "gyro: %f %f %f, acc %f %f %f\n",
             gyro[0],
             gyro[1],
             gyro[2],
             acc[0],
             acc[1],
             acc[2]);
}

static void cmd_motor(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc < 2) {
        chprintf(chp, "Usage: pwm left|right percentage\r\n");
        return;
    }

    float value = atoi(argv[1]) / 100.;
    int select;

    if (!strcmp("left", argv[0])) {
        select = 1;
    } else if (!strcmp("right", argv[0]))   {
        select = 0;
    } else {
        chprintf(chp, "Unknown motor \"%s\".\r\n", argv[0]);
        return;
    }

    motor_pwm_set(select, value);
}

static void cmd_encoders(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    chprintf(chp, "left: %ld\r\nright: %ld\r\n", encoder_get_left(), encoder_get_right());
}


const ShellCommand shell_commands[] = {
    {"test", cmd_test},
    {"pwm", cmd_motor},
    {"encoders", cmd_encoders},
    {"mpu6050", cmd_mpu6050},
    {NULL, NULL}
};

static THD_FUNCTION(shell_spawn_thd, p)
{
    (void) p;
    thread_t *shelltp = NULL;

    static const ShellConfig shell_cfg = {
        (BaseSequentialStream *)&SDU1,
        shell_commands
    };

    shellInit();


    while (TRUE) {
        if (!shelltp) {
            if (SDU1.config->usbp->state == USB_ACTIVE) {
                shelltp = shellCreate(&shell_cfg, SHELL_WA_SIZE, NORMALPRIO);
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


void shell_start(void)
{
    static THD_WORKING_AREA(wa, SHELL_WA_SIZE);

    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, shell_spawn_thd, NULL);
}
