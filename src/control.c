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
#include "analogic.h"
#include "sensors/encoder.h"
#include "parameter/parameter.h"
#include "communication.h"


struct pid_parameter_s {
    parameter_namespace_t root;
    parameter_t kp;
    parameter_t ki;
    parameter_t kd;
    parameter_t ilimit;
};


struct motor_s {
    parameter_namespace_t root;
    const char *id;

    struct cascade_controller cascade;
    struct feedback feedback;
    struct setpoints setpoints;
    float pwm_input;

    struct pid_parameter_s pid_position;
    struct pid_parameter_s pid_velocity;
    struct pid_parameter_s pid_current;
};


static struct motor_s left;
static struct motor_s right;

static THD_WORKING_AREA(waThreadControl, 128);
static THD_FUNCTION(ThreadControl, arg) {

    (void)arg;
    chRegSetThreadName("motor state");
    while (TRUE) {

        /*Parameters*/

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


static void pid_register(struct pid_parameter_s *pid,
                         parameter_namespace_t *parent,
                         const char *name)
{

    parameter_namespace_declare(&pid->root, parent, name);
    parameter_scalar_declare_with_default(&pid->kp, &pid->root, "kp", 0);
    parameter_scalar_declare_with_default(&pid->ki, &pid->root, "ki", 0);
    parameter_scalar_declare_with_default(&pid->kd, &pid->root, "kd", 0);
    parameter_scalar_declare_with_default(&pid->ilimit, &pid->root, "ilimit", 0);
}


void motor_init(struct motor_s *motor, const char *id)
{
    cascade_init(&(motor->cascade));
    setpoints_init(&(motor->setpoints));
    motor->id = id;

    /*Parameters*/
    parameter_namespace_declare(&(motor->root), &parameter_root, motor->id);
    pid_register(&(motor->pid_position), &(motor->root), "position");
    pid_register(&(motor->pid_velocity), &(motor->root), "velocity");
    pid_register(&(motor->pid_current), &(motor->root), "current");


}


void control_start(void)
{
    motor_init(&left, "left");
    motor_init(&right, "right");

    motor_pwm_start();
    analogic_start(1,0,0);
    encoder_init();

    /*PID gains init*/


    chThdCreateStatic(waThreadControl, sizeof(waThreadControl), NORMALPRIO, ThreadControl, NULL);
}


void control_test(void)
{
    //setpoints_set_velocity(&(right.setpoints), 3000);
}