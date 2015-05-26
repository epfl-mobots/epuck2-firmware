#include "control.h"
#include "math.h"
#include "hal.h"
#include "ch.h"
#include "pid/pid.h"
#include "motor_csp.h"
#include "motor_pwm.h"
#include "pid_cascade.h"
#include "feedback.h"

static struct cascade_controller cascade_left;
static struct cascade_controller cascade_right;

static THD_WORKING_AREA(waThreadControl, 128);
static THD_FUNCTION(ThreadControl, arg) {

  (void)arg;
  chRegSetThreadName("motor state");
  while (TRUE) {
        chThdSleepMilliseconds(5);
    }
    return 0;
};



void control_start(void)
{
    cascade_init(cascade_left, cascade_right);
    chThdCreateStatic(waThreadControl, sizeof(waThreadControl), NORMALPRIO, ThreadControl, NULL);
}