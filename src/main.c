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

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)



int main(void) {

    halInit();
    chSysInit();

    palClearPort(GPIOE,GPIOE_LED_STATUS);
	palClearPort(GPIOD,GPIOD_LED_ERROR);
	palClearPort(GPIOE,GPIOE_LED_HEARTBEAT);

	palSetPort(GPIOE,GPIOE_LED_STATUS);
	palSetPort(GPIOD,GPIOD_LED_ERROR);
	palSetPort(GPIOE,GPIOE_LED_HEARTBEAT);

}
