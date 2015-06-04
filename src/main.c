#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include "shell.h"
#include "usbconf.h"
#include "sensors/imu.h"
#include "cmd.h"
#include "control.h"
#include "communication.h"

/*Testing includes*/
#include "analogic.h"
#include "motor_pwm.h"
#include "setpoints.h"
#include "sensors/proximity.h"




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
    control_start();
    control_test();
}


int main(void) {

    halInit();
    chSysInit();


    /*
    * Initializes a serial-over-USB CDC driver.
    */
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    /*
    * Activates the USB driver and then the USB bus pull-up on D+.
    * Note, a delay is inserted in order to not have to disconnect the cable
    * after a reset.
    */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1000);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);


    sdStart(&SD6, NULL);   

    communication_start((BaseSequentialStream *)&SDU1);

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    test_function();

    while (TRUE) {
        chThdSleepMilliseconds(500);
    }
}


