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
#include "sensors/proximity.h"

/*Testing includes*/
#include "analogic.h"
#include "motor_pwm.h"
#include "setpoints.h"


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
    float test[13];
    float test_old[13];
    int i;

    proximity_start();

    while(TRUE){
        for(i = 0; i < 13; i++){
            test_old[i] = test[i];
        }
        proximity_get(test);

        if(test[0] == test_old[0]) {
            palClearPad(GPIOE, GPIOE_LED_STATUS);
        }

        else {
            palSetPad(GPIOE, GPIOE_LED_STATUS);
        }
    }
}

int main(void) {

    halInit();
    chSysInit();

    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    test_function();

    while(TRUE) {
    	chThdSleepMilliseconds(1000);
    }
}


