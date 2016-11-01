#include <ch.h>
#include <hal.h>
#include "motor_pid.h"
#include "motor_pwm.h"
#include "pid/pid.h"
#include "sensors/battery_level.h"
#include "msgbus/messagebus.h"
#include "main.h"
#include "parameter/parameter.h"
#include <math.h>


struct pid_param_s {
    parameter_t kp;
    parameter_t ki;
    parameter_t kd;
    parameter_t i_limit;
};

// control loop parameters
static parameter_namespace_t param_ns_control;
static parameter_t param_vel_limit;
static parameter_t param_torque_limit;
static parameter_t param_acc_limit;
static parameter_namespace_t param_ns_pos_ctrl;
static parameter_namespace_t param_ns_vel_ctrl;
static parameter_namespace_t param_ns_cur_ctrl;
static struct pid_param_s pos_pid_params;
static struct pid_param_s vel_pid_params;
static struct pid_param_s cur_pid_params;


static void right_wheel_voltage_set(float voltage)
{
    static battery_msg_t battery_msg = {0.0f};
    static messagebus_topic_t *battery_topic = NULL;
    static motor_voltage_msg_t voltage_msg = {0.0f, 0.0f};
    static messagebus_topic_t *voltage_topic = NULL;

    if (battery_topic == NULL) {
        battery_topic = messagebus_find_topic(&bus, "/battery_level");
    }

    if (battery_topic != NULL) {
        messagebus_topic_read(battery_topic, &battery_msg, sizeof(battery_msg));
        float duty_cycle = voltage / battery_msg.voltage;
        motor_right_pwm_set(duty_cycle);

        if (voltage_topic == NULL) {
            voltage_topic = messagebus_find_topic(&bus, "/motors/voltage");
        }

        messagebus_topic_read(voltage_topic, &voltage_msg,
                              sizeof(voltage_msg));
        voltage_msg.right = voltage;
        messagebus_topic_publish(voltage_topic, &voltage_msg, sizeof(voltage_msg));
    }
}

static void left_wheel_voltage_set(float voltage)
{
    static battery_msg_t battery_msg = {0.0f};
    static messagebus_topic_t *battery_topic = NULL;
    static motor_voltage_msg_t voltage_msg = {0.0f, 0.0f};
    static messagebus_topic_t *voltage_topic = NULL;

    if (battery_topic == NULL) {
        battery_topic = messagebus_find_topic(&bus, "/battery_level");
    }

    if (battery_topic != NULL) {
        messagebus_topic_read(battery_topic, &battery_msg, sizeof(battery_msg));
        float duty_cycle = voltage / battery_msg.voltage;
        motor_left_pwm_set(duty_cycle);

        if (voltage_topic == NULL) {
            voltage_topic = messagebus_find_topic(&bus, "/motors/voltage");
        }

        messagebus_topic_read(voltage_topic, &voltage_msg,
                              sizeof(voltage_msg));
        voltage_msg.left = voltage;
        messagebus_topic_publish(voltage_topic, &voltage_msg, sizeof(voltage_msg));
    }
}

static void pid_param_declare(struct pid_param_s *p, parameter_namespace_t *ns)
{
    parameter_scalar_declare_with_default(&p->kp, ns, "kp", 0);
    parameter_scalar_declare_with_default(&p->ki, ns, "ki", 0);
    parameter_scalar_declare_with_default(&p->kd, ns, "kd", 0);
    parameter_scalar_declare_with_default(&p->i_limit, ns, "i_limit", INFINITY);
}

static void pid_param_update(struct pid_param_s *p, pid_ctrl_t *ctrl)
{
    if (parameter_changed(&p->kp) ||
        parameter_changed(&p->ki) ||
        parameter_changed(&p->kd)) {
        pid_set_gains(ctrl, parameter_scalar_get(&p->kp),
                            parameter_scalar_get(&p->ki),
                            parameter_scalar_get(&p->kd));
        pid_reset_integral(ctrl);
    }
    if (parameter_changed(&p->i_limit)) {
        pid_set_integral_limit(ctrl, parameter_scalar_get(&p->i_limit));
    }
}


static void declare_parameters(void)
{
    parameter_namespace_declare(&param_ns_control, &parameter_root, "control");
    parameter_scalar_declare(&param_vel_limit, &param_ns_control, "velocity_limit");
    parameter_scalar_declare(&param_torque_limit, &param_ns_control, "torque_limit");
    parameter_scalar_declare(&param_acc_limit, &param_ns_control, "acceleration_limit");

    parameter_namespace_declare(&param_ns_pos_ctrl, &param_ns_control, "position");
    pid_param_declare(&pos_pid_params, &param_ns_pos_ctrl);

    parameter_namespace_declare(&param_ns_vel_ctrl, &param_ns_control, "velocity");
    pid_param_declare(&vel_pid_params, &param_ns_vel_ctrl);

    parameter_namespace_declare(&param_ns_cur_ctrl, &param_ns_control, "current");
    pid_param_declare(&cur_pid_params, &param_ns_cur_ctrl);
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
