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
#include "sensors/encoder.h"

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

        /*Feedback*/
        feedback_get(&(left.feedback), &(right.feedback));

        /*Errors*/
        left.cascade.position_error = setpoints_error_position(&(left.setpoints), &(left.feedback.position));
        left.cascade.velocity_error = setpoints_error_velocity(&(left.setpoints), left.feedback.velocity);
        left.cascade.current_error = setpoints_error_current(&(left.setpoints), left.feedback.current);

        right.cascade.position_error = setpoints_error_position(&(right.setpoints), &(right.feedback.position));
        right.cascade.velocity_error = setpoints_error_velocity(&(right.setpoints), right.feedback.velocity);
        right.cascade.current_error = setpoints_error_current(&(right.setpoints), right.feedback.current);

        /*Controller*/
        left.pwm_input = cascade_step(&(left.cascade));
        right.pwm_input = cascade_step(&(right.cascade));

        /*Output*/
        motor_pwm_set(FALSE, left.pwm_input);
        motor_pwm_set(TRUE, right.pwm_input);


        chThdSleepMilliseconds(2);
    }
    return 0;
};



void control_start(void)
{
    cascade_init(&(left.cascade), &(right.cascade));
    setpoints_init(&(left.setpoints), &(right.setpoints));
    motor_pwm_start();
    encoder_init();
    
    chThdCreateStatic(waThreadControl, sizeof(waThreadControl), NORMALPRIO, ThreadControl, NULL);
}

void control_test(void)
{
    setpoints_set_velocity(&(right.setpoints), -3000);
    setpoints_set_velocity(&(left.setpoints), -3000);

    while(TRUE) {

        chThdSleepMilliseconds(200);
    }
}