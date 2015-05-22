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
#include "motor_pwm.h"
#include "analogic.h"
#include "sensors/encoder.h"

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

    encoder_init();
    motor_pwm_start();
    analogic_start(1,0,0);

    float test = 0.4;
    int enc = 0;

    motor_pwm_set(FALSE, -test);

    while(TRUE) {
        enc = encoder_get_left();

        if(enc > 20000) {
            palSetPad(GPIOE, GPIOE_LED_STATUS);
            motor_pwm_set(FALSE, test);
        }
        if(enc < 500){
            palClearPad(GPIOE, GPIOE_LED_STATUS);
            motor_pwm_set(FALSE, -test);
        }

        chThdSleepMilliseconds(200);
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


