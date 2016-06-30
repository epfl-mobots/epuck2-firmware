#include <string.h>
#include <stdio.h>

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

/* Struct used to share Aseba parameters between C-style API and Aseba. */
static parameter_t aseba_settings[SETTINGS_COUNT];
static char aseba_settings_name[SETTINGS_COUNT][10];

struct _vmVariables vmVariables;

/* Used to detect if a PWM value has changed. */
static sint16 motor_pwm_previous_left, motor_pwm_previous_right;

void AsebaVMResetCB(AsebaVMState *vm)
{
    ASEBA_UNUSED(vm);

    vmVariables.motor_right_pwm = 0;
    vmVariables.motor_left_pwm = 0;
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

     {2, "motor.left.enc"},
     {2, "motor.right.enc"},

     {0, NULL}
}
};

// Event descriptions
const AsebaLocalEventDescription localEvents[] = {
    {"button", "User button clicked"},
    {"range", "New range measurement"},
    {"proximity", "New proximity measurement"},
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
        SET_EVENT(EVENT_RANGE);
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
        SET_EVENT(EVENT_PROXIMITY);
    }
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

    /* Keep previous value of PWM. */
    motor_pwm_previous_left = vmVariables.motor_left_pwm;
    motor_pwm_previous_right = vmVariables.motor_right_pwm;
}

void aseba_write_variables_to_system(AsebaVMState *vm)
{
    ASEBA_UNUSED(vm);

    /* If the motor PWM changed, apply the new one. This is allows setting the
     * PWM from C without Aseba interfering. */
    if (vmVariables.motor_left_pwm != motor_pwm_previous_left) {
        motor_left_pwm_set(vmVariables.motor_left_pwm / 100.);
    }

    if (vmVariables.motor_right_pwm != motor_pwm_previous_right) {
        motor_right_pwm_set(vmVariables.motor_right_pwm / 100.);
    }
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

// Native function descriptions
const AsebaNativeFunctionDescription* nativeFunctionsDescription[] = {
    &AsebaNativeDescription__system_reboot,
    &AsebaNativeDescription__system_settings_read,
    &AsebaNativeDescription__system_settings_write,
    &AsebaNativeDescription_settings_save,
    &AsebaNativeDescription_settings_erase,
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
    ASEBA_NATIVES_STD_FUNCTIONS,
};

const int nativeFunctions_length = sizeof(nativeFunctions) / sizeof(nativeFunctions[0]);
