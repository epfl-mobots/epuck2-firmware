#include "controller.h"
#include "math.h"
#include "pid/pid.h"
#include "motor_pwm.h"


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


void controller_start(bool mode)
{
    if(!mode) {
        controller_legs_init();
        motor_cascade_step();
    }
}