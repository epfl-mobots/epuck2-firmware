#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ch.h"
#include "hal.h"

#include "aseba_node.h"
#include "skel_user.h"

#include "vm/natives.h"
#include "common/productids.h"
#include "common/consts.h"
#include "main.h"
#include "config_flash_storage.h"
#include "motor_pwm.h"

#include "sensors/range.h"
#include "sensors/battery_level.h"
#include "sensors/encoder.h"
#include "sensors/motor_current.h"
#include "sensors/imu.h"
#include "audio/audio_thread.h"
#include "madgwick.h"

#include "motor_pid_thread.h"

#define MEAN_LENGTH 10

/* Struct used to share Aseba parameters between C-style API and Aseba. */
static parameter_t aseba_settings[SETTINGS_COUNT];
static char aseba_settings_name[SETTINGS_COUNT][10];

struct _vmVariables vmVariables;

/* Used to test if something was changed within aseba. */
struct _vmVariables previous_vars;

/* Test if an Aseba variable changed during the VM step. */
#define VM_VAR_CHANGED(var) (vmVariables.var != previous_vars.var)

#define READ_PARAM_TO_VM(var, param) do { \
        parameter_t *p = parameter_find(&parameter_root, param); \
        if (p == NULL) { \
            chSysHalt("Cannot find " param); \
        } \
        vmVariables.var = (int)(1000. * parameter_scalar_read(p)); \
} while (0);

#define WRITE_PARAM_FROM_VM(var, param) do { \
        if (VM_VAR_CHANGED(var)) { \
            parameter_t *p = parameter_find(&parameter_root, param); \
            if (p == NULL) { \
                chSysHalt("Cannot find " param); \
            } \
            parameter_scalar_set(p, (float)(vmVariables.var / 1000.)); \
        } \
} while (0);


void AsebaVMResetCB(AsebaVMState *vm)
{
    ASEBA_UNUSED(vm);

    vmVariables.motor_right_pwm = 0;
    vmVariables.motor_left_pwm = 0;

    for (int i = 0; i < BODY_LED_COUNT; i++) {
        vmVariables.leds[i] = 0;
    }
}

const AsebaVMDescription vmDescription = {
    BOARD_NAME,
    {
        // {Number of element in array, Name displayed in aseba studio}
        {1, "_id"},
        {1, "event.source"},
        {VM_VARIABLES_ARG_SIZE, "event.args"},
        {2, "_fwversion"},
        {1, "_productId"},

        {1, "battery"},
        {1, "range"},
        {PROXIMITY_NB_CHANNELS, "proximity.delta"},
        {PROXIMITY_NB_CHANNELS, "proximity.ambient"},
        {PROXIMITY_NB_CHANNELS, "proximity.reflected"},
        {1, "motor.left.pwm"},
        {1, "motor.right.pwm"},

        {1, "motor.left.current"},
        {1, "motor.right.current"},
        {1, "motor.left.velocity"},
        {1, "motor.right.velocity"},
        {1, "motor.left.position"},
        {1, "motor.right.position"},

        {2, "motor.left.enc"},
        {2, "motor.right.enc"},

        {3, "acc"},
        {3, "gyro"},
        {1, "theta"},
        {1, "phi"},
        {1, "psi"},

        {BODY_LED_COUNT, "leds"},

        {1, "motor.left.setpoint.current"},
        {1, "motor.right.setpoint.current"},
        {1, "motor.left.setpoint.velocity"},
        {1, "motor.right.setpoint.velocity"},
        {1, "motor.left.setpoint.position"},
        {1, "motor.right.setpoint.position"},

        /* Control parameters */
        {1, "_control.left.current.kp"},
        {1, "_control.left.current.ki"},
        {1, "_control.left.current.kd"},
        {1, "_control.left.current.ilimit"},
        {1, "_control.left.velocity.kp"},
        {1, "_control.left.velocity.ki"},
        {1, "_control.left.velocity.kd"},
        {1, "_control.left.velocity.ilimit"},
        {1, "_control.left.position.kp"},
        {1, "_control.left.position.ki"},
        {1, "_control.left.position.kd"},
        {1, "_control.left.position.ilimit"},

        {1, "_control.right.current.kp"},
        {1, "_control.right.current.ki"},
        {1, "_control.right.current.kd"},
        {1, "_control.right.current.ilimit"},
        {1, "_control.right.velocity.kp"},
        {1, "_control.right.velocity.ki"},
        {1, "_control.right.velocity.kd"},
        {1, "_control.right.velocity.ilimit"},
        {1, "_control.right.position.kp"},
        {1, "_control.right.position.ki"},
        {1, "_control.right.position.kd"},
        {1, "_control.right.position.ilimit"},


        {0, NULL}
    }
};

