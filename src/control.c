#include "control.h"
#include "math.h"
#include "hal.h"
#include "ch.h"
#include "pid/pid.h"
#include "motor_csp.h"
#include "motor_pwm.h"
#include "analogic.h"
#include "sensors/encoder.h"
#include "communication.h"



struct motor_s left;
struct motor_s right;


void update_pid_parameters(pid_ctrl_t *pid, struct pid_parameter_s *pid_parameter)
{
    float kp;
    float ki;
    float kd;
    float ilimit;
    if (parameter_namespace_contains_changed(&(pid_parameter->root))) {
        kp = parameter_scalar_get(&(pid_parameter->kp));
        ki = parameter_scalar_get(&(pid_parameter->ki));
        kd = parameter_scalar_get(&(pid_parameter->kd));
        ilimit = parameter_scalar_get(&(pid_parameter->ilimit));
        pid_set_gains(pid, kp, ki, kd);
        pid_set_integral_limit(pid, ilimit);
    }
}

void update_motor_parameters(struct motor_s *motor)
{
    update_pid_parameters(&(motor->cascade.position_pid), &(motor->position_pid));
    update_pid_parameters(&(motor->cascade.velocity_pid), &(motor->velocity_pid));
    update_pid_parameters(&(motor->cascade.current_pid), &(motor->current_pid));
}



static THD_WORKING_AREA(waThreadControl, 128);
static THD_FUNCTION(ThreadControl, arg) {

    (void)arg;
    chRegSetThreadName("motor state");

    while (TRUE) {

        /*Parameters*/
        update_motor_parameters(&left);
        update_motor_parameters(&right);

        /*Feedback*/
        feedback_get(&(left.feedback), &(right.feedback));

        /*Errors*/
        left.cascade.position_error =
            setpoints_error_position(&(left.setpoints), (left.feedback.position));
        left.cascade.velocity_error = setpoints_error_velocity(&(left.setpoints),
                                                               left.feedback.velocity);
        left.cascade.current_error = setpoints_error_current(&(left.setpoints),
                                                             left.feedback.current);

        right.cascade.position_error =
            setpoints_error_position(&(right.setpoints), (right.feedback.position));
        right.cascade.velocity_error = setpoints_error_velocity(&(right.setpoints),
                                                                right.feedback.velocity);
        right.cascade.current_error = setpoints_error_current(&(right.setpoints),
                                                              right.feedback.current);

        switch (setpoint_get_mode(&left.setpoints)) {
            case SETPT_MODE_POS:
                cascade_mode_pos_ctrl(&left.cascade);
                break;

            case SETPT_MODE_VEL:
                cascade_mode_vel_ctrl(&left.cascade);
                break;

            case SETPT_MODE_CUR:
                cascade_mode_cur_ctrl(&left.cascade);
                break;
        }

        switch (setpoint_get_mode(&right.setpoints)) {
            case SETPT_MODE_POS:
                cascade_mode_pos_ctrl(&right.cascade);
                break;

            case SETPT_MODE_VEL:
                cascade_mode_vel_ctrl(&right.cascade);
                break;

            case SETPT_MODE_CUR:
                cascade_mode_cur_ctrl(&right.cascade);
                break;
        }


        /*Controller*/
        left.pwm_input = cascade_step(&(left.cascade));
        right.pwm_input = cascade_step(&(right.cascade));

        /*Output*/
        motor_pwm_set(FALSE, left.pwm_input);
        motor_pwm_set(TRUE, right.pwm_input);


        chThdSleepMilliseconds(2);
    }
}


void pid_register(struct pid_parameter_s *pid,
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
    pid_register(&(motor->position_pid), &(motor->root), "position");
    pid_register(&(motor->velocity_pid), &(motor->root), "velocity");
    pid_register(&(motor->current_pid), &(motor->root), "current");


}


void control_start(void)
{
    motor_init(&left, "left");
    motor_init(&right, "right");

    motor_pwm_start();
    analogic_start(1, 0, 0);
    encoder_init();

    /*PID gains init*/
    parameter_scalar_set(&(left.position_pid.kp), 0.);
    parameter_scalar_set(&(left.position_pid.ki), 0.);
    parameter_scalar_set(&(left.position_pid.kd), 0.);

    parameter_scalar_set(&(left.velocity_pid.kp), 0.);
    parameter_scalar_set(&(left.velocity_pid.ki), 0.);
    parameter_scalar_set(&(left.velocity_pid.kd), 0.);

    parameter_scalar_set(&(left.current_pid.kp), 0.);
    parameter_scalar_set(&(left.current_pid.ki), 0.);
    parameter_scalar_set(&(left.current_pid.kd), 0.);

    parameter_scalar_set(&(right.position_pid.kp), 0.);
    parameter_scalar_set(&(right.position_pid.ki), 0.);
    parameter_scalar_set(&(right.position_pid.kd), 0.);

    parameter_scalar_set(&(right.velocity_pid.kp), 0.);
    parameter_scalar_set(&(right.velocity_pid.ki), 0.);
    parameter_scalar_set(&(right.velocity_pid.kd), 0.);

    parameter_scalar_set(&(right.current_pid.kp), 0.);
    parameter_scalar_set(&(right.current_pid.ki), 0.);
    parameter_scalar_set(&(right.current_pid.kd), 0.);


    chThdCreateStatic(waThreadControl, sizeof(waThreadControl), NORMALPRIO, ThreadControl, NULL);
}
