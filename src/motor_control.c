#include "motor_control.h"
#include "math.h"
#include "pid/pid.h"
#include "motor_pwm.h"
#include "ch.h"
#include "hal.h"


static THD_WORKING_AREA(waThdMotorLeft, 128);
static THD_FUNCTION(ThdMotorLeft, arg) {

 	(void)arg;
 	chRegSetThreadName("motor left");

 	pid_filter_t left_position_pid;
 	pid_filter_t left_velocity_pid;
 	pid_filter_t left_current_pid;

	while (TRUE) {

		// position control
	    float position_error = 0;
	    float position_output = 0;
	    position_output = pid_process(&left_position_pid, position_error);

	    // velocity control
	    float velocity_error = 0 + position_output;
	    float velocity_output = 0;
	    velocity_output = pid_process(&left_velocity_pid, velocity_error);

	    //torque control
	    float current_error = 0 + velocity_output;
	    float current_output = 0;
	    current_output = pid_process(&left_current_pid, current_error);

	    motor_pwm_set(FALSE, current_output);
	    chThdSleepMilliseconds(5);
  	}
}

static THD_WORKING_AREA(waThdMotorRight, 128);
static THD_FUNCTION(ThdMotorRight, arg) {

 	pid_filter_t right_position_pid;
 	pid_filter_t right_velocity_pid;
 	pid_filter_t right_current_pid;

 	(void)arg;
 	chRegSetThreadName("motor right");
 	while (TRUE) {

 		// position control
	    float position_error = 0;
	    float position_output = 0;
	    position_output = pid_process(&right_position_pid, position_error);

	    // velocity control
	    float velocity_error = 0 + position_output;
	    float velocity_output = 0;
	    velocity_output = pid_process(&right_velocity_pid, velocity_error);

	    //torque control
	    float current_error = 0 + velocity_output;
	    float current_output = 0;
	    current_output = pid_process(&right_current_pid, current_error);

		motor_pwm_set(TRUE, current_output);
		chThdSleepMilliseconds(5);
  	}
}

void motor_control_init(void)
{
	chThdCreateStatic(waThdMotorLeft, sizeof(waThdMotorLeft), NORMALPRIO, ThdMotorLeft, NULL);
	chThdCreateStatic(waThdMotorRight, sizeof(waThdMotorRight), NORMALPRIO, ThdMotorRight, NULL);
}