// Event descriptions
const AsebaLocalEventDescription localEvents[] = {
    {"range", "New range measurement"},
    {"proximity", "New proximity measurement"},
    {"encoders", "New motor encoders measurement"},
    {"imu", "New IMU (gyro and acc) measurement"},
    {"timer", "Periodic event"},
    {"sound.finished", "A sound finished playing"},
    {"sound.error", "There was an error during sound playback"},
    {NULL, NULL}
};

/** This thread is responsible for handling new range events for Aseba. */
static THD_FUNCTION(aseba_range_thd, p)
{
    (void) p;
    chRegSetThreadName(__FUNCTION__);

    messagebus_topic_t *topic;

    topic = messagebus_find_topic_blocking(&bus, "/range");

    while (true) {
        messagebus_topic_wait(topic, NULL, 0);

        chSysLock();
        SET_EVENT(EVENT_RANGE);
        chSysUnlock();
    }
}

/** This thread is responsible for handling new encoders events for Aseba. */
static THD_FUNCTION(aseba_encoders_thd, p)
{
    (void) p;
    chRegSetThreadName(__FUNCTION__);

    messagebus_topic_t *topic;

    topic = messagebus_find_topic_blocking(&bus, "/encoders");

    while (true) {
        messagebus_topic_wait(topic, NULL, 0);

        chSysLock();
        SET_EVENT(EVENT_ENCODERS);
        chSysUnlock();
    }
}

/** This thread is responsible for handling new proximity events for Aseba. */
static THD_FUNCTION(aseba_proximity_thd, p)
{
    (void) p;
    chRegSetThreadName(__FUNCTION__);

    messagebus_topic_t *topic;

    topic = messagebus_find_topic_blocking(&bus, "/proximity");

    while (true) {
        messagebus_topic_wait(topic, NULL, 0);
        chSysLock();
        SET_EVENT(EVENT_PROXIMITY);
        chSysUnlock();
    }
}

/** This thread is responsible for handling new IMU events for Aseba. */
static THD_FUNCTION(aseba_imu_thd, p)
{
    (void) p;
    chRegSetThreadName(__FUNCTION__);

    messagebus_topic_t *topic;

    topic = messagebus_find_topic_blocking(&bus, "/imu");

    while (true) {
        messagebus_topic_wait(topic, NULL, 0);
        chSysLock();
        SET_EVENT(EVENT_IMU);
        chSysUnlock();
    }
}

/** This thread is responsible for handling new audio events for Aseba. */
static THD_FUNCTION(aseba_audio_thd, p)
{
    (void) p;
    chRegSetThreadName(__FUNCTION__);
    messagebus_topic_t *topic;
    audio_play_result_t res;

    topic = messagebus_find_topic_blocking(&bus, "/audio/play/result");

    while (true) {
        messagebus_topic_wait(topic, &res, sizeof(res));

        if (res.status == AUDIO_OK) {
            chSysLock();
            SET_EVENT(EVENT_SOUND_PLAY_FINISHED);
            chSysUnlock();
        } else {
            chSysLock();
            SET_EVENT(EVENT_SOUND_ERROR);
            chSysUnlock();
        }
    }
}




static void aseba_timer_cb(void *p)
{
    virtual_timer_t *vt = (virtual_timer_t *)p;

    chSysLockFromISR();

    SET_EVENT(EVENT_TIMER);
    chVTSetI(vt, MS2ST(100), aseba_timer_cb, p);

    chSysUnlockFromISR();
}

