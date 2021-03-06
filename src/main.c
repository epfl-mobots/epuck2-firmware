#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ch.h>
#include <hal.h>
#include <chprintf.h>

#include "main.h"

#include "usbconf.h"
#include "shell.h"
#include "cmd.h"
#include "exti.h"


#include "motor_pwm.h"
#include "sensors/proximity.h"
#include "sensors/imu.h"
#include "sensors/range.h"
#include "sensors/encoder.h"
#include "sensors/battery_level.h"
#include "config_flash_storage.h"
#include "sdcard.h"
#include "sensors/motor_current.h"
#include "body_leds.h"
#include "motor_pid_thread.h"
#include "audio/audio_thread.h"

#include "aseba_vm/aseba_node.h"
#include "aseba_vm/skel_user.h"
#include "aseba_vm/aseba_can_interface.h"

#include "memory_protection.h"
#include "battery_protection.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

parameter_namespace_t parameter_root, aseba_ns;

static THD_FUNCTION(blinker_thd, arg)
{
    (void)arg;
    chRegSetThreadName(__FUNCTION__);
    while (TRUE) {
        palTogglePad(GPIOE, GPIOE_LED_HEARTBEAT);
        chThdSleepMilliseconds(500);
    }
}

static void blinker_start(void)
{
    // Start heartbeat thread
    static THD_WORKING_AREA(blinker_thd_wa, 128);
    chThdCreateStatic(blinker_thd_wa, sizeof(blinker_thd_wa), NORMALPRIO, blinker_thd, NULL);
}

void adc_start(void)
{
    adcStart(&ADCD3, NULL);
    adcStart(&ADCD2, NULL);
}

void i2c_start(void)
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
    usb_start();

    /** Inits the Inter Process Communication bus. */
    messagebus_init(&bus, &bus_lock, &bus_condvar);

    parameter_namespace_declare(&parameter_root, NULL, "");

    chprintf((BaseSequentialStream*)&SDU1, "boot");

    i2c_start();

    adc_start();

    battery_level_start();
    battery_protection_start();

    motor_pwm_start();
    encoder_start();

#ifdef USE_SERIAL_IP
    ip_start();
#else
    shell_start();
#endif
    blinker_start();
    body_leds_start();

    range_start();

    proximity_start();
    imu_start();
    exti_start();
    motor_current_start();
    motor_pid_start();

    // Initialise aseba parameters
    parameter_namespace_declare(&aseba_ns, &parameter_root, "aseba");
    aseba_declare_parameters(&aseba_ns);

    /* Give enough time to make sure all services booted. */
    chThdSleepMilliseconds(100);

    /* Load parameter tree from flash. */
    extern uint32_t _config_start;
    config_load(&parameter_root, &_config_start);

    sdcard_start();
    sdcard_automount();

    audio_start();

    // Initialise Aseba node (CAN and VM)
    aseba_vm_init();
    aseba_can_start(&vmState);
    aseba_vm_start();

    while (TRUE) {
        sdcard_automount();
        chThdSleepMilliseconds(1000);
    }
}

uintptr_t __stack_chk_guard = 0xdeadbeef;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}

void context_switch_hook(void *ntp, void *otp)
{
    (void) otp;

    /* The main thread does not have the same memory layout as the other ones
       (it uses the process stack instead of its own stack), so we ignore it. */
    if (ntp == &ch.mainthread) {
        return;
    }

    mpu_configure_region(6,
                         /* we skip sizeof(thread_t) because the start of the working area is used by ChibiOS. */
                         ntp + sizeof(thread_t) + 32,
                         5, /* 32 bytes */
                         AP_NO_NO, /* no permission */
                         false);

}
