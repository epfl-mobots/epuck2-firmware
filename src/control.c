#include "control.h"
#include "math.h"
#include "hal.h"
#include "ch.h"
#include "pid/pid.h"
#include "motor_csp.h"
#include "motor_pwm.h"
#include "pid_cascade.h"
#include "feedback.h"
#include "setpoints.h"

struct motor {
    struct cascade_controller cascade;
    struct feedback feedback;
    struct setpoints setpoints;
    float pwm_input;
};

static struct motor left;
static struct motor right;

static THD_WORKING_AREA(waThreadControl, 128);
static THD_FUNCTION(ThreadControl, arg) {

    (void)arg;
    chRegSetThreadName("motor state");
    while (TRUE) {

        /*Setpoints*/
        setpoints_get(&(left.setpoints), &(right.setpoints));

        /*Feedback*/
        feedback_get(&(left.feedback), &(right.feedback));

        /*Controller*/
        left.pwm_input = cascade_step(&(left.cascade));


        /*Output*/



        chThdSleepMilliseconds(5);
    }
    return 0;
};



void control_start(void)
{
    cascade_init(&(left.cascade), &(right.cascade));
    void motor_pwm_start(void);
    
    chThdCreateStatic(waThreadControl, sizeof(waThreadControl), NORMALPRIO, ThreadControl, NULL);
    chThdCreateStatic(waThreadControl, sizeof(waThreadControl), NORMALPRIO, ThreadControl, NULL);
}