void aseba_variables_init(parameter_namespace_t *aseba_ns)
{
    /* Initializes constant variables. */
    memset(&vmVariables, 0, sizeof(vmVariables));

    vmVariables.productId = ASEBA_PID_UNDEFINED;
    vmVariables.fwversion[0] = 0;
    vmVariables.fwversion[1] = 1;

    /* Register all event handling threads. */
    static THD_WORKING_AREA(range_wa, 256);
    chThdCreateStatic(range_wa, sizeof(range_wa), NORMALPRIO, aseba_range_thd, NULL);

    static THD_WORKING_AREA(proximity_wa, 256);
    chThdCreateStatic(proximity_wa, sizeof(proximity_wa), NORMALPRIO, aseba_proximity_thd, NULL);

    static THD_WORKING_AREA(encoders_wa, 256);
    chThdCreateStatic(encoders_wa, sizeof(encoders_wa), NORMALPRIO, aseba_encoders_thd, NULL);

    static THD_WORKING_AREA(imu_wa, 256);
    chThdCreateStatic(imu_wa, sizeof(imu_wa), NORMALPRIO, aseba_imu_thd, NULL);

    static THD_WORKING_AREA(audio_wa, 256);
    chThdCreateStatic(audio_wa, sizeof(audio_wa), NORMALPRIO, aseba_audio_thd, NULL);

    /* Start the virtual timer */
    static virtual_timer_t aseba_timer;
    chVTSet(&aseba_timer, MS2ST(100), aseba_timer_cb, (void *)&aseba_timer);

    /* Registers all Aseba settings in global namespace. */
    int i;

    /* Must be in descending order to keep them sorted on display. */
    for (i = SETTINGS_COUNT - 1; i >= 0; i--) {
        sprintf(aseba_settings_name[i], "%d", i);
        parameter_integer_declare_with_default(&aseba_settings[i],
                                               aseba_ns,
                                               aseba_settings_name[i],
                                               0);
    }
}

