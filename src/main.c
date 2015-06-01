#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "shell.h"
#include "usbcfg.h"
#include "sensors/imu.h"
#include "cmd.h"
#include "control.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("Heartbeat");
  while (TRUE) {
    palSetPad(GPIOE, GPIOE_LED_HEARTBEAT);       
    chThdSleepMilliseconds(300);
    palClearPad(GPIOE, GPIOE_LED_HEARTBEAT);
    chThdSleepMilliseconds(300);
  }
}

void test_function(void)
{


}



int main(void) {

    thread_t *shelltp = NULL;


    halInit();
    chSysInit();

    sdStart(&SD6, NULL);

    chprintf((BaseSequentialStream*)&SD6, "boot");

    static const ShellConfig shell_cfg1 = {
        (BaseSequentialStream *)&SD6,
        shell_commands
    };

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    test_function();

    while (TRUE) {
        if (!shelltp) {
            palSetPad(GPIOE, GPIOE_LED_STATUS);
            shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
        } else {
            palClearPad(GPIOE, GPIOE_LED_STATUS);
            if (chThdTerminatedX(shelltp)) {
                chThdRelease(shelltp);
                shelltp = NULL;
            }
        }
        chThdSleepMilliseconds(500);
    }
}


