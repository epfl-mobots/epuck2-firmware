#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "shell.h"
#include "usbcfg.h"
#include "sensors/imu.h"

#define TEST_WA_SIZE        THD_WORKING_AREA_SIZE(256)

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

extern int proximity_main(void);

static void cmd_adctest(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    static int analog_input;
    analog_input=proximity_main();
    chprintf(chp, "adctest value: %i", analog_input);
}


static void cmd_mpu6050(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    float *acc = NULL;
    float *gyro =NULL;
    imu_get_gyro(gyro);
    imu_get_acc(acc);
    chprintf(chp, "gyro: %f %f %f, acc %f %f %f\n", gyro[0], gyro[1], gyro[2], acc[0], acc[1], acc[2]);
}


const ShellCommand shell_commands[] = {
    {"mem", cmd_mem},
    {"threads", cmd_threads},
    {"test", cmd_test},
    {"adctest", cmd_adctest},
    {"mpu6050", cmd_mpu6050},
    {NULL, NULL}
};