void aseba_read_variables_from_system(AsebaVMState *vm)
{
    messagebus_topic_t *topic;

    vmVariables.id = vm->nodeId;

    /* Read battery level */
    topic = messagebus_find_topic(&bus, "/battery_level");
    if (topic != NULL) {
        battery_msg_t msg;
        messagebus_topic_read(topic, &msg, sizeof(msg));
        vmVariables.battery_mv = (int)(1000 * msg.voltage);
    }

    /* Read range sensor */
    topic = messagebus_find_topic(&bus, "/range");
    if (topic != NULL) {
        range_msg_t range;
        messagebus_topic_read(topic, &range, sizeof(range));
        vmVariables.range = (int)range.raw_mm;
    }

    /* Read proximity sensors. */
    topic = messagebus_find_topic(&bus, "/proximity");
    if (topic != NULL) {
        proximity_msg_t proximity;
        messagebus_topic_read(topic, &proximity, sizeof(proximity));
        for (int i = 0; i < PROXIMITY_NB_CHANNELS; i++) {
            vmVariables.proximity_delta[i] = proximity.delta[i];
            vmVariables.proximity_ambient[i] = proximity.ambient[i];
            vmVariables.proximity_reflected[i] = proximity.reflected[i];
        }
    }

    /* Read encoders. */
    topic = messagebus_find_topic(&bus, "/encoders");
    if (topic != NULL) {
        encoders_msg_t msg;
        messagebus_topic_read(topic, &msg, sizeof(msg));

        vmVariables.motor_left_enc[0] = msg.left / INT16_MAX;
        vmVariables.motor_left_enc[1] = msg.left % INT16_MAX;

        vmVariables.motor_right_enc[0] = msg.right / INT16_MAX;
        vmVariables.motor_right_enc[1] = msg.right % INT16_MAX;
    }

    /* Read IMU. */
    topic = messagebus_find_topic(&bus, "/imu");
    if (topic != NULL) {
        imu_msg_t msg;
        messagebus_topic_read(topic, &msg, sizeof(msg));

        for (int i = 0; i < 3; i++) {
            vmVariables.acceleration[i] = msg.acceleration[i] * 1000;
            vmVariables.gyro[i] = msg.roll_rate[i] * 1000;
        }

        MadgwickAHRSupdateIMU(msg.roll_rate[0],msg.roll_rate[1],msg.roll_rate[2],
        msg.acceleration[0],msg.acceleration[1],msg.acceleration[2]);
        float R12 = 2*(q1*q2-q0*q3);
        float R22 = q0*q0-q1*q1+q2*q2-q3*q3;
        float R31 = 2*(q1*q3-q0*q2);
        float R32 = 2*(q2*q3+q0*q1);
        float R33 = q0*q0-q1*q1-q2*q2+q3*q3;
        float phi_angle = asin(R32)/(2*3.14154)*360;
        float theta_angle = 0;
        if(R33>0)
        {
            if(-R31>0){theta_angle=atan2(-R31,R33)/(2*3.14154)*360;}
            else{theta_angle=+atan2(-R31,R33)/(2*3.14154)*360;}
        }
        else if(R33<0)
        {
             if(-R31>0){theta_angle=180-atan2(-R31,R33)/(2*3.14154)*360;}
            else{theta_angle=-180-atan2(-R31,R33)/(2*3.14154)*360;}
        }
        else if (R33==0)
        {
            if(-R31>0){theta_angle=90;}
            else{theta_angle=-90;}
        }

        float psi_angle = atan2(-R12,R22)/(2*3.14154)*360;

        float theta_mean=0;
        float phi_mean =0;
        float psi_mean =0;

        static float tab_theta[MEAN_LENGTH];
        static float tab_phi[MEAN_LENGTH];
        static float tab_psi[MEAN_LENGTH];

         for (int i = MEAN_LENGTH-2; i >= 0; i--) {
         tab_theta [i+1] =tab_theta[i];
         tab_phi[i+1] =tab_phi[i];
         tab_psi[i+1] =tab_psi[i];
         theta_mean+=tab_theta [i+1];
         phi_mean+=tab_phi [i+1];
         psi_mean+=tab_psi [i+1];
         }

         theta_mean =(theta_mean+theta_angle)/MEAN_LENGTH;
         phi_mean=(phi_mean+phi_angle)/MEAN_LENGTH;
         psi_mean=(psi_mean+psi_angle)/MEAN_LENGTH;


         tab_theta [0]=theta_angle;
         tab_phi [0]=phi_angle;
         tab_psi[0]=psi_angle;

        vmVariables.theta=theta_mean;
        vmVariables.phi=phi_mean;
        vmVariables.psi=psi_mean;
    }

    /* Read motor current */
    topic = messagebus_find_topic(&bus, "/motors/current");
    if (topic != NULL) {
        motor_current_msg_t msg;
        messagebus_topic_read(topic, &msg, sizeof(msg));

        /* Convert current to mA. */
        vmVariables.motor_left_current = msg.left * 1000;
        vmVariables.motor_right_current = msg.right * 1000;
    }

    /* Read velocities */
    topic = messagebus_find_topic(&bus, "/wheel_velocities");
    if (topic != NULL) {
        wheel_velocities_msg_t msg;
        messagebus_topic_read(topic, &msg, sizeof(msg));
        vmVariables.motor_left_velocity = msg.left * 180 / M_PI;
        vmVariables.motor_right_velocity = msg.right * 180 / M_PI;
    }

    topic = messagebus_find_topic(&bus, "/wheel_pos");
    if (topic != NULL) {
        wheel_pos_msg_t msg;
        messagebus_topic_read(topic, &msg, sizeof(msg));
        vmVariables.motor_left_position = msg.left * 180 / M_PI;
        vmVariables.motor_right_position = msg.right * 180 / M_PI;
    }


    topic = messagebus_find_topic(&bus, "/motors/setpoint");
    if (topic != NULL) {
        wheels_setpoint_t msg;
        messagebus_topic_read(topic, &msg, sizeof(msg));
        vmVariables.motor_left_current_setpoint = msg.left * 1000;
        vmVariables.motor_right_current_setpoint = msg.right * 1000;
    }

    /* Read control parameters. */
    READ_PARAM_TO_VM(control_left_current_kp, "/left_wheel/control/current/kp");
    READ_PARAM_TO_VM(control_left_current_ki, "/left_wheel/control/current/ki");
    READ_PARAM_TO_VM(control_left_current_kd, "/left_wheel/control/current/kd");
    READ_PARAM_TO_VM(control_left_current_ilimit, "/left_wheel/control/current/i_limit");
    READ_PARAM_TO_VM(control_left_velocity_kp, "/left_wheel/control/velocity/kp");
    READ_PARAM_TO_VM(control_left_velocity_ki, "/left_wheel/control/velocity/ki");
    READ_PARAM_TO_VM(control_left_velocity_kd, "/left_wheel/control/velocity/kd");
    READ_PARAM_TO_VM(control_left_velocity_ilimit, "/left_wheel/control/velocity/i_limit");
    READ_PARAM_TO_VM(control_left_position_kp, "/left_wheel/control/position/kp");
    READ_PARAM_TO_VM(control_left_position_ki, "/left_wheel/control/position/ki");
    READ_PARAM_TO_VM(control_left_position_kd, "/left_wheel/control/position/kd");
    READ_PARAM_TO_VM(control_left_position_ilimit, "/left_wheel/control/position/i_limit");

    READ_PARAM_TO_VM(control_right_current_kp, "/right_wheel/control/current/kp");
    READ_PARAM_TO_VM(control_right_current_ki, "/right_wheel/control/current/ki");
    READ_PARAM_TO_VM(control_right_current_kd, "/right_wheel/control/current/kd");
    READ_PARAM_TO_VM(control_right_current_ilimit, "/right_wheel/control/current/i_limit");
    READ_PARAM_TO_VM(control_right_velocity_kp, "/right_wheel/control/velocity/kp");
    READ_PARAM_TO_VM(control_right_velocity_ki, "/right_wheel/control/velocity/ki");
    READ_PARAM_TO_VM(control_right_velocity_kd, "/right_wheel/control/velocity/kd");
    READ_PARAM_TO_VM(control_right_velocity_ilimit, "/right_wheel/control/velocity/i_limit");
    READ_PARAM_TO_VM(control_right_position_kp, "/right_wheel/control/position/kp");
    READ_PARAM_TO_VM(control_right_position_ki, "/right_wheel/control/position/ki");
    READ_PARAM_TO_VM(control_right_position_kd, "/right_wheel/control/position/kd");
    READ_PARAM_TO_VM(control_right_position_ilimit, "/right_wheel/control/position/i_limit");


    /* Store previous state of variables. */
    memcpy(&previous_vars, &vmVariables, sizeof(vmVariables));
}

