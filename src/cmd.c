#include <string.h>
#include <stdlib.h>
#include <hal.h>
#include <test.h>
#include <chprintf.h>
#include <shell.h>
#include "usbconf.h"
#include "sensors/imu.h"
#include "sensors/encoder.h"
#include "sensors/range.h"
#include "motor_pwm.h"
#include "ff.h"
#include "main.h"

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

    messagebus_topic_t *encoders_topic;
    encoders_msg_t values;

    encoders_topic = messagebus_find_topic_blocking(&bus, "/encoders");
    messagebus_topic_wait(encoders_topic, &values, sizeof(values));

    chprintf(chp, "left: %ld\r\nright: %ld\r\n", values.left, values.right);
}

static void cmd_reboot(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) chp;
    (void) argc;
    (void) argv;
    NVIC_SystemReset();
}

static void cmd_sdcard(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) chp;
    (void) argc;
    (void) argv;

    FRESULT err;
    FATFS SDC_FS;

    chprintf(chp, "Starting SDC driver...\r\n");
    sdcStart(&SDCD1, NULL);
    chThdSleepMilliseconds(500);

    chprintf(chp, "Connecting to SDC...");
    while (true) {
        if (sdcConnect(&SDCD1) == 0) {
            chprintf(chp, "OK");
            break;
        } else {
            chprintf(chp, "FAILED");
        }
        chThdSleepMilliseconds(50);
    }
    chprintf(chp, "\r\n");

    chThdSleepMilliseconds(500);

    chprintf(chp, "Mouting card... ");
    err = f_mount(&SDC_FS, "", 1);
    if (err == FR_OK) {
        chprintf(chp, "OK");
    } else {
        chprintf(chp, "FAILED (err code=%d)", err);
    }

    chprintf(chp, "\r\n");

    sdcDisconnect(&SDCD1);
}

static void cmd_range(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    float distance_m;

    range_get_range(&distance_m);

    chprintf(chp, "distance = %.2f m", distance_m);
}

static void cmd_topics(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    chprintf(chp, "available topics:\r\n");

    MESSAGEBUS_TOPIC_FOREACH(&bus, topic) {
        chprintf(chp, "%s\r\n", topic->name);
    }
}

#define DAC_BUFFER_SIZE 360

/*
 * DAC test buffer (sine wave).
 */

#include "../sound.c"

/*
 * DAC streaming callback.
 */

static void end_cb1(DACDriver *dacp, const dacsample_t *buffer, size_t n);
static void error_cb1(DACDriver *dacp, dacerror_t err);

static const DACConversionGroup dacgrpcfg1 = {
  num_channels: 2U,
  end_cb:       end_cb1,
  error_cb:     error_cb1,
  trigger:      DAC_TRG(0)
};

size_t pos=0;
static void end_cb1(DACDriver *dacp, const dacsample_t *buffer, size_t n) {

  (void)dacp;



  memcpy(buffer, &sound_wav[pos], n);

  pos += n;
  if (pos > sound_wav_len) {
      pos = 0;
  }

}

/*
 * DAC error callback.
 */
static void error_cb1(DACDriver *dacp, dacerror_t err) {

  (void)dacp;
  (void)err;

  chSysHalt("DAC failure");
}

static const DACConfig dac1cfg1 = {
  init:         2047U,
  datamode:     DAC_DHRM_12BIT_RIGHT_DUAL
};


/*
 * GPT2 configuration.
 */
static const GPTConfig gpt6cfg1 = {
  frequency:    1000000U,
  callback:     NULL,
  cr2:          TIM_CR2_MMS_1,  /* MMS = 010 = TRGO on Update Event.        */
  dier:         0U
};

static void cmd_dac(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    (void) chp;
    palSetPadMode(GPIOA, 4, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOA, 5, PAL_MODE_INPUT_ANALOG);
    dacStart(&DACD1, &dac1cfg1);

    /*
     * Starting GPT6 driver, it is used for triggering the DAC.
     */
    gptStart(&GPTD6, &gpt6cfg1);

    /*
     * Starting a continuous conversion.
     * Note, the buffer size is divided by two because two elements are fetched
     * for each transfer.
     */
    dacStartConversion(&DACD1, &dacgrpcfg1, sound_wav, 1000);
    gptStartContinuous(&GPTD6, 2U);



}

const ShellCommand shell_commands[] = {
    {"test", cmd_test},
    {"range", cmd_range},
    {"topics", cmd_topics},
    {"dac", cmd_dac},
    {"pwm", cmd_motor},
    {"encoders", cmd_encoders},
    {"reboot", cmd_reboot},
    {"test", cmd_test},
    {"sdcard", cmd_sdcard},
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
