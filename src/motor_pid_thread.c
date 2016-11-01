#include <ch.h>
#include <hal.h>
#include "motor_pid.h"

void right_wheel_voltage_set(messagebus_t *bus, float voltage)
{
    static battery_msg_t battery_msg = {0.0f};
    static messagebus_topic_t *battery_topic = NULL;
    static motor_voltage_msg_t voltage_msg = {0.0f, 0.0f};
    static messagebus_topic_t *voltage_topic = NULL;

    if (battery_topic == NULL) {
        battery_topic = messagebus_find_topic(bus, "/battery_level");
    }

    if (battery_topic != NULL) {
        messagebus_topic_read(battery_topic, &battery_msg, sizeof(battery_msg));
        float duty_cycle = voltage / battery_msg.voltage;
        motor_right_pwm_set(duty_cycle);

        if (voltage_topic == NULL) {
            voltage_topic = messagebus_find_topic(bus, "/motors/voltage");
        }

        messagebus_topic_read(voltage_topic, &voltage_msg,
                              sizeof(voltage_msg));
        voltage_msg.right = voltage;
        messagebus_topic_publish(voltage_topic, &voltage_msg, sizeof(voltage_msg));
    }
}

void left_wheel_voltage_set(messagebus_t *bus, float voltage)
{
    static battery_msg_t battery_msg = {0.0f};
    static messagebus_topic_t *battery_topic = NULL;
    static motor_voltage_msg_t voltage_msg = {0.0f, 0.0f};
    static messagebus_topic_t *voltage_topic = NULL;

    if (battery_topic == NULL) {
        battery_topic = messagebus_find_topic(bus, "/battery_level");
    }

    if (battery_topic != NULL) {
        messagebus_topic_read(battery_topic, &battery_msg, sizeof(battery_msg));
        float duty_cycle = voltage / battery_msg.voltage;
        motor_left_pwm_set(duty_cycle);

        if (voltage_topic == NULL) {
            voltage_topic = messagebus_find_topic(bus, "/motors/voltage");
        }

        messagebus_topic_read(voltage_topic, &voltage_msg,
                              sizeof(voltage_msg));
        voltage_msg.left = voltage;
        messagebus_topic_publish(voltage_topic, &voltage_msg, sizeof(voltage_msg));
    }
}


static THD_FUNCTION(motor_pid_thd, arg)
{
    (void) arg;

    TOPIC_DECL(motor_voltage_topic, motor_voltage_msg_t);
    messagebus_advertise_topic(&bus, &motor_voltage_topic.topic, "/motors/voltage");

    while (42) {
        chThdSleepMilliseconds(100);
    }
}

void motor_pid_start(void)
{
    static THD_WORKING_AREA(motor_pid_thd_wa, 512);
    chThdCreateStatic(motor_pid_thd_wa, sizeof(motor_pid_thd_wa), NORMALPRIO, motor_pid_thd, NULL);
}
