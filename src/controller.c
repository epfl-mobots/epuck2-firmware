#include "controller.h"
#include "math.h"
#include "pid/pid.h"
#include "motor_pwm.h"


void controller_start(bool mode)
{
    if(!mode) {
        void motor_control_init();
    }
}