void aseba_write_variables_to_system(AsebaVMState *vm)
{
    ASEBA_UNUSED(vm);

    /* If the motor PWM changed, apply the new one. This is allows setting the
     * PWM from C without Aseba interfering. */
    if (VM_VAR_CHANGED(motor_left_pwm)) {
        motor_left_pwm_set(vmVariables.motor_left_pwm / 100.);
    }

    if (VM_VAR_CHANGED(motor_right_pwm)) {
        motor_right_pwm_set(vmVariables.motor_right_pwm / 100.);
    }

    /* Did the current setpoint change ? If yes, switch to current control mode. */
    if (VM_VAR_CHANGED(motor_right_current_setpoint) ||
        VM_VAR_CHANGED(motor_left_current_setpoint)) {
        wheels_setpoint_t msg;
        msg.mode = MOTOR_CONTROLLER_CURRENT;

        /* Convert setpoints from mA to A. */
        msg.left = vmVariables.motor_left_current_setpoint / 1000.;
        msg.right = vmVariables.motor_right_current_setpoint / 1000.;

        messagebus_topic_t *topic = messagebus_find_topic(&bus, "/motors/setpoint");

        messagebus_topic_publish(topic, &msg, sizeof(msg));
    }

    /* Did the velocity setpoint change ? If yes, switch to velocity control mode. */
    if (VM_VAR_CHANGED(motor_right_velocity_setpoint) ||
        VM_VAR_CHANGED(motor_left_velocity_setpoint)) {
        wheels_setpoint_t msg;
        msg.mode = MOTOR_CONTROLLER_VELOCITY;

        /* Convert setpoints from deg/s to rads/s */
        msg.left = (3.14 / 180.) * vmVariables.motor_left_velocity_setpoint;
        msg.right = (3.14 / 180.) * vmVariables.motor_right_velocity_setpoint;

        messagebus_topic_t *topic = messagebus_find_topic(&bus, "/motors/setpoint");

        messagebus_topic_publish(topic, &msg, sizeof(msg));
    }

    /* Did the position setpoint change ? If yes, switch to position control mode. */
    if (VM_VAR_CHANGED(motor_right_position_setpoint) ||
        VM_VAR_CHANGED(motor_left_position_setpoint)) {
        wheels_setpoint_t msg;
        msg.mode = MOTOR_CONTROLLER_POSITION;

        /* Convert setpoints from degs to rads */
        msg.left = (3.14 / 180.) * vmVariables.motor_left_position_setpoint;
        msg.right = (3.14 / 180.) * vmVariables.motor_right_position_setpoint;

        messagebus_topic_t *topic = messagebus_find_topic(&bus, "/motors/setpoint");

        messagebus_topic_publish(topic, &msg, sizeof(msg));
    }



    for (int i = 0; i < BODY_LED_COUNT; i++) {
        if (VM_VAR_CHANGED(leds[i])) {
            char name[32];
            sprintf(name, "/body_leds/%d", i);

            messagebus_topic_t *topic = messagebus_find_topic(&bus, name);
            if (topic != NULL) {
                body_led_msg_t msg;
                msg.value = vmVariables.leds[i] / 100.;
                messagebus_topic_publish(topic, &msg, sizeof(msg));
            }
        }
    }

    /* Write back parameters. */
    WRITE_PARAM_FROM_VM(control_left_current_kp, "/left_wheel/control/current/kp");
    WRITE_PARAM_FROM_VM(control_left_current_ki, "/left_wheel/control/current/ki");
    WRITE_PARAM_FROM_VM(control_left_current_kd, "/left_wheel/control/current/kd");
    WRITE_PARAM_FROM_VM(control_left_current_ilimit, "/left_wheel/control/current/i_limit");
    WRITE_PARAM_FROM_VM(control_left_velocity_kp, "/left_wheel/control/velocity/kp");
    WRITE_PARAM_FROM_VM(control_left_velocity_ki, "/left_wheel/control/velocity/ki");
    WRITE_PARAM_FROM_VM(control_left_velocity_kd, "/left_wheel/control/velocity/kd");
    WRITE_PARAM_FROM_VM(control_left_velocity_ilimit, "/left_wheel/control/velocity/i_limit");
    WRITE_PARAM_FROM_VM(control_left_position_kp, "/left_wheel/control/position/kp");
    WRITE_PARAM_FROM_VM(control_left_position_ki, "/left_wheel/control/position/ki");
    WRITE_PARAM_FROM_VM(control_left_position_kd, "/left_wheel/control/position/kd");
    WRITE_PARAM_FROM_VM(control_left_position_ilimit, "/left_wheel/control/position/i_limit");

    WRITE_PARAM_FROM_VM(control_right_current_kp, "/right_wheel/control/current/kp");
    WRITE_PARAM_FROM_VM(control_right_current_ki, "/right_wheel/control/current/ki");
    WRITE_PARAM_FROM_VM(control_right_current_kd, "/right_wheel/control/current/kd");
    WRITE_PARAM_FROM_VM(control_right_current_ilimit, "/right_wheel/control/current/i_limit");
    WRITE_PARAM_FROM_VM(control_right_velocity_kp, "/right_wheel/control/velocity/kp");
    WRITE_PARAM_FROM_VM(control_right_velocity_ki, "/right_wheel/control/velocity/ki");
    WRITE_PARAM_FROM_VM(control_right_velocity_kd, "/right_wheel/control/velocity/kd");
    WRITE_PARAM_FROM_VM(control_right_velocity_ilimit, "/right_wheel/control/velocity/i_limit");
    WRITE_PARAM_FROM_VM(control_right_position_kp, "/right_wheel/control/position/kp");
    WRITE_PARAM_FROM_VM(control_right_position_ki, "/right_wheel/control/position/ki");
    WRITE_PARAM_FROM_VM(control_right_position_kd, "/right_wheel/control/position/kd");
    WRITE_PARAM_FROM_VM(control_right_position_ilimit, "/right_wheel/control/position/i_limit");


}

