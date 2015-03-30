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

    chThdCreateStatic(led_blinker_wa, sizeof(led_blinker_wa), NORMALPRIO, led_blinker, NULL);

    shellInit();

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
