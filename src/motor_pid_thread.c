#include <ch.h>
#include <hal.h>
#include "main.h"
#include "sensors/battery_level.h"
#include "sensors/motor_current.h"
#include "sensors/encoder.h"
#include "motor_pid_thread.h"
#include "motor_controller.h"
#include "motor_pwm.h"

#define CONTROL_FREQUENCY_HZ 100


enum motor_enum {
    LEFT=0,
    RIGHT=1
};

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

static float get_current(void *arg)
{
    messagebus_topic_t *topic;
    motor_current_msg_t msg;
    topic = messagebus_find_topic(&bus, "/motors/current");

    if (topic == NULL) {
        return 0.;
    }

    if (messagebus_topic_read(topic, &msg, sizeof(msg)) == false) {
        return 0.;
    }

    if ((int)arg == LEFT) {
        return msg.left;
    } else {
        return msg.right;
    }
}

static float get_velocity(void *arg)
{
    messagebus_topic_t *topic;
    wheel_velocities_msg_t msg;
    topic = messagebus_find_topic(&bus, "/wheel_velocities");

    if (topic == NULL) {
        return 0.;
    }

    if (messagebus_topic_read(topic, &msg, sizeof(msg)) == false) {
        return 0.;
    }

    if ((int)arg == LEFT) {
        return msg.left;
    } else {
        return msg.right;
    }
}

static float get_position(void *arg)
{
    messagebus_topic_t *topic;
    wheel_pos_msg_t msg;
    topic = messagebus_find_topic(&bus, "/wheel_pos");

    if (topic == NULL) {
        return 0.;
    }

    if (messagebus_topic_read(topic, &msg, sizeof(msg)) == false) {
        return 0.;
    }

    if ((int)arg == LEFT) {
        return msg.left;
    } else {
        return msg.right;
    }
}

static void set_input_functions(motor_controller_t *controller, enum motor_enum mot)
{
    controller->current.get = get_current;
    controller->current.get_arg = (void *)mot;
    controller->velocity.get = get_velocity;
    controller->velocity.get_arg = (void *)mot;
    controller->position.get = get_position;
    controller->position.get_arg = (void *)mot;
}

static void wait_for_services(void)
{
    messagebus_find_topic_blocking(&bus, "/motors/current");
    messagebus_find_topic_blocking(&bus, "/wheel_velocities");
    messagebus_find_topic_blocking(&bus, "/wheel_pos");
    messagebus_find_topic_blocking(&bus, "/battery_level");
}

static THD_FUNCTION(motor_pid_thd, arg)
{
    (void) arg;

    chRegSetThreadName(__FUNCTION__);

    struct {
        motor_controller_t controller;
        parameter_namespace_t ns;
        float voltage;
    } left, right;


    parameter_namespace_declare(&left.ns, &parameter_root, "left_wheel");
    parameter_namespace_declare(&right.ns, &parameter_root, "right_wheel");

    motor_controller_init(&left.controller, &left.ns);
    motor_controller_init(&right.controller, &right.ns);

    /* Set default parameters. */
    parameter_scalar_set(parameter_find(&left.ns, "control/current/kp"), 2.);
    parameter_scalar_set(parameter_find(&right.ns, "control/current/kp"), 2.);
    parameter_scalar_set(parameter_find(&left.ns, "control/current/ki"), 40.);
    parameter_scalar_set(parameter_find(&right.ns, "control/current/ki"), 40.);
    parameter_scalar_set(parameter_find(&left.ns, "control/current/i_limit"), 50.);
    parameter_scalar_set(parameter_find(&right.ns, "control/current/i_limit"), 50.);

    parameter_scalar_set(parameter_find(&left.ns, "control/velocity/kp"), 0.2);
    parameter_scalar_set(parameter_find(&right.ns, "control/velocity/kp"), 0.2);

    parameter_scalar_set(parameter_find(&left.ns, "control/velocity/kp"), 0.2);
    parameter_scalar_set(parameter_find(&right.ns, "control/velocity/kp"), 0.2);

    parameter_scalar_set(parameter_find(&right.ns, "control/limits/current"), 2.);
    parameter_scalar_set(parameter_find(&left.ns, "control/limits/current"), 2.);
    parameter_scalar_set(parameter_find(&right.ns, "control/limits/acceleration"), 10 * 3.14);
    parameter_scalar_set(parameter_find(&left.ns, "control/limits/acceleration"), 10 * 3.14);
    parameter_scalar_set(parameter_find(&right.ns, "control/limits/velocity"), 20 * 3.14);
    parameter_scalar_set(parameter_find(&left.ns, "control/limits/velocity"), 20 * 3.14);

    /* Set the intput output functions for the controllers. */
    set_input_functions(&left.controller, LEFT);
    set_input_functions(&right.controller, RIGHT);

    motor_controller_set_frequency(&left.controller, CONTROL_FREQUENCY_HZ);
    motor_controller_set_frequency(&right.controller, CONTROL_FREQUENCY_HZ);

    TOPIC_DECL(motor_voltage_topic, motor_voltage_msg_t);
    messagebus_advertise_topic(&bus, &motor_voltage_topic.topic, "/motors/voltage");

    TOPIC_DECL(wheels_setpoint_topic, wheels_setpoint_t);
    messagebus_advertise_topic(&bus, &wheels_setpoint_topic.topic, "/motors/setpoint");

    /* Wait for needed services to come online. */
    wait_for_services();

    while (true) {
        wheels_setpoint_t msg;
        if (messagebus_topic_read(&wheels_setpoint_topic.topic, &msg, sizeof(msg))) {
            motor_controller_set_mode(&left.controller, msg.mode);
            motor_controller_set_mode(&right.controller, msg.mode);
            switch (msg.mode) {
                case MOTOR_CONTROLLER_CURRENT:
                    left.controller.current.target_setpoint = msg.left;
                    right.controller.current.target_setpoint = msg.right;
                    break;

                case MOTOR_CONTROLLER_VELOCITY:
                    left.controller.velocity.target_setpoint = msg.left;
                    right.controller.velocity.target_setpoint = msg.right;
                    break;

                case MOTOR_CONTROLLER_POSITION:
                    left.controller.position.target_setpoint = msg.left;
                    right.controller.position.target_setpoint = msg.right;
                    break;

                default:
                    chSysHalt("unknown control mode.");
                    break;
            }
        }
        left.voltage = motor_controller_process(&left.controller);
        right.voltage = motor_controller_process(&right.controller);

        left_wheel_voltage_set(&bus, left.voltage);
        right_wheel_voltage_set(&bus, right.voltage);

        chThdSleepMilliseconds(1000 / CONTROL_FREQUENCY_HZ);
    }
}

void motor_pid_start(void)
{
    static THD_WORKING_AREA(motor_pid_thd_wa, 8192);
    chThdCreateStatic(motor_pid_thd_wa, sizeof(motor_pid_thd_wa), NORMALPRIO, motor_pid_thd, NULL);
}