// Native functions
static AsebaNativeFunctionDescription AsebaNativeDescription__system_reboot =
{
    "_system.reboot",
    "Reboot the microcontroller",
    {
        {0, 0}
    }
};

void AsebaNative__system_reboot(AsebaVMState *vm)
{
    ASEBA_UNUSED(vm);
    NVIC_SystemReset();
}

static AsebaNativeFunctionDescription AsebaNativeDescription__system_settings_read =
{
    "_system.settings.read",
    "Read a setting",
    {
        { 1, "address"},
        { 1, "value"},
        { 0, 0 }
    }
};

static void AsebaNative__system_settings_read(AsebaVMState *vm)
{
    uint16 address = vm->variables[AsebaNativePopArg(vm)];
    uint16 destidx = AsebaNativePopArg(vm);
    if (address < SETTINGS_COUNT) {
        vm->variables[destidx] = parameter_integer_get(&aseba_settings[address]);
    } else {
        AsebaVMEmitNodeSpecificError(vm, "Invalid settings address.");
    }
}

static AsebaNativeFunctionDescription AsebaNativeDescription__system_settings_write =
{
    "_system.settings.write",
    "Write a setting",
    {
        { 1, "address"},
        { 1, "value"},
        { 0, 0 }
    }
};

static void AsebaNative__system_settings_write(AsebaVMState *vm)
{
    uint16 address = vm->variables[AsebaNativePopArg(vm)];
    uint16 value = vm->variables[AsebaNativePopArg(vm)];

    if (address < SETTINGS_COUNT) {
        parameter_integer_set(&aseba_settings[address], value);
    } else {
        AsebaVMEmitNodeSpecificError(vm, "Invalid settings address.");
    }
}


