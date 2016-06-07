#include <string.h>
#include <stdlib.h>
#include <hal.h>
#include <test.h>
#include <chprintf.h>
#include <shell.h>
#include "usbconf.h"
#include "sensors/imu.h"
#include "motor_pwm.h"

#define TEST_WA_SIZE        THD_WORKING_AREA_SIZE(256)
#define SHELL_WA_SIZE       THD_WORKING_AREA_SIZE(2048)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
    size_t n, size;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: mem\r\n");
        return;
    }
    n = chHeapStatus(NULL, &size);
    chprintf(chp, "core free memory : %u bytes\r\n", chCoreGetStatusX());
    chprintf(chp, "heap fragments     : %u\r\n", n);
    chprintf(chp, "heap free total    : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
    static const char *states[] = {CH_STATE_NAMES};
    thread_t *tp;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: threads\r\n");
        return;
    }
    chprintf(chp, "        addr        stack prio refs         state\r\n");
    tp = chRegFirstThread();
    do {
        chprintf(chp, "%08lx %08lx %4lu %4lu %9s\r\n",
                 (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
                 (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
                 states[tp->p_state]);
        tp = chRegNextThread(tp);
    } while (tp != NULL);
}

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

static void cmd_readclock(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    chprintf(chp, "SYSCLK: %i \n HCLK: %i \n PCLK1  %i \n PCLK2 %i \n",
             STM32_SYSCLK, STM32_HCLK, STM32_PCLK1, STM32_PCLK2);
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


const ShellCommand shell_commands[] = {
    {"mem", cmd_mem},
    {"threads", cmd_threads},
    {"test", cmd_test},
    {"pwm", cmd_motor},
    {"mpu6050", cmd_mpu6050},
    {"clock", cmd_readclock},
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
    static THD_WORKING_AREA(wa, 2048);

    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, shell_spawn_thd, NULL);
}
