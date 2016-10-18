#include <ch.h>
#include <hal.h>
#include "motor_pid.h"
#include "motor_pwm.h"
#include "pid/pid.h"
#include "sensors/battery_level.h"
#include "msgbus/messagebus.h"
#include "main.h"


static void right_wheel_voltage_set(float voltage)
{
    static battery_msg_t battery_msg = {0.0f};
    static messagebus_topic_t *topic = NULL;

    if (topic == NULL) {
        topic = messagebus_find_topic(&bus, "/battery_level");
    } else {
        messagebus_topic_read(topic, &battery_msg, sizeof(battery_msg));
        float duty_cycle = voltage / battery_msg.voltage;
        motor_right_pwm_set(duty_cycle);
    }
}

static void left_wheel_voltage_set(float voltage)
{
    static battery_msg_t battery_msg = {0.0f};
    static messagebus_topic_t *topic = NULL;

    if (topic == NULL) {
        topic = messagebus_find_topic(&bus, "/battery_level");
    } else {
        messagebus_topic_read(topic, &battery_msg, sizeof(battery_msg));
        float duty_cycle = voltage / battery_msg.voltage;
        motor_left_pwm_set(duty_cycle);
    }
}

static THD_FUNCTION(motor_pid_thd, arg)
{
    (void) arg;
    while (42) {
        chThdSleepMilliseconds(100);
    }
}

void motor_pid_start(void)
{
    static THD_WORKING_AREA(motor_pid_thd_wa, 512);
    chThdCreateStatic(motor_pid_thd_wa, sizeof(motor_pid_thd_wa), NORMALPRIO, motor_pid_thd, NULL);
}