static AsebaNativeFunctionDescription AsebaNativeDescription_settings_save =
{
    "_system.settings.flash",
    "Save settings into flash",
    {
        {0, 0}
    }
};

void AsebaNative_settings_save(AsebaVMState *vm)
{
    extern uint32_t _config_start, _config_end;
    size_t len = (size_t)(&_config_end - &_config_start);
    bool success;

    // First write the config to flash
    config_save(&_config_start, len, &parameter_root);

    // Second try to read it back, see if we failed
    success = config_load(&parameter_root, &_config_start);

    if (!success) {
        AsebaVMEmitNodeSpecificError(vm, "Config save failed!");
    }
}

static AsebaNativeFunctionDescription AsebaNativeDescription_settings_erase =
{
    "_system.settings.erase",
    "Restore settings to default value (erases flash)",
    {
        {0, 0}
    }
};


void AsebaNative_settings_erase(AsebaVMState *vm)
{
    ASEBA_UNUSED(vm);

    extern uint32_t _config_start;

    config_erase(&_config_start);
}

static AsebaNativeFunctionDescription AsebaNativeDescription_sound_play =
{
    "sound.play",
    "Play pN.wav from the SD card",
    {
        {1, "Index of the sound to play"},
        {0, 0}
    }
};

void AsebaNative_sound_play(AsebaVMState *vm)
{
    uint16 sound_id = vm->variables[AsebaNativePopArg(vm)];

    messagebus_topic_t *req_topic, *res_topic;
    req_topic = messagebus_find_topic(&bus, "/audio/play/request");
    res_topic = messagebus_find_topic(&bus, "/audio/play/result");

    if (req_topic == NULL || res_topic == NULL) {
        AsebaVMEmitNodeSpecificError(vm, "Cannot find topics. Is audio thread running?");
        return;
    }

    /* Send the request */
    audio_play_request_t request;
    memset(&request, 0, sizeof(request));
    snprintf(request.path, sizeof(request.path) - 1, "/p%d.wav", sound_id);
    messagebus_topic_publish(req_topic, &request, sizeof(request));
}

// Native function descriptions
const AsebaNativeFunctionDescription* nativeFunctionsDescription[] = {
    &AsebaNativeDescription__system_reboot,
    &AsebaNativeDescription__system_settings_read,
    &AsebaNativeDescription__system_settings_write,
    &AsebaNativeDescription_settings_save,
    &AsebaNativeDescription_settings_erase,
    &AsebaNativeDescription_sound_play,
    ASEBA_NATIVES_STD_DESCRIPTIONS,
    0
};

// Native function pointers
AsebaNativeFunctionPointer nativeFunctions[] = {
    AsebaNative__system_reboot,
    AsebaNative__system_settings_read,
    AsebaNative__system_settings_write,
    AsebaNative_settings_save,
    AsebaNative_settings_erase,
    AsebaNative_sound_play,
    ASEBA_NATIVES_STD_FUNCTIONS,
};

const int nativeFunctions_length = sizeof(nativeFunctions) / sizeof(nativeFunctions[0]);
