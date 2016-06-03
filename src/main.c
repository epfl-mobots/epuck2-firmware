#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "memory_protection.h"

#include "chprintf.h"
#include "shell.h"
#include "usbconf.h"

#include "cmd.h"
#include "control.h"
#include "communication.h"

/* Testing includes */
#include "analogic.h"
#include "motor_pwm.h"
#include "setpoints.h"
#include "sensors/proximity.h"
#include "sensors/imu.h"
#include "sensors/range.h"

/* Aseba includes */
#include "aseba_vm/skel.h"
#include "aseba_vm/aseba_node.h"
#include "aseba_vm/aseba_can_interface.h"

#include "segway.h"


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

void i2c_init(void)
{
    /*
     * I2C configuration structure for MPU6000 & VL6180x.
     * Set it to 400kHz fast mode
     */
    static const I2CConfig i2c_cfg = {
        .op_mode = OPMODE_I2C,
        .clock_speed = 400000,
        .duty_cycle = FAST_DUTY_CYCLE_2
    };

    i2cStart(&I2CD1, &i2c_cfg);
}

/** Late init hook, called before c++ static constructors. */
void __late_init(void)
{
    mpu_init();

    /* C++ Static initializer requires working chibios. */
    halInit();
    chSysInit();
}


int main(void)
{
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

    chprintf((BaseSequentialStream*)&SDU1, "boot");

    sdStart(&SD6, NULL);


#if 0
    communication_start((BaseSequentialStream *)&SDU1);
#else
    shell_start();
#endif

    // Start heartbeat thread
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    // Initialise i2c
    i2c_init();

    // Initialise the time of flight range sensor
    range_init(&I2CD1);
    range_start();

    // Start control loops
    control_start();

    // Initialise Aseba node (CAN and VM)
    aseba_vm_init();
    aseba_can_start(&vmState);
    aseba_vm_start();

    while (TRUE) {
        chThdSleepMilliseconds(1000);
    }
}

uintptr_t __stack_chk_guard = 0xdeadbeef;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
