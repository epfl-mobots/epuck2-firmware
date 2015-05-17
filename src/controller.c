#include "controller.h"
#include "math.h"
#include "pid/pid.h"


pid_filter_t legs_position_pid;
pid_filter_t legs_velocity_pid;
pid_filter_t legs_current_pid;


void controller_legs_init(void)
{

    pid_set_gains(&legs_position_pid, 0, 0, 0);
    pid_set_gains(&legs_velocity_pid, 0, 0, 0);
    pid_set_gains(&legs_current_pid, 0, 0, 0);

    pid_set_integral_limit(&legs_position_pid, 0);
    pid_set_integral_limit(&legs_velocity_pid, 0);
    pid_set_integral_limit(&legs_current_pid, 0);

}

void controller_legs(void)
{
    // position control
    float position_error = 0;
    float position_output = 0;
    position_output = pid_process(&legs_position_pid, position_error);

    // velocity control
    float velocity_error = 0 + position_output;
    float velocity_output = 0;
    velocity_output = pid_process(&legs_velocity_pid, velocity_error);

    //torque control
    float current_error = 0 + velocity_output;
    float current_output = 0;
    current_output = pid_process(&legs_current_pid, current_error);
}
