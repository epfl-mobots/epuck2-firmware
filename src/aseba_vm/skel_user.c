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

#include "sensors/range.h"

/* Struct used to share Aseba parameters between C-style API and Aseba. */
static parameter_t aseba_settings[SETTINGS_COUNT];
static char aseba_settings_name[SETTINGS_COUNT][10];

struct _vmVariables vmVariables;


const AsebaVMDescription vmDescription = {
    BOARD_NAME,
    {
     // {Number of element in array, Name displayed in aseba studio}
     {1, "_id"},
     {1, "event.source"},
     {VM_VARIABLES_ARG_SIZE, "event.args"},
     {2, "_fwversion"},
     {1, "_productId"},

     {1, "range"},

     {0, NULL}
}
};

// Event descriptions
const AsebaLocalEventDescription localEvents[] = {
    {"button", "User button clicked"},
    {NULL, NULL}
};

void aseba_variables_init(parameter_namespace_t *aseba_ns)
{
    /* Initializes constant variables. */
    memset(&vmVariables, 0, sizeof(vmVariables));

    vmVariables.productId = ASEBA_PID_UNDEFINED;
    vmVariables.fwversion[0] = 0;
    vmVariables.fwversion[1] = 1;

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
    range_t range;
    messagebus_topic_t *topic;

    vmVariables.id = vm->nodeId;

    topic = messagebus_find_topic(&bus, "/range");
    if (topic != NULL) {
        messagebus_topic_read(topic, &range, sizeof(range));
        vmVariables.range = (int)range.raw_mm;
    }
}

void aseba_write_variables_to_system(AsebaVMState *vm)
{
    ASEBA_UNUSED(vm);
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
